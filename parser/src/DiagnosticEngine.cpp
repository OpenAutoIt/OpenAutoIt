#include "OpenAutoIt/DiagnosticEngine.hpp"

#include "OpenAutoIt/Diagnostic.hpp"
#include "OpenAutoIt/DiagnosticConsumer.hpp"
#include <phi/core/move.hpp>

namespace OpenAutoIt
{

DiagnosticEngine::DiagnosticEngine()
    : m_Consumer{nullptr}
{}

DiagnosticEngine::DiagnosticEngine(phi::observer_ptr<DiagnosticConsumer> consumer)
    : m_Consumer{phi::move(consumer)}
{}

phi::u64 DiagnosticEngine::GetNumberOfWarnings() const
{
    return m_NumberOfWarnings;
}

phi::u64 DiagnosticEngine::GetNumberOfError() const
{
    return m_NumberOfErrors;
}

phi::u64 DiagnosticEngine::GetErrorLimit() const
{
    return m_ErrorLimit;
}

void DiagnosticEngine::SetErrorLimit(phi::u64 limit)
{
    m_ErrorLimit = limit;
}

phi::boolean DiagnosticEngine::HasErrorOccurred() const
{
    return m_ErrorOccurred;
}

phi::boolean DiagnosticEngine::GetIgnoreAllWarnings() const
{
    return m_IgnoreAllWarnings;
}

void DiagnosticEngine::SetIgnoreAllWarnings(phi::boolean ignore)
{
    m_IgnoreAllWarnings = ignore.unsafe();
}

phi::boolean DiagnosticEngine::GetWarningsAsError() const
{
    return m_WarningsAsError;
}

void DiagnosticEngine::SetWarningsAsError(phi::boolean warnings_as_error)
{
    m_WarningsAsError = warnings_as_error.unsafe();
}

void DiagnosticEngine::Report(Diagnostic diagnostic)
{
    PHI_ASSERT(!diagnostic.IsNote());

    if (m_IgnoreAllWarnings)
    {
        diagnostic.IgnoreWarning();
    }

    if (m_WarningsAsError)
    {
        diagnostic.UpgradeWarningToError();
    }

    if (diagnostic.IsErrorOrFatal())
    {
        m_ErrorOccurred = true;
        ++m_NumberOfErrors;
    }

    if (diagnostic.IsWarning())
    {
        ++m_NumberOfWarnings;
    }

    if (m_Consumer)
    {
        m_Consumer->Report(diagnostic);
    }
}

} // namespace OpenAutoIt
