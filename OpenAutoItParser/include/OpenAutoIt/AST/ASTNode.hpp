#pragma once

namespace OpenAutoIt
{
    class ASTNode
    {
    public:
        ASTNode() = default;

        virtual ~ASTNode() = default;

        [[nodiscard]] virtual const char* Name() noexcept
        {
            return "ASTNode";
        }

    private:
    };
} // namespace OpenAutoIt
