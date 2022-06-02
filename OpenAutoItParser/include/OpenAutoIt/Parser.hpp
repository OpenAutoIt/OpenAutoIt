#pragma once

#include "OpenAutoIt/AST/ASTBinaryExpression.hpp"
#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTIntegerLiteral.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/AST/ASTVariableDeclaration.hpp"
#include "OpenAutoIt/Associativity.hpp"
#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/TokenStream.hpp"
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
        [[nodiscard]] static Associativity GetOperatorAssociativity(
                const TokenKind token_kind) noexcept;
        [[nodiscard]] static Associativity GetTokenAssociativity(const Token& token) noexcept;

        [[nodiscard]] static phi::boolean IsUnaryOperator(const TokenKind token_kind) noexcept;
        [[nodiscard]] static phi::boolean IsBinaryOperator(const TokenKind token_kind) noexcept;

        [[nodiscard]] const Token& CurrentToken() const noexcept;

        void ConsumeCurrent() noexcept;

        template <typename TypeT>
        void AppendToDocument(phi::scope_ptr<TypeT> child) noexcept
        {
            m_Document->AppendChild(phi::scope_ptr<ASTNode>{child.leak_ptr()});
        }

        // Main nodes
        phi::scope_ptr<ASTVariableDeclaration> ParseVariableDeclaration() noexcept;

        phi::scope_ptr<ASTExpression> ParseExpression(int precedence) noexcept;

        phi::scope_ptr<ASTIntegerLiteral>   ParseInterLiteral() noexcept;
        phi::scope_ptr<ASTBinaryExpression> ParseBinaryExpression(
                phi::scope_ptr<ASTExpression> lhs_expression) noexcept;

        TokenStream                 m_TokenStream;
        phi::scope_ptr<ASTDocument> m_Document;
    };
} // namespace OpenAutoIt
