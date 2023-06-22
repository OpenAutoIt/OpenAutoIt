#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/scope_ptr.hpp>
#include <string>

namespace OpenAutoIt
{
class ASTBinaryExpression final : public ASTExpression
{
public:
    ASTBinaryExpression(phi::not_null_scope_ptr<ASTExpression>&& lhs, const TokenKind op,
                        phi::not_null_scope_ptr<ASTExpression>&& rhs) noexcept
        : m_LHS{phi::move(lhs)}
        , m_Operator{op}
        , m_RHS{phi::move(rhs)}
    {
        m_NodeType = ASTNodeType::BinaryExpression;
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
    {
        std::string ret;

        ret += indent_times(indent);
        ret += "BinaryExpression [";
        ret += enum_name(m_Operator);
        ret += "]\n";
        ret += indent_times(indent) + "[\n";
        ret += m_LHS->DumpAST(indent + 1u);
        ret += '\n';
        ret += m_RHS->DumpAST(indent + 1u);
        ret += '\n';
        ret += indent_times(indent) + ']';

        return ret;
    }

    // TODO: Make these private
public:
    phi::not_null_scope_ptr<ASTExpression> m_LHS;
    TokenKind                              m_Operator;
    phi::not_null_scope_ptr<ASTExpression> m_RHS;
};
} // namespace OpenAutoIt
