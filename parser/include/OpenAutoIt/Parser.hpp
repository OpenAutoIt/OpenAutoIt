#pragma once

#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/ASTForward.hpp"
#include "OpenAutoIt/Associativity.hpp"
#include "OpenAutoIt/DiagnosticBuilder.hpp"
#include "OpenAutoIt/DiagnosticEngine.hpp"
#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/SourceFile.hpp"
#include "OpenAutoIt/SourceLocation.hpp"
#include "OpenAutoIt/SourceManager.hpp"
#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/scope_ptr.hpp>
#include <filesystem>
#include <stack>
#include <unordered_set>

namespace OpenAutoIt
{

struct ParsingContext
{
    phi::observer_ptr<const SourceFile> source_file;
    TokenStream                         token_stream;
    SourceLocation                      included_from;
};

class Parser
{
public:
    Parser(phi::not_null_observer_ptr<SourceManager>    source_manager,
           phi::not_null_observer_ptr<DiagnosticEngine> diagnostic_engine,
           phi::not_null_observer_ptr<Lexer>            lexer);

    void ParseTokenStream(phi::not_null_observer_ptr<ASTDocument> document, TokenStream&& stream,
                          phi::not_null_observer_ptr<const SourceFile> source_file);
    void ParseString(phi::not_null_observer_ptr<ASTDocument> document, phi::string_view file_name,
                     phi::string_view source);
    void ParseFile(phi::not_null_observer_ptr<ASTDocument> document, const phi::string_view path);

private:
    void ParseDocument(phi::not_null_observer_ptr<ASTDocument> document);

    [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr static Associativity GetOperatorAssociativity(
            const TokenKind token_kind)
    {
        switch (token_kind)
        {
            case TokenKind::OP_Raise:
                return Associativity::Right;

            default:
                return Associativity::Left;
        }
    }

    [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr static Associativity GetTokenAssociativity(
            const Token& token)
    {
        return GetOperatorAssociativity(token.GetTokenKind());
    }

    [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr static phi::boolean IsUnaryOperator(
            const TokenKind token_kind)
    {
        switch (token_kind)
        {
            case TokenKind::OP_Plus:
            case TokenKind::OP_Minus:
            case TokenKind::KW_Not:
                return true;

            default:
                return false;
        }
    }

    [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr static phi::boolean IsBinaryOperator(
            const TokenKind token_kind)
    {
        switch (token_kind)
        {
            case TokenKind::KW_And:
            case TokenKind::KW_Or:
            case TokenKind::OP_Equals:
            case TokenKind::OP_PlusEquals:
            case TokenKind::OP_MinusEquals:
            case TokenKind::OP_MultiplyEquals:
            case TokenKind::OP_DivideEquals:
            case TokenKind::OP_Plus:
            case TokenKind::OP_Minus:
            case TokenKind::OP_Multiply:
            case TokenKind::OP_Divide:
            case TokenKind::OP_Raise:
            case TokenKind::OP_EqualsEquals:
            case TokenKind::OP_NotEqual:
            case TokenKind::OP_GreaterThan:
            case TokenKind::OP_GreaterThanEqual:
            case TokenKind::OP_LessThan:
            case TokenKind::OP_LessThanEqual:
            case TokenKind::OP_Concatenate:
            case TokenKind::OP_ConcatenateEquals:
                return true;

            default:
                return false;
        }
    }

    void PushParsingContext(phi::not_null_observer_ptr<const SourceFile> source_file,
                            TokenStream&&                                token_stream);
    void PushParsingContext(phi::not_null_observer_ptr<const SourceFile> source_file,
                            TokenStream&& token_stream, SourceLocation included_from);

    void PopParsingContext();

    [[nodiscard]] ParsingContext& CurrentParsingContext();

    [[nodiscard]] phi::not_null_observer_ptr<const SourceFile> CurrentSourceFile();

    [[nodiscard]] TokenStream&       CurrentTokenStream();
    [[nodiscard]] const TokenStream& CurrentTokenStream() const;

    [[nodiscard]] phi::boolean HasMoreTokens() const;

    [[nodiscard]] const Token& CurrentToken() const;
    [[nodiscard]] const Token& PreviousToken() const;

    [[nodiscard]] phi::boolean ShouldContinueParsing() const;

    void ConsumeCurrent();

    void ConsumeComments();

    void ConsumeNewLineAndComments();

    void RequireNewLine();

    [[nodiscard]] phi::optional<const Token&> MustParse(TokenKind kind);

    template <typename TypeT>
    void AppendStatementToDocument(phi::not_null_scope_ptr<TypeT> statement)
    {
        m_Document->AppendStatement(phi::move(statement));
    }

    // TODO: Move to .cpp
    void AppendFunctionToDocument(phi::not_null_scope_ptr<ASTFunctionDefinition> function)
    {
        m_Document->AppendFunction(phi::move(function));
    }

    void AppendSourceFileToDocument(phi::not_null_observer_ptr<const SourceFile> source_file,
                                    SourceLocation                               included_from);

    DiagnosticBuilder Diag();

    // Main nodes

    phi::scope_ptr<ASTFunctionDefinition> ParseFunctionDefinition();

    phi::optional<FunctionParameter> ParseFunctionParameterDefinition();

    void ParseIncludeDirective();

    // Statements
    phi::scope_ptr<ASTStatement> ParseStatement();

    phi::scope_ptr<ASTWhileStatement>                  ParseWhileStatement();
    phi::scope_ptr<ASTVariableAssignment>              ParseVariableAssignment();
    phi::scope_ptr<ASTExpressionStatement>             ParseExpressionStatement();
    phi::scope_ptr<ASTIfStatement>                     ParseIfStatement();
    std::vector<phi::not_null_scope_ptr<ASTStatement>> ParseIfCaseStatements();

    // Expressions
    phi::scope_ptr<ASTExpression> ParseExpression();

    phi::scope_ptr<ASTExpression> ParseExpressionLhs();
    phi::scope_ptr<ASTExpression> ParseExpressionRhs(phi::not_null_scope_ptr<ASTExpression> lhs,
                                                     int precedence);

    phi::scope_ptr<ASTExpression>                       ParseFunctionExpression();
    std::vector<phi::not_null_scope_ptr<ASTExpression>> ParseFunctionCallArguments();
    phi::scope_ptr<ASTVariableExpression>               ParseVariableExpression();
    phi::scope_ptr<ASTArraySubscriptExpression>         ParseArraySubscriptExpression();
    phi::scope_ptr<ASTExpression>                       ParseParenExpression();
    phi::scope_ptr<ASTExitStatement>                    ParseExitStatement();
    phi::scope_ptr<ASTUnaryExpression>     ParseUnaryExpression(const TokenKind operator_kind);
    phi::scope_ptr<ASTTernaryIfExpression> ParseTernaryIfExpression(
            phi::not_null_scope_ptr<ASTExpression>&& condition);
    phi::scope_ptr<ASTMacroExpression> ParseMacroExpression(const TokenKind macro_kind);

    // Literals
    phi::scope_ptr<ASTIntegerLiteral> ParseIntegerLiteral();
    phi::scope_ptr<ASTStringLiteral>  ParseStringLiteral();
    phi::scope_ptr<ASTBooleanLiteral> ParseBooleanLiteral();
    phi::scope_ptr<ASTKeywordLiteral> ParseKeywordLiteral();
    phi::scope_ptr<ASTFloatLiteral>   ParseFloatLiteral();

    phi::not_null_observer_ptr<SourceManager>    m_SourceManager;
    phi::not_null_observer_ptr<DiagnosticEngine> m_DiagnosticEngine;
    phi::not_null_observer_ptr<Lexer>            m_Lexer;
    phi::observer_ptr<ASTDocument>               m_Document;

    std::stack<ParsingContext>            m_ParsingContextStack;
    std::unordered_set<const SourceFile*> m_IncludeOnceFiles;
};

} // namespace OpenAutoIt
