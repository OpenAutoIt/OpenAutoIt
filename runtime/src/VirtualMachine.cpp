#include "OpenAutoIt/VirtualMachine.hpp"

namespace OpenAutoIt
{
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
} // namespace OpenAutoIt
