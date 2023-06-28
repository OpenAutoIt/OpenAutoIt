#include "OpenAutoIt/Parser.hpp"

#include "OpenAutoIt/AST.hpp"
#include "OpenAutoIt/Associativity.hpp"
#include "OpenAutoIt/DiagnosticBuilder.hpp"
#include "OpenAutoIt/DiagnosticEngine.hpp"
#include "OpenAutoIt/DiagnosticIds.hpp"
#include "OpenAutoIt/IncludeType.hpp"
#include "OpenAutoIt/SourceFile.hpp"
#include "OpenAutoIt/SourceLocation.hpp"
#include "OpenAutoIt/SourceManager.hpp"
#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include "OpenAutoIt/VariableScope.hpp"
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/unused.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/move.hpp>
#include <phi/core/narrow_cast.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/scope_ptr.hpp>
#include <phi/core/size_t.hpp>
#include <phi/core/sized_types.hpp>
#include <phi/core/types.hpp>
#include <phi/text/hex_digit_value.hpp>
#include <phi/text/is_digit.hpp>
#include <phi/text/is_hex_digit.hpp>
#include <phi/type_traits/to_underlying.hpp>
#include <phi/type_traits/underlying_type.hpp>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")

#include <fmt/format.h>

PHI_GCC_SUPPRESS_WARNING_POP()

namespace OpenAutoIt
{

namespace
{
    constexpr const phi::usize MaxNumberOfIncludeNesting = 256u;
}

class OperatorPrecedenceTable
{
public:
    constexpr OperatorPrecedenceTable()
    {
        for (const auto& op : m_OperatorPrecedence)
        {
            m_TokenPrecedence[static_cast<size_t>(op.token)] = op.precedence;
        }
    }

    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4702) // Unreachable code

    [[nodiscard]] constexpr int lookup(TokenKind token) const
    {
        int precedence = m_TokenPrecedence[static_cast<size_t>(token)];
        //PHI_ASSERT(precedence != 0);

        return precedence;
    }

    PHI_MSVC_SUPPRESS_WARNING_POP()

private:
    int m_TokenPrecedence[NumberOfTokens]{};

    struct OperatorPrecedence
    {
        TokenKind token;
        int       precedence{-1};
    };

    // https://www.autoitscript.com/autoit3/docs/intro/lang_operators.htm
    static constexpr const OperatorPrecedence m_OperatorPrecedence[] = {
            // Precedence 8
            {TokenKind::LParen, 80}, // (

            // Precedence 7
            {TokenKind::KW_Not, 70}, // Not

            // Precedence 6
            {TokenKind::OP_Raise, 60}, // ^

            // Precedence 5
            {TokenKind::OP_Multiply, 50}, // *
            {TokenKind::OP_Divide, 50},   // /

            // Precedence 4
            {TokenKind::OP_Plus, 40},  // +
            {TokenKind::OP_Minus, 40}, // -

            // Precedence 3
            {TokenKind::OP_Concatenate, 30}, // &

            // Precedence 2
            {TokenKind::OP_LessThan, 20},         // <
            {TokenKind::OP_LessThanEqual, 20},    // <=
            {TokenKind::OP_GreaterThan, 20},      // >
            {TokenKind::OP_GreaterThanEqual, 20}, // <=
            {TokenKind::OP_Equals, 20},           // =
            {TokenKind::OP_NotEqual, 20},         // <>
            {TokenKind::OP_EqualsEquals, 20},     // ==

            // Precedence 1
            {TokenKind::KW_Or, 10},  // Or
            {TokenKind::KW_And, 10}, // And
    };
};

constexpr OperatorPrecedenceTable OperatorPrecedence;

Parser::Parser(phi::not_null_observer_ptr<SourceManager>    source_manager,
               phi::not_null_observer_ptr<DiagnosticEngine> diagnostic_engine,
               phi::not_null_observer_ptr<Lexer>            lexer)
    : m_SourceManager{phi::move(source_manager)}
    , m_DiagnosticEngine{phi::move(diagnostic_engine)}
    , m_Lexer{phi::move(lexer)}
{}

void Parser::ParseTokenStream(phi::not_null_observer_ptr<ASTDocument>      document,
                              TokenStream&&                                stream,
                              phi::not_null_observer_ptr<const SourceFile> source_file)
{
    PushParsingContext(phi::move(source_file), phi::move(stream));

    ParseDocument(phi::move(document));
}

void Parser::ParseString(phi::not_null_observer_ptr<ASTDocument> document,
                         phi::string_view file_name, phi::string_view source)
{
    TokenStream stream = m_Lexer->ProcessString(file_name, source);

    SourceFile fake_source_file{SourceFile::Type::Basic, std::string_view(file_name),
                                phi::move(source)};
    ParseTokenStream(phi::move(document), phi::move(stream), &fake_source_file);
}

void Parser::ParseFile(phi::not_null_observer_ptr<ASTDocument> document,
                       const phi::string_view                  path)
{
    phi::observer_ptr<const SourceFile> source_file =
            m_SourceManager->LoadFile(path, IncludeType::Local);
    if (!source_file)
    {
        Diag().FatalError(DiagnosticId::FileNotFound, SourceLocation::Invalid(),
                          std::string_view(path));
        return;
    }

    TokenStream stream = m_Lexer->ProcessFile(source_file.not_null());

    ParseTokenStream(phi::move(document), phi::move(stream), source_file.not_null());
}

void Parser::ParseDocument(phi::not_null_observer_ptr<ASTDocument> document)
{
    m_Document = phi::move(document);

    m_IncludeOnceFiles.clear();

    while (ShouldContinueParsing())
    {
        if (!CurrentTokenStream().has_more())
        {
            PopParsingContext();
            continue;
        }

        const Token& token = CurrentToken();

        // Parse global function definition
        switch (token.GetTokenKind())
        {
            case TokenKind::KW_Func: {
                ConsumeCurrent();

                auto function_definition = ParseFunctionDefinition();
                if (!function_definition)
                {
                    err("ERR: Failed to parse function definition!\n");
                    continue;
                }

                AppendFunctionToDocument(function_definition.release_not_null());
                break;
            }

            case TokenKind::Comment:
            case TokenKind::PP_CommentsStart:
            case TokenKind::PP_CommentsEnd:
            case TokenKind::NewLine: {
                // Simply ignore and consume newlines and comments
                ConsumeCurrent();
                break;
            }
            case TokenKind::NotAToken: {
                err(fmt::format("ERR: Unexpected NotAToken with text '{:s}'!\n",
                                std::string_view(token.GetText())));
                ConsumeCurrent();
                break;
            }

            case TokenKind::PP_Include: {
                ConsumeCurrent();

                ParseIncludeDirective();

                break;
            }

            case TokenKind::PP_IncludeOnce: {
                ConsumeCurrent();

                m_IncludeOnceFiles.emplace(CurrentSourceFile().get());

                break;
            }

            default: {
                auto statement = ParseStatement();
                if (!statement)
                {
                    // TODO: Proper error reporting
                    err("ERR: Failed to parse statement!\n");

                    if (HasMoreTokens())
                    {
                        // Swallow the bad token
                        ConsumeCurrent();
                    }
                    continue;
                }

                AppendStatementToDocument(statement.release_not_null());
                break;
            }
        }
    }
}

void Parser::PushParsingContext(phi::not_null_observer_ptr<const SourceFile> source_file,
                                TokenStream&&                                token_stream)
{
    PushParsingContext(phi::move(source_file), phi::move(token_stream), SourceLocation::Invalid());
}

void Parser::PushParsingContext(phi::not_null_observer_ptr<const SourceFile> source_file,
                                TokenStream&& token_stream, SourceLocation included_from)
{
    ParsingContext context{.source_file   = phi::move(source_file),
                           .token_stream  = phi::move(token_stream),
                           .included_from = phi::move(included_from)};

    m_ParsingContextStack.emplace(phi::move(context));
    m_SourceManager->SetLocalSearchPath(source_file->m_FilePath.parent_path());
}

void Parser::PopParsingContext()
{
    m_ParsingContextStack.pop();
    if (!m_ParsingContextStack.empty())
    {
        m_SourceManager->SetLocalSearchPath(
                m_ParsingContextStack.top().source_file->m_FilePath.parent_path());
    }
    else
    {
        m_SourceManager->SetLocalSearchPath("");
    }
}

ParsingContext& Parser::CurrentParsingContext()
{
    PHI_ASSERT(!m_ParsingContextStack.empty());

    return m_ParsingContextStack.top();
}

phi::not_null_observer_ptr<const SourceFile> Parser::CurrentSourceFile()
{
    return CurrentParsingContext().source_file.not_null();
}

TokenStream& Parser::CurrentTokenStream()
{
    PHI_ASSERT(!m_ParsingContextStack.empty());

    return m_ParsingContextStack.top().token_stream;
}

const TokenStream& Parser::CurrentTokenStream() const
{
    PHI_ASSERT(!m_ParsingContextStack.empty());

    return m_ParsingContextStack.top().token_stream;
}

phi::boolean Parser::HasMoreTokens() const
{
    return CurrentTokenStream().has_more();
}

const Token& Parser::CurrentToken() const
{
    PHI_ASSERT(CurrentTokenStream().has_more());

    return CurrentTokenStream().look_ahead();
}

const Token& Parser::PreviousToken() const
{
    return CurrentTokenStream().look_behind();
}

phi::boolean Parser::ShouldContinueParsing() const
{
    const phi::boolean stack_empty          = m_ParsingContextStack.empty();
    const phi::boolean fatal_error_occurred = m_DiagnosticEngine->HasFatalErrorOccurred();
    const phi::boolean error_limit_reached =
            (m_DiagnosticEngine->GetErrorLimit() == 0u ?
                     false :
                     m_DiagnosticEngine->GetNumberOfError() >= m_DiagnosticEngine->GetErrorLimit());

    return !stack_empty && !fatal_error_occurred && !error_limit_reached;
}

void Parser::ConsumeCurrent()
{
    CurrentTokenStream().consume();
}

void Parser::ConsumeComments()
{
    while (HasMoreTokens())
    {
        switch (CurrentToken().GetTokenKind())
        {
            case TokenKind::Comment:
            case TokenKind::PP_CommentsStart:
            case TokenKind::PP_CommentsEnd:
                ConsumeCurrent();
                break;

            default:
                return;
        }
    }
}

void Parser::ConsumeNewLineAndComments()
{
    while (HasMoreTokens())
    {
        switch (CurrentToken().GetTokenKind())
        {
            case TokenKind::Comment:
            case TokenKind::PP_CommentsStart:
            case TokenKind::PP_CommentsEnd:
            case TokenKind::NewLine:
                ConsumeCurrent();
                break;

            default:
                return;
        }
    }
}

phi::optional<const Token&> Parser::MustParse(TokenKind kind)
{
    // Do we even have more tokens?
    if (!HasMoreTokens())
    {
        return {};
    }

    const Token& token = CurrentToken();

    // Is this the correct token kind
    if (token.GetTokenKind() != kind)
    {
        return {};
    }

    ConsumeCurrent();
    return token;
}

void Parser::AppendSourceFileToDocument(phi::not_null_observer_ptr<const SourceFile> source_file,
                                        SourceLocation                               included_from)
{
    // Don't include the file if it's already included with #include-once set in the file
    if (m_IncludeOnceFiles.contains(source_file.get()))
    {
        return;
    }

    PushParsingContext(phi::move(source_file), m_Lexer->ProcessFile(source_file),
                       phi::move(included_from));
}

DiagnosticBuilder Parser::Diag()
{
    return {m_DiagnosticEngine};
}

phi::scope_ptr<ASTFunctionDefinition> Parser::ParseFunctionDefinition()
{
    // Next we MUST parse the function name
    auto function_name_token = MustParse(TokenKind::FunctionIdentifier);
    if (!function_name_token)
    {
        err("Expected identifier for function name");
        return {};
    }

    auto function_definition            = phi::make_scope<ASTFunctionDefinition>();
    function_definition->m_FunctionName = function_name_token->GetText();

    // Next we MUST parse an opening parenthesis (LParen)
    if (!MustParse(TokenKind::LParen))
    {
        err("Expected opening parenthesis");
        return {};
    }

    // Next we parse the function parameter declarations until right parenthesis (RParen)
    while (HasMoreTokens() && CurrentToken().GetTokenKind() != TokenKind::RParen)
    {
        phi::optional<FunctionParameter> function_parameter_optional =
                ParseFunctionParameterDefinition();
        if (!function_parameter_optional)
        {
            // TODO: Proper error reporting
            return {};
        }
        FunctionParameter& function_parameter = function_parameter_optional.value();

        function_definition->m_Parameters.emplace_back(phi::move(function_parameter));

        // Parse comma
        if (HasMoreTokens() && CurrentToken().GetTokenKind() == TokenKind::Comma)
        {
            ConsumeCurrent();
        }
    }

    // Next we MUST parse a right parenthesis (RParen)
    if (!MustParse(TokenKind::RParen))
    {
        err("ERR: Expected ')'");
        return {};
    }

    // Next we MUST parse a new line
    if (!MustParse(TokenKind::NewLine))
    {
        err("ERR: Missing newline!");
        return {};
    }

    // Next parse Statements until EndFunc
    while (HasMoreTokens() && CurrentToken().GetTokenKind() != TokenKind::KW_EndFunc)
    {
        auto statement = ParseStatement();
        if (!statement)
        {
            err(fmt::format("ERR: Failed while parsing statement for function \"{:s}\"\n",
                            std::string_view(function_definition->m_FunctionName)));
            // TODO: Report proper error
            return {};
        }

        function_definition->m_FunctionBody.emplace_back(phi::move(statement.release_not_null()));

        ConsumeNewLineAndComments();
    }

    // Next we MUST parse EndFunc
    if (!MustParse(TokenKind::KW_EndFunc))
    {
        // TODO: Proper error
        return {};
    }

    return phi::move(function_definition);
}

phi::optional<FunctionParameter> Parser::ParseFunctionParameterDefinition()
{
    // TODO: This entire function requires more error checks
    FunctionParameter parameter;

    while (HasMoreTokens())
    {
        const Token& token = CurrentToken();

        switch (token.GetTokenKind())
        {
            case TokenKind::VariableIdentifier: {
                parameter.name = token.GetText().substring_view(1u);
                ConsumeCurrent();
                break;
            }
            case TokenKind::KW_Const: {
                parameter.as_const = true;
                ConsumeCurrent();
                break;
            }
            case TokenKind::KW_ByRef: {
                parameter.by_ref = true;
                ConsumeCurrent();
                break;
            }
            case TokenKind::OP_Equals: {
                if (parameter.name.is_empty())
                {
                    // TODO: PROPER ERROR
                    return {};
                }

                // Consume the '='
                ConsumeCurrent();

                // Default value is an expression
                auto default_expression = ParseExpression();
                if (!default_expression)
                {
                    // TODO: Report error
                    return {};
                }

                // For default values we artificially create a variable assignment
                auto default_var_assignment = phi::make_not_null_scope<ASTVariableAssignment>();

                default_var_assignment->m_Scope                  = VariableScope::Auto;
                default_var_assignment->m_VariableName           = parameter.name;
                default_var_assignment->m_InitialValueExpression = phi::move(default_expression);

                parameter.default_value_init.emplace_back(phi::move(default_var_assignment));
                break;
            }
            case TokenKind::Comma:
            case TokenKind::RParen: {
                return phi::move(parameter);
            }
            default: {
                // TODO: Report error
                return {};
            }
        }
    }

    // TODO: Report error unexpected EOF
    return {};
}

void Parser::ParseIncludeDirective()
{
    if (!HasMoreTokens())
    {
        Diag().Error(DiagnosticId::UnexpectedEndOfFile, PreviousToken().GetBeginLocation(),
                     "include directive");
        return;
    }

    const Token&     token = CurrentToken();
    phi::string_view file_name;
    IncludeType      include_type = IncludeType::Local;

    // Local include like this '#include "foo.au3"'
    if (token.GetTokenKind() == TokenKind::StringLiteral)
    {
        ConsumeCurrent();

        // Get the file name
        file_name = token.GetText().substring_view(1u, token.GetText().length() - 2u);
    }
    // Global include like this '#include <foo.au3>'
    else if (token.GetTokenKind() == TokenKind::OP_LessThan)
    {
        // FIXME: This is not really a nice way, but we essentially start from after the opening '<' and include everything upto the closing '>'
        ConsumeCurrent();

        include_type = IncludeType::Global;
        file_name    = token.GetText().remove_prefix(1u);

        phi::boolean found_end{false};
        phi::boolean continue_parsing{true};
        while (HasMoreTokens() && continue_parsing)
        {
            const Token& end_token = CurrentToken();

            if (end_token.GetTokenKind() == TokenKind::OP_GreaterThan)
            {
                found_end        = true;
                continue_parsing = false;
                const phi::usize difference =
                        phi::narrow_cast<phi::usize>(end_token.GetText().data() - file_name.data());
                file_name.add_postfix(difference);
            }
            else if (end_token.GetTokenKind() == TokenKind::NewLine)
            {
                // Stop once we hit a newline
                continue_parsing = false;
            }

            ConsumeCurrent();
        }

        if (!found_end)
        {
            Diag().Error(DiagnosticId::Expected, PreviousToken().GetEndLocation(), "'>'")
                    .Note(token.GetBeginLocation(), "to match this '<'");
            return;
        }
    }
    else
    {
        Diag().Error(DiagnosticId::Expected, token.GetEndLocation(), "\"Filename\" or <Filename>");
        return;
    }

    // Limit include nesting
    if (m_ParsingContextStack.size() >= MaxNumberOfIncludeNesting)
    {
        Diag().FatalError(DiagnosticId::IncludeNestingTooDeeply, token.GetBeginLocation());
        return;
    }

    // Emit error for empty file names
    if (file_name.is_empty())
    {
        Diag().FatalError(DiagnosticId::EmptyFilename, token.GetBeginLocation());
        return;
    }

    // Emit error if the file name is too long
    if (file_name.length() > 255u)
    {
        Diag().FatalError(DiagnosticId::FileNameTooLong, token.GetBeginLocation(),
                          std::string_view(file_name));
        return;
    }

    // Load the file from the SourceManager
    phi::observer_ptr<const SourceFile> include_file =
            m_SourceManager->LoadFile(std::string_view(file_name), include_type);
    if (!include_file)
    {
        Diag().FatalError(DiagnosticId::FileNotFound, token.GetBeginLocation(),
                          std::string_view(file_name));
        return;
    }

    // Append the file
    AppendSourceFileToDocument(include_file.not_null(), token.GetBeginLocation());
}

phi::scope_ptr<ASTStatement> Parser::ParseStatement()
{
    ConsumeNewLineAndComments();

    if (!HasMoreTokens())
    {
        // TODO: Report proper error
        return {};
    }

    phi::scope_ptr<ASTStatement> ret_statement;

    // Loop until we parse something or there is nothing left to parse
    const Token& token = CurrentToken();
    switch (token.GetTokenKind())
    {
        // Variable assignment
        case TokenKind::KW_Const:
        case TokenKind::KW_Local:
        case TokenKind::KW_Global:
        case TokenKind::KW_Static:
        case TokenKind::VariableIdentifier: {
            ret_statement = ParseVariableAssignment();
            if (!ret_statement)
            {
                err("ERR: Failed to parse variable assignment!\n");
                return {};
            }
            break;
        }

        // If Statement
        case TokenKind::KW_If: {
            ret_statement = ParseIfStatement();
            if (!ret_statement)
            {
                err("ERR: Failed to parse if statement!\n");
                return {};
            }
            break;
        }

        // While statement
        case TokenKind::KW_While: {
            ret_statement = ParseWhileStatement();
            if (!ret_statement)
            {
                err("ERR: Failed to parse while statement!\n");
                return {};
            }
            break;
        }

        // Exit statement
        case TokenKind::KW_Exit: {
            ret_statement = ParseExitStatement();
            if (!ret_statement)
            {
                err("ERR: Failed to parse exit statement!\n");
                return {};
            }
            break;
        }

        default: {
            // Try to parse ExpressionStatement
            ret_statement = ParseExpressionStatement();
            if (!ret_statement)
            {
                err(fmt::format("ERR: Unexpected token: '{:s}'\n",
                                enum_name(token.GetTokenKind())));
                return {};
            }
            break;
        }
    }

    ConsumeComments();

    if (HasMoreTokens() && !MustParse(TokenKind::NewLine))
    {
        err("Requires newline after statement\n");
        return {};
    }

    return phi::move(ret_statement);
}

phi::scope_ptr<ASTWhileStatement> Parser::ParseWhileStatement()
{
    const Token& token = CurrentToken();
    if (token.GetTokenKind() != TokenKind::KW_While)
    {
        // TODO: Proper error
        return {};
    }
    ConsumeCurrent();

    // Next we MUST parse an Expression
    auto while_condition_expression = ParseExpression();
    if (!while_condition_expression)
    {
        // TODO: Proper error
        return {};
    }

    auto while_statement =
            phi::make_scope<ASTWhileStatement>(while_condition_expression.release_not_null());

    // Parse statements until KW_WEnd
    while (HasMoreTokens() && CurrentToken().GetTokenKind() != TokenKind::KW_WEnd)
    {
        ConsumeNewLineAndComments();

        // Parse statements
        auto statement = ParseStatement();
        if (!statement)
        {
            // TODO: Propeer error
            return {};
        }

        while_statement->m_Statements.emplace_back(statement.release_not_null());
    }

    if (!HasMoreTokens())
    {
        // TODO: Proper error
        return {};
    }

    // Next token MUST be KW_Wend
    const Token& wend_token = CurrentToken();
    if (wend_token.GetTokenKind() != TokenKind::KW_WEnd)
    {
        // TODO: Proper error
        return {};
    }
    ConsumeCurrent();

    return phi::move(while_statement);
}

phi::scope_ptr<ASTVariableAssignment> Parser::ParseVariableAssignment()
{
    auto variable_declaration = phi::make_scope<ASTVariableAssignment>();

    phi::boolean parsed_identifier = false;
    // Parse all specifiers until we hit a VariableIdentifier
    while (HasMoreTokens() && !parsed_identifier)
    {
        const Token& current_token = CurrentToken();
        ConsumeCurrent();

        switch (current_token.GetTokenKind())
        {
            case TokenKind::KW_Const: {
                if (variable_declaration->m_IsConst)
                {
                    // TODO: Error more than one const specifier
                    err("ERR: More than one const specifier given\n");
                    return {};
                }
                variable_declaration->m_IsConst = true;
                break;
            }

            case TokenKind::KW_Static: {
                if (variable_declaration->m_IsStatic)
                {
                    // TODO: Error more than one static specifier
                    err("ERR: More than one static specifier given\n");
                    return {};
                }
                variable_declaration->m_IsStatic = true;
                break;
            }

            case TokenKind::KW_Global: {
                if (variable_declaration->m_Scope != VariableScope::Auto)
                {
                    // TODO: Error more than one scope specifier
                    err("ERR: More than one scope specifier given\n");
                    return {};
                }
                variable_declaration->m_Scope = VariableScope::Global;
                break;
            }

            case TokenKind::KW_Local: {
                if (variable_declaration->m_Scope != VariableScope::Auto)
                {
                    // TODO: Error more than one scope specifier
                    err("ERR: More than one scope specifier given\n");
                    return {};
                }
                variable_declaration->m_Scope = VariableScope::Local;
                break;
            }

            case TokenKind::VariableIdentifier: {
                // VariableIdentifiers begin with a '$'
                // Like: $MyVariable
                // So for the name we ignore the very first character
                PHI_ASSERT(current_token.GetText().length() > 1u);
                variable_declaration->m_VariableName = current_token.GetText().substring_view(1u);

                PHI_ASSERT(!variable_declaration->m_VariableName.is_empty());
                PHI_ASSERT(!variable_declaration->m_VariableName.is_null());

                parsed_identifier = true;
                break;
            }

            default: {
                // TODO: Error unexpected token
                return {};
            }
        }
    }

    if (!parsed_identifier)
    {
        // TODO: Error variable declaration ends before the VariableIdentifier
        err("ERR: Missing variable identifier!\n");
        return {};
    }

    // Next me must parse a OP_Equals/'=', a new line, comment or finish parsing
    if (!HasMoreTokens())
    {
        return variable_declaration;
    }

    // Check for equals
    const Token& next_token = CurrentToken();

    if (next_token.GetTokenKind() == TokenKind::OP_Equals)
    {
        ConsumeCurrent();

        // Now me MUST parse an expression
        phi::scope_ptr<ASTExpression> expression = ParseExpression();
        if (!expression)
        {
            // TODO: Error failed to parse a valid expression
            err("ERR: Failed to parse a valid expression inside variable assignment!\n");
            return {};
        }

        variable_declaration->m_InitialValueExpression = phi::move(expression);
    }

    return variable_declaration;
}

phi::scope_ptr<ASTExpressionStatement> Parser::ParseExpressionStatement()
{
    auto expression = ParseExpression();
    if (!expression)
    {
        return {};
    }

    auto expression_statement =
            phi::make_not_null_scope<ASTExpressionStatement>(expression.release_not_null());

    if (!expression_statement->m_Expression->IsValidAsStatement())
    {
        err(fmt::format("Expression {:s} is not valid as an statement\n",
                        expression_statement->m_Expression->Name()));
        return {};
    }

    return phi::move(expression_statement);
}

phi::scope_ptr<ASTIfStatement> Parser::ParseIfStatement()
{
    if (!MustParse(TokenKind::KW_If))
    {
        return {};
    }

    // Next me MUST parse an expression
    auto if_condition = ParseExpression();
    if (!if_condition)
    {
        err("ERR: failed to parse expression!\n");

        return {};
    }

    // Next we MUST parse Then
    if (!MustParse(TokenKind::KW_Then))
    {
        err("ERR: Missing then!\n");

        return {};
    }

    ConsumeNewLineAndComments();

    IfCase if_case{.condition{if_condition.release_not_null()}, .body{}};

    // Next parse statements until we hit and EndIf, ElseIf or Else
    while (HasMoreTokens() && CurrentToken().GetTokenKind() != TokenKind::KW_EndIf &&
           CurrentToken().GetTokenKind() != TokenKind::KW_Else &&
           CurrentToken().GetTokenKind() != TokenKind::KW_ElseIf)
    {
        auto statement = ParseStatement();
        if (!statement)
        {
            err("ERR: Failed to parse statement inside of IF\n");
            return {};
        }

        if_case.body.emplace_back(statement.release_not_null());

        ConsumeNewLineAndComments();
    }

    auto if_statement = phi::make_not_null_scope<ASTIfStatement>(phi::move(if_case));

    // Handle all ElseIf cases which are optional
    while (HasMoreTokens() && CurrentToken().GetTokenKind() == TokenKind::KW_ElseIf)
    {
        // Consume KW_ElseIf token
        ConsumeCurrent();

        // Parse the condition
        auto else_if_condition = ParseExpression();
        if (!else_if_condition)
        {
            return {};
        }

        // Parse KW_Then
        if (!MustParse(TokenKind::KW_Then))
        {
            // TODO: Better error message and this error should be recoverable
            err("ERR: Missing then!\n");
        }

        ConsumeNewLineAndComments();

        IfCase else_if_case{.condition{else_if_condition.release_not_null()},
                            .body{ParseIfCaseStatements()}};

        // Append our case to the if statement
        if_statement->m_ElseIfCases.emplace_back(phi::move(else_if_case));
    }

    // Handle optional else case
    if (HasMoreTokens() && CurrentToken().GetTokenKind() == TokenKind::KW_Else)
    {
        // Consume KW_Else token
        ConsumeCurrent();

        ConsumeNewLineAndComments();

        if_statement->m_ElseCase = ParseIfCaseStatements();
    }

    if (!MustParse(TokenKind::KW_EndIf))
    {
        err("ERR: Missing EndIf!\n");
        // TODO: Proper Error
        return {};
    }

    return phi::move(if_statement);
}

std::vector<phi::not_null_scope_ptr<ASTStatement>> Parser::ParseIfCaseStatements()
{
    std::vector<phi::not_null_scope_ptr<ASTStatement>> statements;

    // Parse statements until KW_EndIf, KW_Else, KW_ElseIf
    while (HasMoreTokens() && CurrentToken().GetTokenKind() != TokenKind::KW_EndIf &&
           CurrentToken().GetTokenKind() != TokenKind::KW_Else &&
           CurrentToken().GetTokenKind() != TokenKind::KW_ElseIf)
    {
        auto statement = ParseStatement();
        if (!statement)
        {
            err("ERR: Failed to parse statement inside of IF\n");
            return {};
        }

        statements.emplace_back(statement.release_not_null());

        ConsumeNewLineAndComments();
    }

    return statements;
}

phi::scope_ptr<ASTIntegerLiteral> Parser::ParseIntegerLiteral()
{
    const Token& token = CurrentToken();
    if (token.GetTokenKind() != TokenKind::IntegerLiteral)
    {
        return {};
    }

    if (token.GetText().length() > 18u)
    {
        // TODO: Better check and proper error
        return {};
    }

    phi::int64_t value{0};
    phi::boolean parsing_hex{false};
    for (phi::usize index{0u}; index < token.GetText().length(); ++index)
    {
        char character = token.GetText().at(index);

        // Checking the second character
        if (index == 1u)
        {
            if (character == 'x' || character == 'X')
            {
                parsing_hex = true;
                continue;
            }
        }

        if (parsing_hex)
        {
            PHI_ASSERT(phi::is_hex_digit(character));

            value <<= 4;
            value |= phi::hex_digit_value(character).unsafe();
        }
        else
        {
            PHI_ASSERT(character >= '0' && character <= '9');

            value *= 10;
            value += (character - '0');
        }
    }

    ConsumeCurrent();
    return phi::make_scope<ASTIntegerLiteral>(value);
}

phi::scope_ptr<ASTStringLiteral> Parser::ParseStringLiteral()
{
    const Token& token = CurrentToken();
    if (token.GetTokenKind() != TokenKind::StringLiteral)
    {
        return {};
    }
    ConsumeCurrent();

    auto string_literal = phi::make_scope<ASTStringLiteral>();

    const phi::usize length = token.GetText().length();
    // Trim the trailing and leading "
    string_literal->m_Value = token.GetText().substring_view(1u, length - 2u);

    return phi::move(string_literal);
}

phi::scope_ptr<ASTExpression> Parser::ParseExpression()
{
    phi::scope_ptr<ASTExpression> lhs_expression = ParseExpressionLhs();
    if (!lhs_expression)
    {
        return {};
    }

    return ParseExpressionRhs(lhs_expression.release_not_null(), 0);
}

phi::scope_ptr<ASTExpression> Parser::ParseExpressionLhs()
{
    if (!HasMoreTokens())
    {
        // TODO: Proper error
        return {};
    }

    const Token& token = CurrentToken();
    if (IsUnaryOperator(token.GetTokenKind()))
    {
        const int op_precedence = OperatorPrecedence.lookup(token.GetTokenKind());
        PHI_UNUSED_VARIABLE(op_precedence);
        ConsumeCurrent();

        phi::scope_ptr<ASTExpression> unary_expression = ParseUnaryExpression(token.GetTokenKind());
        if (!unary_expression)
        {
            // TODO: Proper error
            return {};
        }

        return phi::move(unary_expression);
    }
    if (token.GetTokenKind() == TokenKind::LParen)
    {
        // Consume the LParen
        ConsumeCurrent();

        phi::scope_ptr<ASTExpression> paren_expression = ParseParenExpression();
        if (!paren_expression)
        {
            // TODO: Proper error
            return {};
        }

        return phi::move(paren_expression);
    }
    if (token.GetTokenKind() == TokenKind::IntegerLiteral)
    {
        phi::scope_ptr<ASTExpression> int_literal = ParseIntegerLiteral();
        if (!int_literal)
        {
            // TODO: Error failed to parse integer literal
            return {};
        }

        return phi::move(int_literal);
    }
    if (token.GetTokenKind() == TokenKind::StringLiteral)
    {
        auto string_literal_expression = ParseStringLiteral();
        if (!string_literal_expression)
        {
            // TODO: Proper error
            return {};
        }

        return phi::move(string_literal_expression);
    }

    // Boolean literal
    if (token.GetTokenKind() == TokenKind::KW_True || token.GetTokenKind() == TokenKind::KW_False)
    {
        auto boolean_literal = ParseBooleanLiteral();
        if (!boolean_literal)
        {
            // TODO: Proper error
            return {};
        }

        return phi::move(boolean_literal);
    }

    // Function call expression
    if (token.GetTokenKind() == TokenKind::FunctionIdentifier || token.IsBuiltInFunction())
    {
        auto function_call_expression = ParseFunctionCallExpression();
        if (!function_call_expression)
        {
            // TODO: Proper error
            err("ERR: Failed to parse function call expression!\n");
            return {};
        }

        return phi::move(function_call_expression);
    }
    // Variable expression
    if (token.GetTokenKind() == TokenKind::VariableIdentifier)
    {
        auto variable_expression = ParseVariableExpression();
        if (!variable_expression)
        {
            // TODO: Proper error
            err("ERR: Failed to parse Variable expression\n");
            return {};
        }

        return phi::move(variable_expression);
    }
    // Keyword literal
    if (token.IsKeywordLiteral())
    {
        auto keyword_literal = ParseKeywordLiteral();
        if (!keyword_literal)
        {
            // TODO: Proper error
            err("ERR: Failed to parse keyword literal expression!\n");
            return {};
        }

        return phi::move(keyword_literal);
    }
    // Float literal
    if (token.GetTokenKind() == TokenKind::FloatLiteral)
    {
        auto float_literal = ParseFloatLiteral();
        if (!float_literal)
        {
            // TODO: Proper error
            err("ERR: Failed to parse float literal expression!\n");
            return {};
        }

        return phi::move(float_literal);
    }
    // ArraySubscript expression
    if (token.GetTokenKind() == TokenKind::LSquare)
    {
        auto subscript_expression = ParseArraySubscriptExpression();
        if (!subscript_expression)
        {
            err("ERR: Failed to parse array subscript expression\n");
            return {};
        }

        return phi::move(subscript_expression);
    }
    if (token.IsMacro())
    {
        ConsumeCurrent();

        phi::scope_ptr<ASTExpression> macro_expression = ParseMacroExpression(token.GetTokenKind());
        if (!macro_expression)
        {
            // TODO: Proper error
            return {};
        }

        return macro_expression;
    }

    // TODO: Error Unexpected token
    err(fmt::format("Unexpected token '{:s}' while parsing expression\n",
                    enum_name(token.GetTokenKind())));
    return {};
}

phi::scope_ptr<ASTExpression> Parser::ParseExpressionRhs(phi::not_null_scope_ptr<ASTExpression> lhs,
                                                         int precedence)
{
    while (true)
    {
        if (!HasMoreTokens())
        {
            return phi::move(lhs);
        }

        const Token& operator_token = CurrentToken();
        if (!IsBinaryOperator(operator_token.GetTokenKind()) &&
            operator_token.GetTokenKind() != TokenKind::OP_TernaryIf)
        {
            // If its not a binary operator and not a ternary just return the lhs expression
            return phi::move(lhs);
        }
        int token_precedence = OperatorPrecedence.lookup(operator_token.GetTokenKind());
        ConsumeCurrent();

        if (token_precedence < precedence)
        {
            return phi::move(lhs);
        }

        if (operator_token.GetTokenKind() == TokenKind::OP_TernaryIf)
        {
            phi::scope_ptr<ASTTernaryIfExpression> ternary_if_expression =
                    ParseTernaryIfExpression(phi::move(lhs));
            if (!ternary_if_expression)
            {
                // TODO: Proper error
                return {};
            }

            return phi::move(ternary_if_expression);
        }

        // This must be an binary expression
        phi::scope_ptr<ASTExpression> rhs_expression = ParseExpressionLhs();
        if (!rhs_expression)
        {
            // TODO: Proper error
            return {};
        }

        // Nothing left to parse so directly return from here
        if (!HasMoreTokens())
        {
            return phi::make_not_null_scope<ASTBinaryExpression>(phi::move(lhs),
                                                                 operator_token.GetTokenKind(),
                                                                 rhs_expression.release_not_null());
        }

        // If BinOp binds less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        const Token& next_token      = CurrentToken();
        int          next_precedence = OperatorPrecedence.lookup(next_token.GetTokenKind());

        if (token_precedence < next_precedence)
        {
            rhs_expression =
                    ParseExpressionRhs(rhs_expression.release_not_null(), token_precedence + 1);
            if (!rhs_expression)
            {
                // TODO: Proper error
                return {};
            }
        }

        // Merge LHS/RHS.
        lhs = phi::make_not_null_scope<ASTBinaryExpression>(
                phi::move(lhs), operator_token.GetTokenKind(), rhs_expression.release_not_null());
    }
}

phi::scope_ptr<ASTFunctionCallExpression> Parser::ParseFunctionCallExpression()
{
    // Parse the function name
    const Token& function_identifier_token = CurrentToken();
    if (function_identifier_token.GetTokenKind() != TokenKind::FunctionIdentifier &&
        !function_identifier_token.IsBuiltInFunction())
    {
        return {};
    }
    ConsumeCurrent();

    phi::scope_ptr<ASTFunctionCallExpression> function_call_expression =
            phi::make_scope<ASTFunctionCallExpression>();
    if (!function_call_expression)
    {
        // TODO: Proper error
        return {};
    }

    if (function_identifier_token.IsBuiltInFunction())
    {
        function_call_expression->m_IsBuiltIn       = true;
        function_call_expression->m_BuiltInFunction = function_identifier_token.GetTokenKind();
    }
    else
    {
        const phi::string_view function_name = function_identifier_token.GetText();

        function_call_expression->m_IsBuiltIn    = false;
        function_call_expression->m_FunctionName = function_name;
    }

    // TODO: These 2 checks should be combined
    // Now me MUST parse an LParen
    if (!HasMoreTokens())
    {
        // TODO: Give proper error
        err(fmt::format("ERR: Expected opening parenthesis for function call '{:s}'\n",
                        std::string_view{function_call_expression->FunctionName()}));
        return {};
    }

    const Token& left_paren_token = CurrentToken();
    ConsumeCurrent();
    if (left_paren_token.GetTokenKind() != TokenKind::LParen)
    {
        // TODO: Give error
        err(fmt::format("ERR: Expected opening parenthesis for function call '{:s}'\n",
                        std::string_view{function_call_expression->FunctionName()}));
        return {};
    }

    // Now parse all the arguments (which are expressions) separated by commas or nothing
    function_call_expression->m_Arguments = ParseFunctionCallArguments();

    // Finally we MUST parse an RParen
    if (!HasMoreTokens())
    {
        err(fmt::format("ERR: Expected closing parenthesis for function call '{:s}'\n",
                        std::string_view{function_call_expression->FunctionName()}));
        // TODO: Give proper error
        return {};
    }

    const Token& right_paren_token = CurrentToken();
    ConsumeCurrent();
    if (right_paren_token.GetTokenKind() != TokenKind::RParen)
    {
        err(fmt::format("ERR: Expected closing parenthesis for function call '{:s}'\n",
                        std::string_view{function_call_expression->FunctionName()}));
        // TODO: Give Error
        return {};
    }

    // Return result
    return function_call_expression;
}

std::vector<phi::not_null_scope_ptr<ASTExpression>> Parser::ParseFunctionCallArguments()
{
    std::vector<phi::not_null_scope_ptr<ASTExpression>> arguments;

    while (HasMoreTokens() && CurrentToken().GetTokenKind() != TokenKind::RParen)
    {
        // Parse the expression
        phi::scope_ptr<ASTExpression> expression = ParseExpression();
        if (!expression)
        {
            err("ERR: While parsing expression for function call arguments\n");
            // TODO: Give Prober error
            arguments.clear();
            return arguments;
        }

        // Add argument to parameters
        arguments.emplace_back(expression.release_not_null());

        // Next Token MUST be a comma followed by another expression or RParen
        if (HasMoreTokens() && CurrentToken().GetTokenKind() == TokenKind::Comma)
        {
            ConsumeCurrent();
        }
    }

    return arguments;
}

phi::scope_ptr<ASTVariableExpression> Parser::ParseVariableExpression()
{
    if (!HasMoreTokens())
    {
        // TODO: Proper error
        return {};
    }

    const Token& token = CurrentToken();
    if (token.GetTokenKind() != TokenKind::VariableIdentifier)
    {
        // TODO error
        return {};
    }

    auto variable_expression            = phi::make_scope<ASTVariableExpression>();
    variable_expression->m_VariableName = token.GetText().substring_view(1u);

    ConsumeCurrent();

    return phi::move(variable_expression);
}

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wsuggest-attribute=const")
PHI_GCC_SUPPRESS_WARNING("-Wsuggest-attribute=pure")

phi::scope_ptr<ASTArraySubscriptExpression> Parser::ParseArraySubscriptExpression()
{
    if (!HasMoreTokens())
    {
        return {};
    }

    if (!MustParse(TokenKind::LSquare))
    {
        return {};
    }

    phi::scope_ptr<ASTExpression> expression = ParseExpression();
    if (!expression)
    {
        return {};
    }

    if (!MustParse(TokenKind::RSquare))
    {
        return {};
    }

    return phi::make_scope<ASTArraySubscriptExpression>(phi::move(expression.release_not_null()));
}

PHI_GCC_SUPPRESS_WARNING_POP()

phi::scope_ptr<ASTExpression> Parser::ParseParenExpression()
{
    // NOTE: Me MUST have consumed the LParen before this

    phi::scope_ptr<ASTExpression> expression = ParseExpression();
    if (!expression)
    {
        // TODO: Proper error
        return {};
    }

    if (!MustParse(TokenKind::RParen))
    {
        // TODO: Proper error expected closing paren
        return {};
    }

    return phi::move(expression);
}

phi::scope_ptr<ASTExitStatement> Parser::ParseExitStatement()
{
    if (!HasMoreTokens())
    {
        return {};
    }

    if (!MustParse(TokenKind::KW_Exit))
    {
        return {};
    }

    // Parse optional expression
    phi::scope_ptr<ASTExpression> expression = ParseExpression();

    return phi::make_scope<ASTExitStatement>(phi::move(expression));
}

phi::scope_ptr<ASTUnaryExpression> Parser::ParseUnaryExpression(const TokenKind operator_kind)
{
    PHI_ASSERT(IsUnaryOperator(operator_kind));

    if (!HasMoreTokens())
    {
        return {};
    }

    // Parse expression
    phi::scope_ptr<ASTExpression> expression = ParseExpression();
    if (!expression)
    {
        // TODO: Proper error
        return {};
    }

    return phi::make_scope<ASTUnaryExpression>(operator_kind,
                                               phi::move(expression.release_not_null()));
}

phi::scope_ptr<ASTTernaryIfExpression> Parser::ParseTernaryIfExpression(
        phi::not_null_scope_ptr<ASTExpression>&& condition)
{
    if (!HasMoreTokens())
    {
        return {};
    }

    phi::scope_ptr<ASTExpression> true_expression = ParseExpression();
    if (!true_expression)
    {
        return {};
    }

    // Must parse the else part ":"
    if (!MustParse(TokenKind::OP_TernaryElse))
    {
        err("Missing ':' for ternary if\n");
        return {};
    }

    phi::scope_ptr<ASTExpression> false_expression = ParseExpression();
    if (!false_expression)
    {
        return {};
    }

    return phi::make_scope<ASTTernaryIfExpression>(phi::move(condition),
                                                   phi::move(true_expression.release_not_null()),
                                                   phi::move(false_expression.release_not_null()));
}

phi::scope_ptr<ASTMacroExpression> Parser::ParseMacroExpression(const TokenKind macro_kind)
{
    const auto macro = static_cast<phi::size_t>(macro_kind);
    PHI_ASSERT(macro >= MacroFirst && macro <= MacroLast);

    return phi::make_scope<ASTMacroExpression>(macro_kind);
}

phi::scope_ptr<ASTBooleanLiteral> Parser::ParseBooleanLiteral()
{
    if (!HasMoreTokens())
    {
        // TODO: Proper error
        return {};
    }

    // BooleanLiteral is either KW_True or KW_False
    const Token& token = CurrentToken();
    if (token.GetTokenKind() == TokenKind::KW_True)
    {
        ConsumeCurrent();
        return phi::make_scope<ASTBooleanLiteral>(true);
    }

    if (token.GetTokenKind() == TokenKind::KW_False)
    {
        ConsumeCurrent();
        return phi::make_scope<ASTBooleanLiteral>(false);
    }

    // TODO: Proper error
    return {};
}

phi::scope_ptr<ASTKeywordLiteral> Parser::ParseKeywordLiteral()
{
    if (!HasMoreTokens())
    {
        // TODO: Proper error
        return {};
    }

    const Token& token = CurrentToken();
    if (token.IsKeywordLiteral())
    {
        ConsumeCurrent();
        return phi::make_scope<ASTKeywordLiteral>(token.GetTokenKind());
    }

    // TODO: Proper error
    return {};
}

phi::scope_ptr<ASTFloatLiteral> Parser::ParseFloatLiteral()
{
    if (!HasMoreTokens())
    {
        // TODO: Proper error
        return {};
    }

    const Token& token = CurrentToken();
    if (token.GetTokenKind() == TokenKind::FloatLiteral)
    {
        ConsumeCurrent();

        char*    ptr   = nullptr;
        phi::f64 value = std::strtod(token.GetText().begin(), &ptr);

        return phi::make_scope<ASTFloatLiteral>(value);
    }

    // TODO: Proper error
    return {};
}
} // namespace OpenAutoIt
