#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/boolean.hpp>
#include <phi/core/types.hpp>
#include <string>

namespace OpenAutoIt
{
class ASTBooleanLiteral final : public ASTExpression
{
public:
    ASTBooleanLiteral(phi::boolean value)
        : m_Value{value}
    {
        m_NodeType = ASTNodeType::BooleanLiteral;
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const override
    {
        std::string ret;

        ret += indent_times(indent);
        ret += "BooleanLiteral [";
        ret += m_Value ? "True" : "False";
        ret += "]";

        return ret;
    }

    // TODO: Make private
public:
    phi::boolean m_Value;
};
} // namespace OpenAutoIt
