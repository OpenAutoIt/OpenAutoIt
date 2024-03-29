#include "OpenAutoIt/VirtualMachine.hpp"

#include "OpenAutoIt/Scope.hpp"
#include "OpenAutoIt/StackTraceEntry.hpp"
#include "OpenAutoIt/VariableScope.hpp"
#include "OpenAutoIt/Variant.hpp"
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/observer_ptr.hpp>

PHI_GCC_SUPPRESS_WARNING("-Wsuggest-attribute=pure")

namespace OpenAutoIt
{
void VirtualMachine::PushFunctionScope(std::string_view function_name, Statements& statements)
{
    m_Scopes.emplace_front(ScopeKind::Function, function_name, statements);
}

void VirtualMachine::PushBlockScope(Statements& statements)
{
    m_Scopes.emplace_front(ScopeKind::Block, "<block_scope>", statements);
}

void VirtualMachine::PushGlobalScope(Statements& statements)
{
    m_Scopes.emplace_back(ScopeKind::Function, "<global>", statements);
}

void VirtualMachine::PopScope()
{
    m_Scopes.pop_front();
}

PHI_ATTRIBUTE_PURE Scope& VirtualMachine::GetCurrentScope()
{
    PHI_ASSERT(!m_Scopes.empty());

    return m_Scopes.front();
}

PHI_ATTRIBUTE_PURE const Scope& VirtualMachine::GetCurrentScope() const
{
    PHI_ASSERT(!m_Scopes.empty());

    return m_Scopes.front();
}

PHI_ATTRIBUTE_PURE Scope& VirtualMachine::GetGlobalScope()
{
    PHI_ASSERT(!m_Scopes.empty());

    return m_Scopes.back();
}

PHI_ATTRIBUTE_PURE const Scope& VirtualMachine::GetGlobalScope() const
{
    PHI_ASSERT(!m_Scopes.empty());

    return m_Scopes.back();
}

StackTrace VirtualMachine::GetStackTrace() const
{
    // Count number of function elements
    phi::u64 count = 0u;
    for (const Scope& scope : m_Scopes)
    {
        if (scope.kind == ScopeKind::Function)
        {
            count += 1u;
        }
    }

    StackTrace stack_trace;
    stack_trace.reserve(count.unsafe());

    // Populate the stacktrace
    for (const Scope& scope : m_Scopes)
    {
        if (scope.kind == ScopeKind::Function)
        {
            // TODO: Line and Column not implemented
            stack_trace.emplace_back(
                    StackTraceEntry{.file = "", .function = scope.name, .line = 0u, .column = 0u});
        }
    }

    return phi::move(stack_trace);
}

phi::boolean VirtualMachine::PushVariable(std::string_view name, Variant value)
{
    Scope& current_scope = GetCurrentScope();

    if (current_scope.variables.contains(name))
    {
        return false;
    }

    current_scope.variables[name] = phi::move(value);
    return true;
}

phi::boolean VirtualMachine::PushVariableGlobal(std::string_view name, Variant value)
{
    Scope& global_scope = GetGlobalScope();

    if (global_scope.variables.contains(name))
    {
        return false;
    }

    global_scope.variables[name] = phi::move(value);
    return true;
}

phi::boolean VirtualMachine::PushVariableWithScope(std::string_view name, Variant value,
                                                   VariableScope scope)
{
    switch (scope)
    {
        case VariableScope::Global:
            return PushVariableGlobal(name, phi::move(value));

        default:
            return PushVariable(name, phi::move(value));
    }
}

void VirtualMachine::PushOrAssignVariable(std::string_view name, Variant value)
{
    auto variable_opt = LookupVariableRefByName(name);
    if (variable_opt)
    {
        // Overwrite current value
        variable_opt.value() = phi::move(value);
        return;
    }

    Scope& current_scope          = GetCurrentScope();
    current_scope.variables[name] = phi::move(value);
}

phi::optional<Variant> VirtualMachine::LookupVariableByName(std::string_view variable_name) const
{
    auto variable = LookupVariableRefByName(variable_name);
    if (variable.has_value())
    {
        return *variable;
    }

    return {};
}

phi::optional<Variant&> VirtualMachine::LookupVariableRefByName(std::string_view variable_name)
{
    phi::boolean found_function_boundary{false};

    for (Scope& scope : m_Scopes)
    {
        if (scope.kind == ScopeKind::Function)
        {
            if (found_function_boundary)
            {
                // We hit the function boundary so only check the global scope and don't continue
                Scope& global_scope = GetGlobalScope();
                if (global_scope.variables.contains(variable_name))
                {
                    return global_scope.variables.at(variable_name);
                }

                return {};
            }

            found_function_boundary = true;
        }

        if (scope.variables.contains(variable_name))
        {
            return scope.variables.at(variable_name);
        }
    }

    return {};
}

phi::optional<const Variant&> VirtualMachine::LookupVariableRefByName(
        std::string_view variable_name) const
{
    auto res = const_cast<VirtualMachine&>(*this).LookupVariableRefByName(variable_name);
    if (res.has_value())
    {
        return res.value();
    }

    return {};
}

PHI_ATTRIBUTE_PURE phi::boolean VirtualMachine::CanRun() const
{
    return !m_Scopes.empty() && !m_Aborting;
}

void VirtualMachine::Exit(phi::u32 exit_code)
{
    m_Scopes.clear();
    m_ExitCode = exit_code;

    // TODO: Push scopes of registered on exit functions
}

phi::u32 VirtualMachine::GetExitCode() const
{
    return m_ExitCode;
}

void VirtualMachine::SetupOutputHandler(OutputHandler standard, OutputHandler error)
{
    m_StandardOutputHandler = standard;
    m_ErrorOutputHandler    = error;
}

void VirtualMachine::Print(const std::string& message) const
{
    if (m_StandardOutputHandler != nullptr)
    {
        m_StandardOutputHandler(message);
    }
}

void VirtualMachine::PrintError(const std::string& message) const
{
    if (m_ErrorOutputHandler != nullptr)
    {
        m_ErrorOutputHandler(message);
    }
}

} // namespace OpenAutoIt
