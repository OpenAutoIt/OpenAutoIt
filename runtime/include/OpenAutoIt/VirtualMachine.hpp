#pragma once

#include "OpenAutoIt/AST/ASTStatement.hpp"
#include "OpenAutoIt/Variant.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/observer_ptr.hpp>
#include <iostream>
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
        // TODO: Remove Pre/PostStatement stuff
        using PreStatementActionT  = void (*)(phi::not_null_observer_ptr<ASTStatement> statement);
        using PostStatementActionT = void (*)(phi::not_null_observer_ptr<ASTStatement> statement);

        void SetPreStatementAction(PreStatementActionT action) noexcept;
        void PreStatementAction(phi::not_null_observer_ptr<ASTStatement> statement) const noexcept;

        void SetPostStatementAction(PostStatementActionT action) noexcept;
        void PostStatementAction(phi::not_null_observer_ptr<ASTStatement> statement) const noexcept;

        using ConsoleWriteT      = void (*)(phi::string_view text);
        using ConsoleWriteErrorT = void (*)(phi::string_view text);

        //TODO: private
        std::unordered_map<std::string_view, Variant>
                m_Globals; // TODO: Wrap lookups etc. inside function

        ConsoleWriteT      m_ConsoleWrite{default_console_write};
        ConsoleWriteErrorT m_ConsoleWriteError{default_console_write_error};

        // TODO: Remove these
        PreStatementActionT  m_PreStatementAction{nullptr};
        PostStatementActionT m_PostStatementAction{nullptr};
        phi::boolean         m_GracefullyHalt{false};
    };
} // namespace OpenAutoIt
