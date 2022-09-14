#pragma once

#include "ASTExpression.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/container/string_view.hpp>
#include <string>

namespace OpenAutoIt
{
    class ASTStringLiteral final : public ASTExpression
    {
    public:
        [[nodiscard]] virtual const char* Name() const noexcept override
        {
            return "ASTStringLiteral";
        }

        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            std::string ret;

            ret += indent_times(indent);
            ret += "StringLiteral [";
            ret += std::string_view{m_Value.data(), m_Value.length().unsafe()};
            ret += "]";

            return ret;
        }

    public:
        phi::string_view m_Value;
    };
} // namespace OpenAutoIt
