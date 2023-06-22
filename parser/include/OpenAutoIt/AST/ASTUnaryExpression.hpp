#pragma once

#include "ASTExpression.hpp"

namespace OpenAutoIt
{
class ASTUnaryExpression : public ASTExpression
{
    ASTUnaryExpression()
    {
        m_NodeType = ASTNodeType::UnaryExpression;
    }
};
} // namespace OpenAutoIt
