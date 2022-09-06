#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/boolean.hpp>
#include <phi/core/types.hpp>
#include <string>

namespace OpenAutoIt
{
    class ASTBooleanLiteral : public ASTExpression
    {
    public:
        ASTBooleanLiteral(phi::boolean value) noexcept
            : m_Value{value}
        {}

        [[nodiscard]] virtual const char* Name() const noexcept override
        {
            return "ASTBooleanLiteral";
        }

        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            std::string ret;

            ret += indent_times(indent);
            ret += "BooleanLiteral [";
            ret += m_Value ? "True" : "False";
            ret += "]";

            return ret;
        }

        // TODO: Make private
    public:
        phi::boolean m_Value;
    };
} // namespace OpenAutoIt
