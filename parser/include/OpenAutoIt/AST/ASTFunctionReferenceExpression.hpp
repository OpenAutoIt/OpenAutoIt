#pragma once

#include "OpenAutoIt/AST/ASTExpression.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/FunctionReference.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/scope_ptr.hpp>

namespace OpenAutoIt
{

class ASTFunctionReferenceExpression final : public ASTExpression
{
public:
    explicit ASTFunctionReferenceExpression(FunctionReference function_reference)
        : m_FunctionReference{phi::move(function_reference)}
    {
        m_NodeType = ASTNodeType::FunctionReferenceExpression;
    }

    [[nodiscard]] phi::string_view FunctionName() const
    {
        return m_FunctionReference.FunctionName();
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const override
    {
        std::string ret;

        ret += indent_times(indent);
        ret += "FunctionReferenceExpression: \"";
        ret += FunctionName();
        ret += "\"\n";

        return ret;
    }

    FunctionReference m_FunctionReference;
};

} // namespace OpenAutoIt
