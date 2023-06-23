#pragma once

#include "OpenAutoIt/AST/ASTArraySubscriptExpression.hpp"
#include "OpenAutoIt/AST/ASTBinaryExpression.hpp"
#include "OpenAutoIt/AST/ASTBooleanLiteral.hpp"
#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/AST/ASTExitStatement.hpp"
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
#include "OpenAutoIt/AST/ASTTernaryIfExpression.hpp"
#include "OpenAutoIt/AST/ASTUnaryExpression.hpp"
#include "OpenAutoIt/AST/ASTVariableAssignment.hpp"
#include "OpenAutoIt/AST/ASTVariableExpression.hpp"
#include "OpenAutoIt/AST/ASTWhileStatement.hpp"
#include "OpenAutoIt/Associativity.hpp"
#include "OpenAutoIt/ParseResult.hpp"
#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/scope_ptr.hpp>

namespace OpenAutoIt
{
class Parser
{
public:
    Parser();

    void ParseDocument(ParseResult& parse_result);

private:
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

    [[nodiscard]] const Token& CurrentToken() const;

    void ConsumeCurrent();

    void ConsumeComments();

    void ConsumeNewLineAndComments();

    [[nodiscard]] phi::optional<const Token&> MustParse(TokenKind kind);

    template <typename TypeT>
    void AppendStatementToDocument(phi::not_null_scope_ptr<TypeT> statement)
    {
        m_ParseResult->m_Document->AppendStatement(phi::move(statement));
    }

    // TODO: Move to .cpp
    void AppendFunctionToDocument(phi::not_null_scope_ptr<ASTFunctionDefinition> function)
    {
        m_ParseResult->m_Document->AppendFunction(phi::move(function));
    }

    // Main nodes

    phi::scope_ptr<ASTFunctionDefinition> ParseFunctionDefinition();

    phi::optional<FunctionParameter> ParseFunctionParameterDefinition();

    // Statements
    phi::scope_ptr<ASTStatement> ParseStatement();

    phi::scope_ptr<ASTWhileStatement>      ParseWhileStatement();
    phi::scope_ptr<ASTVariableAssignment>  ParseVariableAssignment();
    phi::scope_ptr<ASTExpressionStatement> ParseExpressionStatement();
    phi::scope_ptr<ASTIfStatement>         ParseIfStatement();

    // Expressions
    phi::scope_ptr<ASTExpression> ParseExpression();

    phi::scope_ptr<ASTExpression> ParseExpressionLhs();
    phi::scope_ptr<ASTExpression> ParseExpressionRhs(phi::not_null_scope_ptr<ASTExpression> lhs,
                                                     int precedence);

    phi::scope_ptr<ASTFunctionCallExpression>           ParseFunctionCallExpression();
    std::vector<phi::not_null_scope_ptr<ASTExpression>> ParseFunctionCallArguments();
    phi::scope_ptr<ASTVariableExpression>               ParseVariableExpression();
    phi::scope_ptr<ASTArraySubscriptExpression>         ParseArraySubscriptExpression();
    phi::scope_ptr<ASTExpression>                       ParseParenExpression();
    phi::scope_ptr<ASTExitStatement>                    ParseExitStatement();
    phi::scope_ptr<ASTUnaryExpression>     ParseUnaryExpression(const TokenKind operator_kind);
    phi::scope_ptr<ASTTernaryIfExpression> ParseTernaryIfExpression(
            phi::not_null_scope_ptr<ASTExpression>&& condition);

    // Literals
    phi::scope_ptr<ASTIntegerLiteral> ParseIntegerLiteral();
    phi::scope_ptr<ASTStringLiteral>  ParseStringLiteral();
    phi::scope_ptr<ASTBooleanLiteral> ParseBooleanLiteral();
    phi::scope_ptr<ASTKeywordLiteral> ParseKeywordLiteral();
    phi::scope_ptr<ASTFloatLiteral>   ParseFloatLiteral();

    ParseResult* m_ParseResult;
    TokenStream* m_TokenStream;
};
} // namespace OpenAutoIt
