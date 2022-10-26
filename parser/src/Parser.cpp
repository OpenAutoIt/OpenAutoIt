#include "OpenAutoIt/Parser.hpp"

#include "OpenAutoIt/AST/ASTBinaryExpression.hpp"
#include "OpenAutoIt/AST/ASTBooleanLiteral.hpp"
#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTExpressionStatement.hpp"
#include "OpenAutoIt/AST/ASTFloatLiteral.hpp"
#include "OpenAutoIt/AST/ASTFunctionCallExpression.hpp"
#include "OpenAutoIt/AST/ASTFunctionDefinition.hpp"
#include "OpenAutoIt/AST/ASTIfStatement.hpp"
#include "OpenAutoIt/AST/ASTIntegerLiteral.hpp"
#include "OpenAutoIt/AST/ASTKeywordLiteral.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/AST/ASTStringLiteral.hpp"
#include "OpenAutoIt/AST/ASTVariableAssignment.hpp"
#include "OpenAutoIt/AST/ASTVariableExpression.hpp"
#include "OpenAutoIt/AST/ASTWhileStatement.hpp"
#include "OpenAutoIt/Associativity.hpp"
#include "OpenAutoIt/ParseResult.hpp"
#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include "OpenAutoIt/VariableScope.hpp"
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/unused.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/move.hpp>
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

namespace OpenAutoIt
{
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
            if (precedence == 0)
            {
                // No precedence defined
                PHI_ASSERT_NOT_REACHED();
            }

            return precedence;
        }

        PHI_MSVC_SUPPRESS_WARNING_POP()

    private:
        int m_TokenPrecedence[NumberOfTokens]{};

        struct OperatorPrecedence
        {
            TokenKind token;
            int       precedence;
        };

        // https://www.autoitscript.com/autoit3/docs/intro/lang_operators.htm
        static constexpr const OperatorPrecedence m_OperatorPrecedence[] = {
                // Precedence 8
                {TokenKind::LParen, 8}, // (

                // Precedence 7
                {TokenKind::KW_Not, 7}, // Not

                // Precedence 6
                {TokenKind::OP_Raise, 6}, // ^

                // Precedence 5
                {TokenKind::OP_Multiply, 5}, // *
                {TokenKind::OP_Divide, 5},   // /

                // Precedence 4
                {TokenKind::OP_Plus, 4},  // +
                {TokenKind::OP_Minus, 4}, // -

                // Precedence 3
                {TokenKind::OP_Concatenate, 3}, // &

                // Precedence 2
                {TokenKind::OP_LessThan, 2},         // <
                {TokenKind::OP_LessThanEqual, 2},    // <=
                {TokenKind::OP_GreaterThan, 2},      // >
                {TokenKind::OP_GreaterThanEqual, 2}, // <=
                {TokenKind::OP_Equals, 2},           // =
                {TokenKind::OP_NotEqual, 2},         // <>
                {TokenKind::OP_EqualsEquals, 2},     // ==

                // Precedence 1
                {TokenKind::KW_Or, 1},  // Or
                {TokenKind::KW_And, 1}, // And
        };
    };

    constexpr OperatorPrecedenceTable OperatorPrecedence;

    Parser::Parser() noexcept = default;

    void Parser::ParseDocument(ParseResult& parse_result) noexcept
    {
        m_ParseResult = &parse_result;
        m_TokenStream = &parse_result.m_TokenStream;

        m_ParseResult->m_Document = phi::make_not_null_scope<ASTDocument>();

        while (m_TokenStream->has_more())
        {
            const Token& token = CurrentToken();

            // Parse global function definition
            if (token.GetTokenKind() == TokenKind::KW_Func)
            {
                auto function_definition = ParseFunctionDefinition();
                if (!function_definition)
                {
                    std::cout << "ERR: Failed to parse function definition!\n";
                    continue;
                }

                AppendFunctionToDocument(function_definition.release_not_null());
            }
            else if (token.GetTokenKind() == TokenKind::NewLine ||
                     token.GetTokenKind() == TokenKind::Comment)
            {
                // Simply ignore and consume newlines and comments
                ConsumeCurrent();
            }
            else if (token.GetTokenKind() == TokenKind::NotAToken)
            {
                std::cout << "ERR: Unexpected NotAToken!\n";
                ConsumeCurrent();
                continue;
            }
            else
            {
                auto statement = ParseStatement();
                if (!statement)
                {
                    // TODO: Proper error reporting
                    std::cout << "ERR: Failed to parse statement!\n";

                    if (m_TokenStream->has_more())
                    {
                        // Swallow the bad token
                        ConsumeCurrent();
                    }
                    continue;
                }

                AppendStatementToDocument(statement.release_not_null());
            }
        }
    }

    const Token& Parser::CurrentToken() const noexcept
    {
        PHI_ASSERT(m_TokenStream->has_more());

        return m_TokenStream->look_ahead();
    }

    void Parser::ConsumeCurrent() noexcept
    {
        m_TokenStream->consume();
    }

    void Parser::ConsumeNewLineAndComments() noexcept
    {
        while (m_TokenStream->has_more())
        {
            switch (CurrentToken().GetTokenKind())
            {
                case TokenKind::NewLine:
                case TokenKind::Comment:
                    ConsumeCurrent();
                    break;

                default:
                    return;
            }
        }
    }

    phi::boolean Parser::MustParse(TokenKind kind) noexcept
    {
        // Do we even have more tokens?
        if (!m_TokenStream->has_more())
        {
            return false;
        }

        // Is is the correct token kind
        if (CurrentToken().GetTokenKind() != kind)
        {
            return false;
        }

        ConsumeCurrent();
        return true;
    }

    phi::scope_ptr<ASTFunctionDefinition> Parser::ParseFunctionDefinition() noexcept
    {
        if (!MustParse(TokenKind::KW_Func))
        {
            // TODO: Proper Error
            return {};
        }

        // Next we MUST parse the function name
        if (!m_TokenStream->has_more())
        {
            return {};
        }
        const Token& function_name_token = CurrentToken();
        if (function_name_token.GetTokenKind() != TokenKind::FunctionIdentifier)
        {
            // TODO: Proper error
            return {};
        }
        ConsumeCurrent();

        auto function_definition            = phi::make_scope<ASTFunctionDefinition>();
        function_definition->m_FunctionName = function_name_token.GetText();

        // Next we MUST parse an opening parenthesis (LParen)
        if (!MustParse(TokenKind::LParen))
        {
            // TODO: Proper error
            return {};
        }

        // Next me parse the function parameter declarations until right parenthesis (RParen)
        while (m_TokenStream->has_more() && CurrentToken().GetTokenKind() != TokenKind::RParen)
        {
            phi::optional<FunctionParameter> function_paremeter_optional =
                    ParseFunctionParameterDefinition();
            if (!function_paremeter_optional)
            {
                // TODO: Proper error reporting
                return {};
            }
            FunctionParameter& function_paremeter = function_paremeter_optional.value();

            function_definition->m_Parameters.emplace_back(phi::move(function_paremeter));

            // Parse comma
            if (m_TokenStream->has_more() && CurrentToken().GetTokenKind() == TokenKind::Comma)
            {
                ConsumeCurrent();
            }
        }

        // Next we MUST parse a right parenthesis (RParen)
        if (!MustParse(TokenKind::RParen))
        {
            // TODO: Proper error
            return {};
        }

        // Next we MUST parse a new line
        if (!MustParse(TokenKind::NewLine))
        {
            // TODO: Proper error
            return {};
        }

        // Next parse Statements until EndFunc
        while (m_TokenStream->has_more() && CurrentToken().GetTokenKind() != TokenKind::KW_EndFunc)
        {
            auto statement = ParseStatement();
            if (!statement)
            {
                std::cout << "ERR: Failed while parsing statement for function \""
                          << std::string(function_definition->m_FunctionName) << "\"\n";
                // TODO: Report proper error
                return {};
            }

            function_definition->m_FunctionBody.emplace_back(phi::move(statement));
        }

        // Next we MUST parse EndFunc
        if (!MustParse(TokenKind::KW_EndFunc))
        {
            // TODO: Proper error
            return {};
        }

        return phi::move(function_definition);
    }

    phi::optional<FunctionParameter> Parser::ParseFunctionParameterDefinition() noexcept
    {
        // TODO: This entire function requres more error checks
        FunctionParameter parameter;

        while (m_TokenStream->has_more())
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
                    // Consume the '='
                    ConsumeCurrent();

                    // Default value is an expression
                    auto default_expression = ParseExpression(0);
                    if (!default_expression)
                    {
                        // TODO: Report error
                        return {};
                    }

                    parameter.default_value = phi::move(default_expression);
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

    phi::scope_ptr<ASTStatement> Parser::ParseStatement() noexcept
    {
        ConsumeNewLineAndComments();

        if (!m_TokenStream->has_more())
        {
            // TODO: Report proper error
            return {};
        }

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
                auto variable_declaration = ParseVariableAssignment();
                if (!variable_declaration)
                {
                    std::cout << "ERR: Failed to parse variable assignment!\n";
                    break;
                }

                return phi::move(variable_declaration);
            }

            // If Statement
            case TokenKind::KW_If: {
                auto if_statement = ParseIfStatement();
                if (!if_statement)
                {
                    std::cout << "ERR: Failed to parse if statement!\n";
                    break;
                }

                return phi::move(if_statement);
            }

            // While statement
            case TokenKind::KW_While: {
                auto while_statement = ParseWhileStatement();
                if (!while_statement)
                {
                    std::cout << "ERR: Failed to parse while statement!\n";
                    break;
                }

                return phi::move(while_statement);
            }

            default: {
                // Try to parse ExpressionStatement
                auto expression_statement = ParseExpressionStatement();
                if (!expression_statement)
                {
                    std::cout << "ERR: Unexpected token: " << enum_name(token.GetTokenKind())
                              << '\n';
                    return {};
                }

                return phi::move(expression_statement);
            }
        }

        // TODO: Proper error reporting
        return {};
    }

    phi::scope_ptr<ASTWhileStatement> Parser::ParseWhileStatement() noexcept
    {
        const Token& token = CurrentToken();
        if (token.GetTokenKind() != TokenKind::KW_While)
        {
            // TODO: Proper error
            return {};
        }
        ConsumeCurrent();

        // Next we MUST parse an Expression
        auto while_condition_expression = ParseExpression(0);
        if (!while_condition_expression)
        {
            // TODO: Proper error
            return {};
        }

        auto while_statement =
                phi::make_scope<ASTWhileStatement>(while_condition_expression.release_not_null());

        // Parse statements until KW_WEnd
        while (m_TokenStream->has_more() && CurrentToken().GetTokenKind() != TokenKind::KW_WEnd)
        {
            // Skip NewLines and comments
            if (CurrentToken().GetTokenKind() == TokenKind::NewLine ||
                CurrentToken().GetTokenKind() == TokenKind::Comment)
            {
                ConsumeCurrent();
                continue;
            }

            // Parse statements
            auto statement = ParseStatement();
            if (!statement)
            {
                // TODO: Propeer error
                return {};
            }

            while_statement->m_Statements.emplace_back(statement.release_not_null());
        }

        if (!m_TokenStream->has_more())
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

    phi::scope_ptr<ASTVariableAssignment> Parser::ParseVariableAssignment() noexcept
    {
        auto variable_declaration = phi::make_scope<ASTVariableAssignment>();

        phi::boolean parsed_identifier = false;
        // Parse all specifiers until we hit a VariableIdentifier
        while (m_TokenStream->has_more() && !parsed_identifier)
        {
            const Token& current_token = CurrentToken();
            ConsumeCurrent();

            switch (current_token.GetTokenKind())
            {
                case TokenKind::KW_Const: {
                    if (variable_declaration->m_IsConst)
                    {
                        // TODO: Error more than a const specifier
                        std::cout << "ERR: More than one const specifier given\n";
                        return {};
                    }
                    variable_declaration->m_IsConst = true;
                    break;
                }

                case TokenKind::KW_Static: {
                    if (variable_declaration->m_IsStatic)
                    {
                        // TODO: Error more than one static specifier
                        std::cout << "ERR: More than one static specifier given\n";
                        return {};
                    }
                    variable_declaration->m_IsStatic = true;
                    break;
                }

                case TokenKind::KW_Global: {
                    if (variable_declaration->m_Scope != VariableScope::Auto)
                    {
                        // TODO: Error more than one scope specifier
                        std::cout << "ERR: More than one scope specifier given\n";
                        return {};
                    }
                    variable_declaration->m_Scope = VariableScope::Global;
                    break;
                }

                case TokenKind::KW_Local: {
                    if (variable_declaration->m_Scope != VariableScope::Auto)
                    {
                        // TODO: Error more than one scope specifier
                        std::cout << "ERR: More than one scope specifier given\n";
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
                    variable_declaration->m_VariableName =
                            current_token.GetText().substring_view(1u);

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
            std::cout << "ERR: Missing variable identifier!\n";
            return {};
        }

        // Next me must parse a OP_Equals/'=', a new line, comment or finish parsing
        if (!m_TokenStream->has_more())
        {
            return variable_declaration;
        }

        // Check for equals
        const Token& next_token = CurrentToken();

        if (next_token.GetTokenKind() == TokenKind::OP_Equals)
        {
            ConsumeCurrent();

            // Now me MUST parse an expression
            phi::scope_ptr<ASTExpression> expression = phi::move(ParseExpression(-1));
            if (!expression)
            {
                // TODO: Error failed to parse a valid expression
                std::cout
                        << "ERR: Failed to parse a valid expression inside variable assignment!\n";
                return {};
            }

            variable_declaration->m_InitialValueExpression = phi::move(expression);
        }

        return variable_declaration;
    }

    phi::scope_ptr<ASTExpressionStatement> Parser::ParseExpressionStatement() noexcept
    {
        auto expression = ParseExpression(0);
        if (!expression)
        {
            return {};
        }

        auto expression_statement =
                phi::make_not_null_scope<ASTExpressionStatement>(expression.release_not_null());

        // TODO: Afterwards we surely should parse a NewLine or EOF

        // TODO: Make sure the expression is valid for an expression statement!

        return phi::move(expression_statement);
    }

    phi::scope_ptr<ASTIfStatement> Parser::ParseIfStatement() noexcept
    {
        if (!MustParse(TokenKind::KW_If))
        {
            return {};
        }

        // Next me MUST parse an expression
        auto if_condition = ParseExpression(0);
        if (!if_condition)
        {
            std::cout << "ERR: failed to parse expression!\n";

            return {};
        }

        // Next we MUST parse Then
        if (!MustParse(TokenKind::KW_Then))
        {
            std::cout << "ERR: Missing then!\n";

            return {};
        }

        ConsumeNewLineAndComments();

        IfCase if_case{.condition{if_condition.release_not_null()}, .body{}};

        // Next parse statements until we hit and EndIf, ElseIf or Else
        while (m_TokenStream->has_more() && CurrentToken().GetTokenKind() != TokenKind::KW_EndIf &&
               CurrentToken().GetTokenKind() != TokenKind::KW_Else &&
               CurrentToken().GetTokenKind() != TokenKind::KW_ElseIf)
        {
            auto statement = ParseStatement();
            if (!statement)
            {
                std::cout << "ERR: Failed to parse statement inside of IF\n";
                return {};
            }

            if_case.body.emplace_back(statement.release_not_null());

            ConsumeNewLineAndComments();
        }

        auto if_statement = phi::make_not_null_scope<ASTIfStatement>(phi::move(if_case));

        // TODO: Handle ElseIf, Else

        if (!MustParse(TokenKind::KW_EndIf))
        {
            std::cout << "ERR: Missing EndIf!\n";
            // TODO: Proper Error
            return {};
        }

        return phi::move(if_statement);
    }

    phi::scope_ptr<ASTExpression> Parser::ParseExpression(int precedence) noexcept
    {
        if (!m_TokenStream->has_more())
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

            // TODO: Implement me
        }
        else if (token.GetTokenKind() == TokenKind::LParen)
        {
            // TODO: Implement me
            m_TokenStream->consume();

            if (!m_TokenStream->has_more())
            {
                // TODO: Give proper Error
                return {};
            }

            ParseExpression(precedence);
            // TODO: expect )
        }
        else if (token.GetTokenKind() == TokenKind::IntegerLiteral)
        {
            phi::scope_ptr<ASTExpression> int_literal = ParseIntegerLiteral();
            if (!int_literal)
            {
                // TODO: Error failed to parse integer literal
                return {};
            }

            // TODO: For some reason reached_end() always returns false
            // Check for EOF or NewLine
            if (m_TokenStream->reached_end() || CurrentToken().GetTokenKind() == TokenKind::NewLine)
            {
                // This is just an inter literal
                return int_literal;
            }

            const Token& next_token = CurrentToken();

            // TODO: Why on earth is this hardcoded to integer literal? You can have binary operators for all sorts of things...
            if (IsBinaryOperator(next_token.GetTokenKind()))
            {
                // Then this must be a binary expression
                auto binary_exp = ParseBinaryExpression(int_literal.release_not_null());
                if (!binary_exp)
                {
                    // TODO: Error failed to parse binary expression
                    return {};
                }

                return phi::move(binary_exp);
            }

            // Integer literal not followed by operator so just IntegerLiteralExpression
            return phi::move(int_literal);
        }
        else if (token.GetTokenKind() == TokenKind::StringLiteral)
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
        else if (token.GetTokenKind() == TokenKind::KW_True ||
                 token.GetTokenKind() == TokenKind::KW_False)
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
        else if (token.GetTokenKind() == TokenKind::FunctionIdentifier || token.IsBuiltInFunction())
        {
            auto function_call_expression = ParseFunctionCallExpression();
            if (!function_call_expression)
            {
                // TODO: Proper error
                std::cout << "ERR: Failed to parse function call expression!\n";
                return {};
            }

            return phi::move(function_call_expression);
        }
        // Variable expression
        else if (token.GetTokenKind() == TokenKind::VariableIdentifier)
        {
            auto variable_expression = ParseVariableExpression();
            if (!variable_expression)
            {
                // TODO: Proper error
                std::cout << "ERR: Failed to parse Variable expression\n";
                return {};
            }

            // TODO: There could be an operator after here which whould need to be parsed
            //       Like $var = $var + 1 for example

            return phi::move(variable_expression);
        }
        // Keyword literal
        else if (token.IsKeywordLiteral())
        {
            auto keyword_literal = ParseKeywordliteral();
            if (!keyword_literal)
            {
                // TODO: Proper error
                std::cout << "ERR: Failed to parse keyword literal expression!\n";
                return {};
            }

            return phi::move(keyword_literal);
        }
        // Float literal
        else if (token.GetTokenKind() == TokenKind::FloatLiteral)
        {
            auto float_literal = ParseFloatLiteral();
            if (!float_literal)
            {
                // TODO: Proper error
                std::cout << "ERR: Failed to parse float literal expression!\n";
                return {};
            }

            return phi::move(float_literal);
        }

        // TODO: Error Unexpected token
        std::cout << "Unexpected token \"" << enum_name(token.GetTokenKind())
                  << "\" while parsing expression\n";
        return {};
    }

    phi::scope_ptr<ASTIntegerLiteral> Parser::ParseIntegerLiteral() noexcept
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

    phi::scope_ptr<ASTStringLiteral> Parser::ParseStringLiteral() noexcept
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

    phi::scope_ptr<ASTBinaryExpression> Parser::ParseBinaryExpression(
            phi::not_null_scope_ptr<ASTExpression> lhs_expression) noexcept
    {
        const Token& op_token = CurrentToken();

        if (!IsBinaryOperator(op_token.GetTokenKind()))
        {
            // TODO: Error non binary operator token
            return {};
        }
        ConsumeCurrent();

        phi::scope_ptr<ASTExpression> rhs_expression =
                ParseExpression(-1); // TODO: -1 is very likely not correct and a HACK
        if (!rhs_expression)
        {
            // TODO: Error failed to parse rhs expression
            return {};
        }

        return phi::make_not_null_scope<ASTBinaryExpression>(phi::move(lhs_expression),
                                                             op_token.GetTokenKind(),
                                                             rhs_expression.release_not_null());
    }

    phi::scope_ptr<ASTFunctionCallExpression> Parser::ParseFunctionCallExpression() noexcept
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
        if (!m_TokenStream->has_more())
        {
            std::cout << "ERR: Expected opening parenthesis for function call \""
                      << std::string{function_call_expression->FunctionName()} << "\"\n";
            // TODO: Give proper error
            return {};
        }

        const Token& left_paren_token = CurrentToken();
        ConsumeCurrent();
        if (left_paren_token.GetTokenKind() != TokenKind::LParen)
        {
            std::cout << "ERR: Expected opening parenthesis for function call \""
                      << std::string{function_call_expression->FunctionName()} << "\"\n";
            // TODO: Give error
            return {};
        }

        // Now parse all the arguments (which are expressions) separated by commas or nothing
        function_call_expression->m_Arguments = ParseFunctionCallArguments();

        // Finally we MUST parse an RParen
        if (!m_TokenStream->has_more())
        {
            std::cout << "ERR: Expected closing parenthesis for function call \""
                      << std::string{function_call_expression->FunctionName()} << "\"\n";
            // TODO: Give proper error
            return {};
        }

        const Token& right_paren_token = CurrentToken();
        ConsumeCurrent();
        if (right_paren_token.GetTokenKind() != TokenKind::RParen)
        {
            std::cout << "ERR: Expected closing parenthesis for function call \""
                      << std::string{function_call_expression->FunctionName()} << "\"\n";
            // TODO: Give Error
            return {};
        }

        // Return result
        return function_call_expression;
    }

    std::vector<phi::not_null_scope_ptr<ASTExpression>> Parser::
            ParseFunctionCallArguments() noexcept
    {
        std::vector<phi::not_null_scope_ptr<ASTExpression>> arguments;

        while (m_TokenStream->has_more() && CurrentToken().GetTokenKind() != TokenKind::RParen)
        {
            // Parse the expression
            phi::scope_ptr<ASTExpression> expression = ParseExpression(0);
            if (!expression)
            {
                std::cout << "ERR: While parsing expression for function call arguments\n";
                // TODO: Give Prober error
                arguments.clear();
                return arguments;
            }

            // Add argument to parameters
            arguments.emplace_back(expression.release_not_null());

            // Next Token MUST be a comma followed by another expression or RParen
            if (m_TokenStream->has_more() && CurrentToken().GetTokenKind() == TokenKind::Comma)
            {
                ConsumeCurrent();
            }
        }

        return arguments;
    }

    phi::scope_ptr<ASTVariableExpression> Parser::ParseVariableExpression() noexcept
    {
        if (!m_TokenStream->has_more())
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

    phi::scope_ptr<ASTArraySubscriptExpression> ParseArraySubscriptExpression() noexcept
    {
        // TODO: Implement me

        return phi::scope_ptr<ASTArraySubscriptExpression>{nullptr};
    }

    PHI_GCC_SUPPRESS_WARNING_POP()

    phi::scope_ptr<ASTBooleanLiteral> Parser::ParseBooleanLiteral() noexcept
    {
        if (!m_TokenStream->has_more())
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

    phi::scope_ptr<ASTKeywordLiteral> Parser::ParseKeywordliteral() noexcept
    {
        if (!m_TokenStream->has_more())
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

    phi::scope_ptr<ASTFloatLiteral> Parser::ParseFloatLiteral() noexcept
    {
        if (!m_TokenStream->has_more())
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
