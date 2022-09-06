#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/Utililty.hpp"

namespace OpenAutoIt
{
    class ASTExpressionStatement : public ASTStatement
    {
    public:
        [[nodiscard]] const char* Name() const noexcept override
        {
            return "ASTExpressionStatement";
        }

        [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
        {
            std::string ret;

            ret += indent_times(indent);
            ret += "ExpressionStatement\n";
            ret += indent_times(indent);
            ret += "[\n";
            ret += m_Expression->DumpAST(indent + 1u);
            ret += "\n";
            ret += indent_times(indent);
            ret += "]\n";

            return ret;
        }

        phi::scope_ptr<ASTExpression> m_Expression;
    };
} // namespace OpenAutoIt
