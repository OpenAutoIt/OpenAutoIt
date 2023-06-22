#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/scope_ptr.hpp>
#include <string>

namespace OpenAutoIt
{
class ASTExitStatement final : public ASTStatement
{
public:
    ASTExitStatement(phi::scope_ptr<ASTExpression>&& expression) noexcept
        : m_Expression{phi::move(expression)}
    {
        m_NodeType = ASTNodeType::ExitStatement;
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
    {
        std::string ret;

        // TODO:
        (void)indent;

        return ret;
    }

    // TODO: Make these private
public:
    phi::scope_ptr<ASTExpression> m_Expression;
};
} // namespace OpenAutoIt
