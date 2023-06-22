#pragma once

#include "OpenAutoIt/AST/ASTFunctionDefinition.hpp"
#include "OpenAutoIt/AST/ASTNode.hpp"
#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/algorithm/string_equals.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/scope_ptr.hpp>
#include <vector>

namespace OpenAutoIt
{
// TODO: Add file name, path maybe?
class ASTDocument final : public ASTNode
{
public:
    void AppendStatement(phi::not_null_scope_ptr<ASTStatement> child) noexcept
    {
        m_Statements.emplace_back(phi::move(child));
    }

    void AppendFunction(phi::not_null_scope_ptr<ASTFunctionDefinition> child) noexcept
    {
        m_Functions.emplace_back(phi::move(child));
    }

    [[nodiscard]] phi::observer_ptr<ASTFunctionDefinition> LookupFunctionDefinitionByName(
            phi::string_view function_name) noexcept
    {
        for (phi::not_null_observer_ptr<ASTFunctionDefinition> func_definition : m_Functions)
        {
            const phi::string_view function_definition_name = func_definition->m_FunctionName;

            if (string_equals_ignore_case(function_definition_name, function_name))
            {
                return phi::move(func_definition);
            }
        }

        return nullptr;
    }

    [[nodiscard]] std::string DumpAST(phi::usize indent = 0u) const noexcept override
    {
        std::string ret{"Document:\n"};

        if (!m_Functions.empty())
        {
            ret += "Functions:\n";
        }

        // Add all functions
        for (const auto& function : m_Functions)
        {
            ret += function->DumpAST(indent);
        }

        if (!m_Statements.empty())
        {
            ret += "Statements:\n";
        }

        // Add all statements
        for (const auto& statement : m_Statements)
        {
            ret += statement->DumpAST(indent);
        }

        return ret;
    }

    // TODO: Make private
public:
    std::vector<phi::not_null_scope_ptr<ASTStatement>>          m_Statements;
    std::vector<phi::not_null_scope_ptr<ASTFunctionDefinition>> m_Functions;
};
} // namespace OpenAutoIt
