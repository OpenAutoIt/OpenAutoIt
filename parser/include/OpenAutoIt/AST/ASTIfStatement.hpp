#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/scope_ptr.hpp>
#include <phi/core/types.hpp>
#include <string>
#include <vector>

namespace OpenAutoIt
{
struct IfCase
{
    phi::not_null_scope_ptr<ASTExpression>             condition;
    std::vector<phi::not_null_scope_ptr<ASTStatement>> body;
};

class ASTIfStatement final : public ASTStatement
{
public:
    ASTIfStatement(IfCase&& if_case) noexcept
        : m_IfCase{phi::move(if_case)}
    {
        m_NodeType = ASTNodeType::IfStatement;
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
    {
        std::string ret;

        ret += indent_times(indent);
        ret += "IfStatement:\n";
        ret += indent_times(indent);
        ret += "condition [\n";
        ret += m_IfCase.condition->DumpAST(indent + 1u);
        ret += "\n";
        ret += indent_times(indent);
        ret += "]\n";
        ret += indent_times(indent);
        ret += "Body:\n";
        ret += indent_times(indent);

        if (m_IfCase.body.empty())
        {
            ret += "[]\n";
        }
        else
        {
            ret += "[\n";
            for (const auto& statement : m_IfCase.body)
            {
                ret += statement->DumpAST(indent + 1u);
            }
            ret += "\n";
            ret += indent_times(indent);
            ret += "]\n";
        }

        // TODO: Implement ElseIf, ElseCase

        return ret;
    }

    IfCase                                             m_IfCase;      // Must be present
    std::vector<IfCase>                                m_ElseIfCases; // Optional
    std::vector<phi::not_null_scope_ptr<ASTStatement>> m_ElseCase;    // Optional
};
} // namespace OpenAutoIt
