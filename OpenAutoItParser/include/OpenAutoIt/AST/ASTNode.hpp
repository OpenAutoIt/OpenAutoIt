#pragma once

#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/assert.hpp>
#include <phi/core/types.hpp>
#include <string>

namespace OpenAutoIt
{
    class ASTNode
    {
    public:
        ASTNode() = default;

        virtual ~ASTNode() = default;

        [[nodiscard]] virtual const char* Name() const noexcept = 0;

        [[nodiscard]] virtual std::string DumpAST(phi::usize indent = 0u) const noexcept = 0;
    };
} // namespace OpenAutoIt
