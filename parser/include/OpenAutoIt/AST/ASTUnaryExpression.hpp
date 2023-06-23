#pragma once

#include "ASTExpression.hpp"
#include <phi/core/scope_ptr.hpp>

namespace OpenAutoIt
{

class ASTUnaryExpression final : public ASTExpression
{
public:
    ASTUnaryExpression(TokenKind operator_kind, phi::not_null_scope_ptr<ASTExpression>&& expression)
        : m_Operator{operator_kind}
        , m_Expression{phi::move(expression)}
    {
        m_NodeType = ASTNodeType::UnaryExpression;
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const override
    {
        std::string ret;

        ret += indent_times(indent);
        ret += "UnaryExpression [";
        ret += enum_name(m_Operator);
        ret += "]\n";
        ret += indent_times(indent) + "[\n";
        ret += m_Expression->DumpAST(indent + 1u);
        ret += indent_times(indent) + ']';

        return ret;
    }

    // TODO: Make private
public:
    TokenKind                              m_Operator;
    phi::not_null_scope_ptr<ASTExpression> m_Expression;
};

} // namespace OpenAutoIt
