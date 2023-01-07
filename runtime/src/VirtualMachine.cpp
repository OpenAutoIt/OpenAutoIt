#include "OpenAutoIt/VirtualMachine.hpp"
#include "OpenAutoIt/FunctionScope.hpp"
#include "OpenAutoIt/VariableScope.hpp"
#include <phi/core/assert.hpp>

namespace OpenAutoIt
{
    VirtualMachine::VirtualMachine()
    {
        // Push global scope
        PushFunctionScope("<global>");
    }

    void VirtualMachine::SetPreStatementAction(VirtualMachine::PreStatementActionT action) noexcept
    {
        m_PreStatementAction = action;
    }

    void VirtualMachine::PreStatementAction(
            phi::not_null_observer_ptr<ASTStatement> statement) const noexcept
    {
        if (m_PreStatementAction)
        {
            m_PreStatementAction(statement);
        }
    }

    void VirtualMachine::SetPostStatementAction(
            VirtualMachine::PostStatementActionT action) noexcept
    {
        m_PostStatementAction = action;
    }

    void VirtualMachine::PostStatementAction(
            phi::not_null_observer_ptr<ASTStatement> statement) const noexcept
    {
        if (m_PostStatementAction)
        {
            m_PostStatementAction(statement);
        }
    }

    void VirtualMachine::PushFunctionScope(std::string_view function_name) noexcept
    {
        m_FunctionScopes.emplace_front(function_name);
    }

    void VirtualMachine::PopFunctionScope() noexcept
    {
        PHI_ASSERT(m_FunctionScopes.size() > 1u, "You cannot pop the global scope");

        m_FunctionScopes.pop_front();
    }

    FunctionScope& VirtualMachine::GetLocalScope() noexcept
    {
        return m_FunctionScopes.front();
    }

    const FunctionScope& VirtualMachine::GetLocalScope() const noexcept
    {
        return m_FunctionScopes.front();
    }

    FunctionScope& VirtualMachine::GetGlobalScope() noexcept
    {
        return m_FunctionScopes.back();
    }

    const FunctionScope& VirtualMachine::GetGlobalScope() const noexcept
    {
        return m_FunctionScopes.back();
    }

    phi::boolean VirtualMachine::PushVariable(std::string_view name, Variant value) noexcept
    {
        FunctionScope& current_scope = GetLocalScope();

        if (current_scope.variables.contains(name))
        {
            return false;
        }

        current_scope.variables[name] = phi::move(value);
        return true;
    }

    phi::boolean VirtualMachine::PushVariableGlobal(std::string_view name, Variant value) noexcept
    {
        FunctionScope& global_scope = GetGlobalScope();

        if (global_scope.variables.contains(name))
        {
            return false;
        }

        global_scope.variables[name] = phi::move(value);
        return true;
    }

    phi::boolean VirtualMachine::PushVariableWithScope(std::string_view name, Variant value,
                                                       VariableScope scope) noexcept
    {
        switch (scope)
        {
            case VariableScope::Global:
                return PushVariableGlobal(name, phi::move(value));

            default:
                return PushVariable(name, phi::move(value));
        }
    }

    void VirtualMachine::PushOrAssignVariable(std::string_view name, Variant value) noexcept
    {
        FunctionScope& current_scope = GetLocalScope();

        current_scope.variables.insert_or_assign(name, value);
    }

    phi::optional<Variant> VirtualMachine::LookupVariableByName(
            std::string_view variable_name) const noexcept
    {
        for (const FunctionScope& scope : m_FunctionScopes)
        {
            if (scope.variables.contains(variable_name))
            {
                return scope.variables.at(variable_name);
            }
        }

        return {};
    }
} // namespace OpenAutoIt
