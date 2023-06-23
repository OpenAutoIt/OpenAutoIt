#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/types.hpp>
#include <string>

namespace OpenAutoIt
{
class ASTIntegerLiteral final : public ASTExpression
{
public:
    ASTIntegerLiteral(phi::i64 value)
        : m_Value{value}
    {
        m_NodeType = ASTNodeType::IntegerLiteral;
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const override
    {
        return indent_times(indent) + "IntegerLiteral [" + std::to_string(m_Value.unsafe()) + "]";
    }

    // TODO: Make private
public:
    // TODO: Support signed AND unsigned
    phi::i64 m_Value;
};
} // namespace OpenAutoIt
