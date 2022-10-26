#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/scope_ptr.hpp>
#include <string>
#include <vector>

namespace OpenAutoIt
{
    class ASTWhileStatement final : public ASTStatement
    {
    public:
        ASTWhileStatement(phi::not_null_scope_ptr<ASTExpression>&& condition) noexcept
            : m_ConditionExpression{phi::move(condition)}
        {
            m_NodeType = ASTNodeType::WhileStatement;
        }

        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            std::string ret;

            ret += indent_times(indent);
            ret += "WhileStatement [";
            ret += m_ConditionExpression->DumpAST(0u);
            ret += "]\n";
            ret += indent_times(indent);
            ret += "[\n";
            for (const auto& statement : m_Statements)
            {
                ret += statement->DumpAST(indent + 1u);
            }
            ret += indent_times(indent);
            ret += "]\n";

            return ret;
        }

    public:
        phi::not_null_scope_ptr<ASTExpression>             m_ConditionExpression;
        std::vector<phi::not_null_scope_ptr<ASTStatement>> m_Statements;
    };
} // namespace OpenAutoIt
