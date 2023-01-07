#pragma once

#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/FunctionScope.hpp"
#include "OpenAutoIt/VariableScope.hpp"
#include "OpenAutoIt/Variant.hpp"
#include <fmt/core.h>
#include <fmt/format.h>
#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/forward.hpp>
#include <phi/core/observer_ptr.hpp>
#include <iostream>
#include <list>
#include <string_view>
#include <unordered_map>

namespace OpenAutoIt
{
    // TODO: Move me somewhere saner
    inline void default_console_write(phi::string_view text) noexcept
    {
        std::cout << std::string_view(text);
    }

    inline void default_console_write_error(phi::string_view text) noexcept
    {
        std::cerr << std::string_view(text);
    }

    // The abstract virtual machine running AutoIt
    class VirtualMachine
    {
    public:
        VirtualMachine();

        // TODO: Remove Pre/PostStatement stuff
        using PreStatementActionT  = void (*)(phi::not_null_observer_ptr<ASTStatement> statement);
        using PostStatementActionT = void (*)(phi::not_null_observer_ptr<ASTStatement> statement);

        void SetPreStatementAction(PreStatementActionT action) noexcept;
        void PreStatementAction(phi::not_null_observer_ptr<ASTStatement> statement) const noexcept;

        void SetPostStatementAction(PostStatementActionT action) noexcept;
        void PostStatementAction(phi::not_null_observer_ptr<ASTStatement> statement) const noexcept;
        // TODO: End of remove me

        template <typename... ArgsT>
        void RuntimeError(std::string_view format_string, ArgsT&&... args) noexcept
        {
            std::cerr << "[OpenAutoIt] RUNTIME ERROR: "
                      << fmt::format(fmt::runtime(format_string), phi::forward<ArgsT>(args)...)
                      << '\n';
            m_Aborting = true;
        }

        void PushFunctionScope(std::string_view function_name) noexcept;
        void PopFunctionScope() noexcept;

        [[nodiscard]] FunctionScope&       GetLocalScope() noexcept;
        [[nodiscard]] const FunctionScope& GetLocalScope() const noexcept;

        [[nodiscard]] FunctionScope&       GetGlobalScope() noexcept;
        [[nodiscard]] const FunctionScope& GetGlobalScope() const noexcept;

        phi::boolean PushVariable(std::string_view name, Variant value) noexcept;
        phi::boolean PushVariableGlobal(std::string_view name, Variant value) noexcept;
        phi::boolean PushVariableWithScope(std::string_view name, Variant value,
                                           VariableScope scope) noexcept;

        void PushOrAssignVariable(std::string_view name, Variant value) noexcept;

        // Needs to be the variable name without a leading $
        [[nodiscard]] phi::optional<Variant> LookupVariableByName(
                std::string_view variable_name) const noexcept;

        using ConsoleWriteT      = void (*)(phi::string_view text);
        using ConsoleWriteErrorT = void (*)(phi::string_view text);

        //TODO: private
        std::list<FunctionScope> m_FunctionScopes;

        ConsoleWriteT      m_ConsoleWrite{default_console_write};
        ConsoleWriteErrorT m_ConsoleWriteError{default_console_write_error};

        // TODO: Remove these
        PreStatementActionT  m_PreStatementAction{nullptr};
        PostStatementActionT m_PostStatementAction{nullptr};
        phi::boolean         m_Aborting{false};
    };
} // namespace OpenAutoIt
