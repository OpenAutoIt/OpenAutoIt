#pragma once

#include "OpenAutoIt/AST/ASTArraySubscriptExpression.hpp"
#include "OpenAutoIt/AST/ASTBinaryExpression.hpp"
#include "OpenAutoIt/AST/ASTBooleanLiteral.hpp"
#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTExpressionStatement.hpp"
#include "OpenAutoIt/AST/ASTFunctionCallExpression.hpp"
#include "OpenAutoIt/AST/ASTFunctionDefinition.hpp"
#include "OpenAutoIt/AST/ASTIfStatement.hpp"
#include "OpenAutoIt/AST/ASTIntegerLiteral.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/AST/ASTStringLiteral.hpp"
#include "OpenAutoIt/AST/ASTVariableAssignment.hpp"
#include "OpenAutoIt/AST/ASTVariableExpression.hpp"
#include "OpenAutoIt/AST/ASTWhileStatement.hpp"
#include "OpenAutoIt/Associativity.hpp"
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

        explicit Parser(TokenStream&& stream) noexcept;

        void SetTokenStream(TokenStream&& stream) noexcept;

        phi::scope_ptr<ASTDocument> ParseDocument() noexcept;

        phi::scope_ptr<ASTDocument> ParseDocument(TokenStream&& stream) noexcept;

    private:
        [[nodiscard]] PHI_ATTRIBUTE_CONST static Associativity GetOperatorAssociativity(
                const TokenKind token_kind) noexcept;
        [[nodiscard]] PHI_ATTRIBUTE_CONST static Associativity GetTokenAssociativity(
                const Token& token) noexcept;

        [[nodiscard]] PHI_ATTRIBUTE_CONST static phi::boolean IsUnaryOperator(
                const TokenKind token_kind) noexcept;
        [[nodiscard]] PHI_ATTRIBUTE_CONST static phi::boolean IsBinaryOperator(
                const TokenKind token_kind) noexcept;

        [[nodiscard]] const Token& CurrentToken() const noexcept;

        void ConsumeCurrent() noexcept;

        void ConsumeNewLineAndComments() noexcept;

        [[nodiscard]] phi::boolean MustParse(TokenKind kind) noexcept;

        template <typename TypeT>
        void AppendStatementToDocument(phi::scope_ptr<TypeT> statement) noexcept
        {
            m_Document->AppendStatement(phi::scope_ptr<ASTStatement>{statement.leak_ptr()});
        }

        // TODO: Move to .cpp
        void AppendFunctionToDocument(phi::scope_ptr<ASTFunctionDefinition> function) noexcept
        {
            m_Document->AppendFunction(phi::move(function));
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
        phi::scope_ptr<ASTExpression> ParseExpression(int precedence) noexcept;

        phi::scope_ptr<ASTBinaryExpression> ParseBinaryExpression(
                phi::scope_ptr<ASTExpression> lhs_expression) noexcept;

        phi::scope_ptr<ASTFunctionCallExpression>   ParseFunctionCallExpression() noexcept;
        phi::scope_ptr<ASTVariableExpression>       ParseVariableExpression() noexcept;
        phi::scope_ptr<ASTArraySubscriptExpression> ParseArraySubscriptExpression() noexcept;

        // Literals
        phi::scope_ptr<ASTIntegerLiteral> ParseIntegerLiteral() noexcept;
        phi::scope_ptr<ASTStringLiteral>  ParseStringLiteral() noexcept;
        phi::scope_ptr<ASTBooleanLiteral> ParseBooleanLiteral() noexcept;

        TokenStream                 m_TokenStream;
        phi::scope_ptr<ASTDocument> m_Document;
    };
} // namespace OpenAutoIt
