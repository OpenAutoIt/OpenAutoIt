#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include "OpenAutoIt/VariableScope.hpp"
#include <magic_enum.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/scope_ptr.hpp>

namespace OpenAutoIt
{
    // TODO: Is an variableAssigment also an Expression like in C/C++?
    class ASTVariableAssignment : public ASTStatement
    {
    public:
        [[nodiscard]] const char* Name() const noexcept override
        {
            return "ASTVariableAssigment";
        }

        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            std::string ret;

            ret += indent_times(indent);
            ret += "VariableAssignment: [";
            ret += (m_IsStatic ? "static, " : "");
            ret += (m_IsConst ? "const, " : "");
            ret += magic_enum::enum_name(m_Scope);
            ret += "] $";
            ret += std::string_view(m_VariableName.data(), m_VariableName.length().unsafe());
            ret += " =";

            if (m_InitialValueExpression)
            {
                ret += "\n[\n";
                ret += m_InitialValueExpression->DumpAST(indent + 1u);
                ret += "\n]\n";
            }
            else
            {
                ret += " Uninitialized\n";
            }

            return ret;
        }

        /// TODO: These should not be public
    public:
        phi::boolean                  m_IsStatic{false};
        phi::boolean                  m_IsConst{false};
        VariableScope                 m_Scope{VariableScope::Auto};
        phi::string_view              m_VariableName{};
        phi::scope_ptr<ASTExpression> m_InitialValueExpression;
    };
} // namespace OpenAutoIt
