#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/scope_ptr.hpp>
#include <string>

namespace OpenAutoIt
{
    class ASTArraySubscriptExpression final : public ASTExpression
    {
    public:
        [[nodiscard]] const char* Name() const noexcept override
        {
            return "ASTArraySubscriptExpression";
        }

        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            (void)indent;

            std::string ret;

            // TODO: Implement me

            return ret;
        }

        // TODO: Make these private
    public:
        phi::scope_ptr<ASTExpression> m_BaseExpression;
        phi::scope_ptr<ASTExpression> m_IndexExpression;
    };
} // namespace OpenAutoIt
