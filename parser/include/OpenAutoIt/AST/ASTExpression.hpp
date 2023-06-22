#pragma once

#include "OpenAutoIt/AST/ASTNode.hpp"
#include <phi/core/boolean.hpp>

namespace OpenAutoIt
{
class ASTExpression : public ASTNode
{
public:
    [[nodiscard]] constexpr phi::boolean IsValidAsStatement() noexcept
    {
        switch (m_NodeType)
        {
            case ASTNodeType::FunctionCallExpression:
            case ASTNodeType::UnaryExpression:
            case ASTNodeType::VariableExpression:
                return true;

            default:
                return false;
        }
    }
};
} // namespace OpenAutoIt
