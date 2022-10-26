#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/scope_ptr.hpp>
#include <phi/core/types.hpp>
#include <vector>

namespace OpenAutoIt
{
    struct FunctionParameter
    {
        phi::string_view              name; // Parameter name without the $
        phi::scope_ptr<ASTExpression> default_value;
        phi::boolean                  by_ref   = false;
        phi::boolean                  as_const = false;
    };

    class ASTFunctionDefinition final : public ASTNode
    {
    public:
        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            std::string ret;

            ret += indent_times(indent);
            ret += "FunctionDefinition: ";
            ret += std::string_view(m_FunctionName.data(), m_FunctionName.length().unsafe());
            ret += " (";
            for (const auto& parameter : m_Parameters)
            {
                if (parameter.as_const)
                {
                    ret += "Const ";
                }
                if (parameter.by_ref)
                {
                    ret += "ByRef ";
                }
                ret += '$';
                ret += std::string_view{parameter.name.data(), parameter.name.length().unsafe()};

                if (parameter.default_value)
                {
                    ret += " = ";
                    ret += parameter.default_value->DumpAST(0u);
                }

                ret += ", ";
            }

            // Remove last ", "
            if (!m_Parameters.empty())
            {
                ret.pop_back();
                ret.pop_back();
            }

            ret += ")\n";
            ret += indent_times(indent);
            ret += "[\n";
            for (const auto& statement : m_FunctionBody)
            {
                ret += statement->DumpAST(indent + 1u);
                ret += '\n';
            }

            ret += indent_times(indent);
            ret += "]\n";

            return ret;
        }

        phi::string_view                          m_FunctionName;
        std::vector<FunctionParameter>            m_Parameters;
        std::vector<phi::scope_ptr<ASTStatement>> m_FunctionBody;
    };
} // namespace OpenAutoIt
