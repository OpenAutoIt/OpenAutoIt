#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"

namespace OpenAutoIt
{
    class ASTCallExpression : public ASTExpression
    {
        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            return "CallExpression";
        }
    };
} // namespace OpenAutoIt
