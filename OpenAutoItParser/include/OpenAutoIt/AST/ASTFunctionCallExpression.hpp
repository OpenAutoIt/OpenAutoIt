#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/scope_ptr.hpp>
#include <string_view>
#include <vector>

namespace OpenAutoIt
{
    class ASTFunctionCallExpression : public ASTExpression
    {
    public:
        [[nodiscard]] const char* Name() const noexcept override
        {
            return "ASTFunctionCallExpression";
        }

        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            std::string ret;

            ret += indent_times(indent);
            ret += "FunctionCallExpression: \"";
            ret += m_FunctionName;
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

        std::string_view                           m_FunctionName;
        std::vector<phi::scope_ptr<ASTExpression>> m_Arguments;
    };
} // namespace OpenAutoIt
