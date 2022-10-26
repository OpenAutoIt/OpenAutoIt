#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/move.hpp>
#include <phi/core/scope_ptr.hpp>

namespace OpenAutoIt
{
    class ASTExpressionStatement final : public ASTStatement
    {
    public:
        ASTExpressionStatement(phi::not_null_scope_ptr<ASTExpression>&& expression) noexcept
            : m_Expression{phi::move(expression)}
        {
            m_NodeType = ASTNodeType::ExpressionStatement;
        }

        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            std::string ret;

            ret += indent_times(indent);
            ret += "ExpressionStatement\n";
            ret += indent_times(indent);
            ret += "[\n";
            ret += m_Expression->DumpAST(indent + 1u);
            ret += "\n";
            ret += indent_times(indent);
            ret += "]\n";

            return ret;
        }

        phi::not_null_scope_ptr<ASTExpression> m_Expression;
    };
} // namespace OpenAutoIt
