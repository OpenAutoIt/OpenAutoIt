#pragma once

#include "OpenAutoIt/AST/ASTNode.hpp"
#include <vector>

namespace OpenAutoIt
{
    class ASTDocument : public ASTNode
    {
    public:
        [[nodiscard]] virtual const char* Name() noexcept override
        {
            return "Document";
        }

        void AppendChild(ASTNode&& child) noexcept
        {
            m_Children.push_back(child);
        }

    private:
        std::vector<ASTNode> m_Children;
    };
} // namespace OpenAutoIt
