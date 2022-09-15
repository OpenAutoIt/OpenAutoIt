#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/core/scope_ptr.hpp>
#include <string>

namespace OpenAutoIt
{
    class ASTBinaryExpression final : public ASTExpression
    {
    public:
        [[nodiscard]] const char* Name() const noexcept override
        {
            return "ASTBinaryExpression";
        }

        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            std::string ret;

            ret += indent_times(indent);
            ret += "BinaryExpression [";
            ret += enum_name(m_Operator);
            ret += "]\n";
            ret += indent_times(indent) + "[\n";
            ret += m_LHS->DumpAST(indent + 1u);
            ret += '\n';
            ret += m_RHS->DumpAST(indent + 1u);
            ret += '\n';
            ret += indent_times(indent) + ']';

            return ret;
        }

        // TODO: Make these private
    public:
        phi::scope_ptr<ASTExpression> m_LHS;
        TokenKind                     m_Operator;
        phi::scope_ptr<ASTExpression> m_RHS;
    };
} // namespace OpenAutoIt
