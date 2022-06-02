#include "OpenAutoIt/Parser.hpp"

#include "OpenAutoIt/AST/ASTBinaryExpression.hpp"
#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTIntegerLiteral.hpp"
#include "OpenAutoIt/AST/ASTVariableDeclaration.hpp"
#include "OpenAutoIt/Associativity.hpp"
#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include "OpenAutoIt/VariableScope.hpp"
#include <magic_enum.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/move.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/scope_ptr.hpp>
#include <phi/core/size_t.hpp>
#include <cstddef>

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

        [[nodiscard]] constexpr int lookup(TokenKind token) const
        {
            int p = m_TokenPrecedence[static_cast<size_t>(token)];
            if (p == 0)
            {
                // No precedence defined
                PHI_DBG_ASSERT_NOT_REACHED();
                return -1;
            }

            return p;
        }

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

    Parser::Parser(TokenStream&& stream) noexcept
        : m_TokenStream{stream}
    {}

    void Parser::SetTokenStream(TokenStream&& stream) noexcept
    {
        m_TokenStream = stream;
    }

    phi::scope_ptr<ASTDocument> Parser::ParseDocument() noexcept
    {
        m_Document = phi::make_scope<ASTDocument>();

        while (m_TokenStream.has_more())
        {
            const Token& token = CurrentToken();

            switch (token.GetTokenKind())
            {
                case TokenKind::KW_Const:
                case TokenKind::KW_Local:
                case TokenKind::KW_Global:
                case TokenKind::KW_Static:
                case TokenKind::
                        VariableIdentifier: // TODO: VariableIdentifier MUST NOT mean variable declaraction
                    // TODO: Actually it kinda does since we can't differenciate between assignment and declaration
                    {
                        auto variable_declaration = ParseVariableDeclaration();
                        if (!variable_declaration)
                        {
                            // TODO: This should give a warning/Error not a crash
                            PHI_DBG_ASSERT_NOT_REACHED();
                        }
                        AppendToDocument(phi::move(variable_declaration));
                        break;
                    }

                default:
                    PHI_ASSERT_NOT_REACHED();
            }
        }

        return phi::move(m_Document);
    }

    phi::scope_ptr<ASTDocument> Parser::ParseDocument(TokenStream&& stream) noexcept
    {
        SetTokenStream(phi::move(stream));

        return phi::move(ParseDocument());
    }

    Associativity Parser::GetOperatorAssociativity(const TokenKind token_kind) noexcept
    {
        switch (token_kind)
        {
            case TokenKind::OP_Raise:
                return Associativity::Right;

            default:
                return Associativity::Left;
        }
    }

    Associativity Parser::GetTokenAssociativity(const Token& token) noexcept
    {
        return GetOperatorAssociativity(token.GetTokenKind());
    }

    phi::boolean Parser::IsUnaryOperator(const TokenKind token_kind) noexcept
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

    phi::boolean Parser::IsBinaryOperator(const TokenKind token_kind) noexcept
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

    const Token& Parser::CurrentToken() const noexcept
    {
        PHI_ASSERT(m_TokenStream.has_more());

        return m_TokenStream.look_ahead();
    }

    void Parser::ConsumeCurrent() noexcept
    {
        auto current_token = CurrentToken();
        (void)m_TokenStream.consume();
    }

    phi::scope_ptr<ASTVariableDeclaration> Parser::ParseVariableDeclaration() noexcept
    {
        auto variable_declaration = phi::make_scope<ASTVariableDeclaration>();

        phi::boolean parsed_identifier = false;
        // Parse all specifiers until we hit a VariableIdentifier
        while (m_TokenStream.has_more() && !parsed_identifier)
        {
            const Token& current_token = CurrentToken();
            ConsumeCurrent();

            switch (current_token.GetTokenKind())
            {
                case TokenKind::KW_Const: {
                    if (variable_declaration->m_IsConst)
                    {
                        // TODO: Error more than a const specifier
                    }
                    variable_declaration->m_IsConst = true;
                    break;
                }

                case TokenKind::KW_Static: {
                    if (variable_declaration->m_IsStatic)
                    {
                        // TODO: Error more than one static specifier
                    }
                    variable_declaration->m_IsStatic = true;
                    break;
                }

                case TokenKind::KW_Global: {
                    if (variable_declaration->m_Scope != VariableScope::Auto)
                    {
                        // TODO: Error more than one scope specifier
                    }
                    variable_declaration->m_Scope = VariableScope::Global;
                    break;
                }

                case TokenKind::KW_Local: {
                    if (variable_declaration->m_Scope != VariableScope::Auto)
                    {
                        // TODO: Error more than one scope specifier
                    }
                    variable_declaration->m_Scope = VariableScope::Local;
                    break;
                }

                case TokenKind::VariableIdentifier: {
                    // VariableIdentifiers begin with a '$'
                    // Like: $MyVariable
                    // So for the name we ignore the very first character
                    variable_declaration->m_VariableName =
                            current_token.GetText().substring_view(1u);

                    parsed_identifier = true;
                    break;
                }

                default: {
                    // TODO: Error unexpected token
                    return {};
                    break;
                }
            }
        }

        if (!parsed_identifier)
        {
            // TODO: Error variable declaration ends before the VariableIdentifier
            return {};
        }

        // Next me must parse a OP_Equals/'=', a new line, comment or finish parsing
        if (!m_TokenStream.has_more())
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
                return {};
            }

            variable_declaration->m_InitialValueExpression = phi::move(expression);
        }

        return variable_declaration;
    } // namespace OpenAutoIt

    phi::scope_ptr<ASTExpression> Parser::ParseExpression(int precedence) noexcept
    {
        const Token& token = CurrentToken();
        if (IsUnaryOperator(token.GetTokenKind()))
        {
            int precedence = OperatorPrecedence.lookup(token.GetTokenKind());
            ConsumeCurrent();

            // TODO: Implement me
        }
        else if (token.GetTokenKind() == TokenKind::LParen)
        {
            // TODO: Implement me
            (void)m_TokenStream.consume();
            ParseExpression(precedence);
            //expect )
        }
        else if (token.GetTokenKind() == TokenKind::IntegerLiteral)
        {
            auto int_literal = phi::scope_ptr<ASTExpression>{ParseInterLiteral().leak_ptr()};
            if (!int_literal)
            {
                // TODO: Error failed to parse integer literal
                return {};
            }

            // TODO: For some reason reached_end() always returns false
            // Check for EOF or NewLine
            if (m_TokenStream.reached_end() || CurrentToken().GetTokenKind() == TokenKind::NewLine)
            {
                // This is just an inter literal
                return phi::scope_ptr<ASTExpression>{int_literal.leak_ptr()};
            }

            const Token& next_token = CurrentToken();

            if (IsBinaryOperator(next_token.GetTokenKind()))
            {
                // Then this must be a binary expression
                auto binary_exp = phi::scope_ptr<ASTExpression>{
                        ParseBinaryExpression(phi::move(int_literal)).leak_ptr()};
                if (!binary_exp)
                {
                    // TODO: Error failed to parse binary expression
                    return {};
                }

                return binary_exp;
            }

            // TODO: Error unexpected token
            return {};
        }

        // TODO: Fix me
        return {};
        // TODO: Error: not valid token
    }

    phi::scope_ptr<ASTIntegerLiteral> Parser::ParseInterLiteral() noexcept
    {
        const Token& token = CurrentToken();
        if (token.GetTokenKind() != TokenKind::IntegerLiteral)
        {
            return phi::scope_ptr<ASTIntegerLiteral>{nullptr};
        }

        phi::i64 value{0};
        for (char character : token.GetText())
        {
            PHI_DBG_ASSERT(character >= '0' && character <= '9');
            value *= 10;
            value += (character - '0');
        }

        ConsumeCurrent();
        return phi::make_scope<ASTIntegerLiteral>(value);
    }

    phi::scope_ptr<ASTBinaryExpression> Parser::ParseBinaryExpression(
            phi::scope_ptr<ASTExpression> lhs_expression) noexcept
    {
        auto binary_exp   = phi::make_scope<ASTBinaryExpression>();
        binary_exp->m_LHS = phi::move(lhs_expression);

        const Token& op_token = CurrentToken();

        if (!IsBinaryOperator(op_token.GetTokenKind()))
        {
            // TODO: Error non binary operator token
            return {};
        }

        ConsumeCurrent();
        binary_exp->m_Operator = op_token.GetTokenKind();

        auto rhs_exp = ParseExpression(-1); // TODO: -1 is very likely not correct and a HACK
        if (!rhs_exp)
        {
            // TODO: Error failed to parse rhs expression
            return {};
        }

        binary_exp->m_RHS = phi::move(rhs_exp);

        return binary_exp;
    }
} // namespace OpenAutoIt
