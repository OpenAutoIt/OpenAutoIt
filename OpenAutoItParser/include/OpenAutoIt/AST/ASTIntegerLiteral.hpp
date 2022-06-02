#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/types.hpp>
#include <string>

namespace OpenAutoIt
{
    class ASTIntegerLiteral : public ASTExpression
    {
    public:
        ASTIntegerLiteral(phi::i64 value) noexcept
            : m_Value{value}
        {}

        [[nodiscard]] virtual const char* Name() const noexcept override
        {
            return "ASTIntegerLiteral";
        }

        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            return indent_times(indent) + "IntegerLiteral [" + std::to_string(m_Value.unsafe()) +
                   "]";
        }

        // TODO: Make private
    public:
        // TODO: Support signed AND unsigned
        phi::i64 m_Value;
    };
} // namespace OpenAutoIt
