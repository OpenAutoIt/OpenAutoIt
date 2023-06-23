#pragma once

#include "ASTExpression.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/scope_ptr.hpp>
#include <string>

namespace OpenAutoIt
{
class ASTTernaryIfExpression final : public ASTExpression
{
public:
    ASTTernaryIfExpression(phi::not_null_scope_ptr<ASTExpression>&& condition_expression,
                           phi::not_null_scope_ptr<ASTExpression>&& true_expression,
                           phi::not_null_scope_ptr<ASTExpression>&& false_expression)
        : m_ConditionExpression{phi::move(condition_expression)}
        , m_TrueExpression{phi::move(true_expression)}
        , m_FalseExpression{phi::move(false_expression)}
    {
        m_NodeType = ASTNodeType::TernaryIfExpression;
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const override
    {
        std::string ret;

        ret += indent_times(indent) + "TernaryIfExpression [\n";
        ret += indent_times(indent) + "condition [\n";
        ret += m_ConditionExpression->DumpAST(indent + 1u);
        ret += indent_times(indent) + "]\n";
        ret += indent_times(indent) + "true_case [\n";
        ret += m_TrueExpression->DumpAST(indent + 1u);
        ret += indent_times(indent) + "]\n";
        ret += indent_times(indent) + "false_case [\n";
        ret += m_FalseExpression->DumpAST(indent + 1u);
        ret += indent_times(indent) + "]\n";
        ret += indent_times(indent) + "]";

        return ret;
    }

public:
    phi::not_null_scope_ptr<ASTExpression> m_ConditionExpression;
    phi::not_null_scope_ptr<ASTExpression> m_TrueExpression;
    phi::not_null_scope_ptr<ASTExpression> m_FalseExpression;
};
} // namespace OpenAutoIt
