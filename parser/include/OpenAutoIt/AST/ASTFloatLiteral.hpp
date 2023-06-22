#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/types.hpp>
#include <string>

namespace OpenAutoIt
{
class ASTFloatLiteral final : public ASTExpression
{
public:
    ASTFloatLiteral(phi::f64 value) noexcept
        : m_Value{value}
    {
        m_NodeType = ASTNodeType::FloatLiteral;
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
    {
        return indent_times(indent) + "FloatLiteral [" + std::to_string(m_Value.unsafe()) + "]";
    }

    // TODO: Make private
public:
    phi::f64 m_Value;
};
} // namespace OpenAutoIt
