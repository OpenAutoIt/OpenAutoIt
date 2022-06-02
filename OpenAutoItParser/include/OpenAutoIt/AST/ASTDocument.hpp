#pragma once

#include "OpenAutoIt/AST/ASTNode.hpp"
#include <phi/core/assert.hpp>
#include <phi/core/scope_ptr.hpp>
#include <vector>

namespace OpenAutoIt
{
    // TODO: Add file name, path maybe?
    class ASTDocument : public ASTNode
    {
    public:
        [[nodiscard]] const char* Name() const noexcept override
        {
            return "Document";
        }

        void AppendChild(phi::scope_ptr<ASTNode> child) noexcept
        {
            m_Children.emplace_back(phi::move(child));
        }

        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            std::string ret{"Document:\n"};

            // Add all children
            for (const auto& child : m_Children)
            {
                ret += child->DumpAST(indent);
            }

            return ret;
        }

        // TODO: Make private
    public:
        std::vector<phi::scope_ptr<ASTNode>> m_Children;
    };
} // namespace OpenAutoIt
