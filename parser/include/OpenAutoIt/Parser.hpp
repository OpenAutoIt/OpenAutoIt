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
        Parser() noexcept;

        void ParseDocument(ParseResult& parse_result) noexcept;

    private:
        [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr static Associativity GetOperatorAssociativity(
                const TokenKind token_kind) noexcept
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
                const Token& token) noexcept
        {
            return GetOperatorAssociativity(token.GetTokenKind());
        }

        [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr static phi::boolean IsUnaryOperator(
                const TokenKind token_kind) noexcept
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
                const TokenKind token_kind) noexcept
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
                    return true;

                default:
                    return false;
            }
        }

        [[nodiscard]] const Token& CurrentToken() const noexcept;

        void ConsumeCurrent() noexcept;

        void ConsumeComments() noexcept;

        void ConsumeNewLineAndComments() noexcept;

        [[nodiscard]] phi::optional<const Token&> MustParse(TokenKind kind) noexcept;

        template <typename TypeT>
        void AppendStatementToDocument(phi::not_null_scope_ptr<TypeT> statement) noexcept
        {
            m_ParseResult->m_Document->AppendStatement(phi::move(statement));
        }

        // TODO: Move to .cpp
        void AppendFunctionToDocument(
                phi::not_null_scope_ptr<ASTFunctionDefinition> function) noexcept
        {
            m_ParseResult->m_Document->AppendFunction(phi::move(function));
        }

        // Main nodes

        phi::scope_ptr<ASTFunctionDefinition> ParseFunctionDefinition() noexcept;

        phi::optional<FunctionParameter> ParseFunctionParameterDefinition() noexcept;

        // Statements
        phi::scope_ptr<ASTStatement> ParseStatement() noexcept;

        phi::scope_ptr<ASTWhileStatement>      ParseWhileStatement() noexcept;
        phi::scope_ptr<ASTVariableAssignment>  ParseVariableAssignment() noexcept;
        phi::scope_ptr<ASTExpressionStatement> ParseExpressionStatement() noexcept;
        phi::scope_ptr<ASTIfStatement>         ParseIfStatement() noexcept;

        // Expressions
        phi::scope_ptr<ASTExpression> ParseExpression() noexcept;

        phi::scope_ptr<ASTExpression> ParseExpressionLhs() noexcept;
        phi::scope_ptr<ASTExpression> ParseExpressionRhs(phi::not_null_scope_ptr<ASTExpression> lhs,
                                                         int precedence) noexcept;

        phi::scope_ptr<ASTFunctionCallExpression>           ParseFunctionCallExpression() noexcept;
        std::vector<phi::not_null_scope_ptr<ASTExpression>> ParseFunctionCallArguments() noexcept;
        phi::scope_ptr<ASTVariableExpression>               ParseVariableExpression() noexcept;
        phi::scope_ptr<ASTArraySubscriptExpression> ParseArraySubscriptExpression() noexcept;
        phi::scope_ptr<ASTExpression>               ParseParenExpression() noexcept;
        phi::scope_ptr<ASTExitStatement>            ParseExitStatement() noexcept;

        // Literals
        phi::scope_ptr<ASTIntegerLiteral> ParseIntegerLiteral() noexcept;
        phi::scope_ptr<ASTStringLiteral>  ParseStringLiteral() noexcept;
        phi::scope_ptr<ASTBooleanLiteral> ParseBooleanLiteral() noexcept;
        phi::scope_ptr<ASTKeywordLiteral> ParseKeywordLiteral() noexcept;
        phi::scope_ptr<ASTFloatLiteral>   ParseFloatLiteral() noexcept;

        ParseResult* m_ParseResult;
        TokenStream* m_TokenStream;
    };
} // namespace OpenAutoIt
