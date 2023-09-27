#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/FunctionReference.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/boolean.hpp>
#include <phi/core/scope_ptr.hpp>
#include <vector>

namespace OpenAutoIt
{
class ASTFunctionCallExpression final : public ASTExpression
{
public:
    ASTFunctionCallExpression(FunctionReference function_reference)
        : m_FunctionReference{phi::move(function_reference)}
    {
        m_NodeType = ASTNodeType::FunctionCallExpression;
    }

    [[nodiscard]] phi::boolean IsBuiltIn() const
    {
        return m_FunctionReference.IsBuiltIn();
    }

    [[nodiscard]] phi::string_view FunctionName() const
    {
        return m_FunctionReference.FunctionName();
    }

    [[nodiscard]] const FunctionReference& FunctionRef() const
    {
        return m_FunctionReference;
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const override
    {
        std::string ret;

        ret += indent_times(indent);
        ret += "FunctionCallExpression: \"";
        ret += FunctionName();
        ret += "\"\n";
        ret += indent_times(indent);

        if (m_Arguments.empty())
        {
            ret += "[]";
        }
        else
        {
            ret += "[\n";

            for (const auto& exp : m_Arguments)
            {
                ret += exp->DumpAST(indent + 1u) + ",\n";
            }
            ret += indent_times(indent);
            ret += "]";
        }

        return ret;
    }

    FunctionReference                                   m_FunctionReference;
    std::vector<phi::not_null_scope_ptr<ASTExpression>> m_Arguments;
};

} // namespace OpenAutoIt
