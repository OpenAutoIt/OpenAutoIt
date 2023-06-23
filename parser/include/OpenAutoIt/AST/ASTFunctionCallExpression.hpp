#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/scope_ptr.hpp>
#include <string_view>
#include <vector>

namespace OpenAutoIt
{
class ASTFunctionCallExpression final : public ASTExpression
{
public:
    ASTFunctionCallExpression()
        : m_FunctionName{}
    {
        m_NodeType = ASTNodeType::FunctionCallExpression;
    }

    [[nodiscard]] phi::string_view FunctionName() const
    {
        if (m_IsBuiltIn)
        {
            return enum_name(m_BuiltInFunction);
        }

        return m_FunctionName;
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

    phi::boolean m_IsBuiltIn{false};
    union
    {
        TokenKind        m_BuiltInFunction;
        phi::string_view m_FunctionName;
    };
    std::vector<phi::not_null_scope_ptr<ASTExpression>> m_Arguments;
};
} // namespace OpenAutoIt
