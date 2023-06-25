#pragma once

#include "OpenAutoIt/Diagnostic.hpp"
#include <phi/core/boolean.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/types.hpp>

namespace OpenAutoIt
{

class DiagnosticConsumer;

class DiagnosticEngine
{
public:
    DiagnosticEngine();

    DiagnosticEngine(phi::observer_ptr<DiagnosticConsumer> consumer);

    [[nodiscard]] phi::u64 GetNumberOfWarnings() const;

    [[nodiscard]] phi::u64 GetNumberOfError() const;

    [[nodiscard]] phi::u64 GetErrorLimit() const;
    void                   SetErrorLimit(phi::u64 limit);

    [[nodiscard]] phi::boolean HasErrorOccurred() const;

    [[nodiscard]] phi::boolean GetIgnoreAllWarnings() const;
    void                       SetIgnoreAllWarnings(phi::boolean ignore);

    [[nodiscard]] phi::boolean GetWarningsAsError() const;
    void                       SetWarningsAsError(phi::boolean warnings_as_error);

    void Report(Diagnostic diagnostic);

private:
    /// Total number of warnings emitted
    phi::u64 m_NumberOfWarnings{0u};

    /// Total number of error emitted
    phi::u64 m_NumberOfErrors{0u};

    /// Limit for number of error before stopping. Use 0 for unlimited
    phi::u64 m_ErrorLimit{0u};

    /// Optional diagnostic consumer
    phi::observer_ptr<DiagnosticConsumer> m_Consumer;

    /// Sticky flag for whether an error or fatal error has occurred
    bool m_ErrorOccurred : 1 {false};

    /// Ignore all warnings: -w
    bool m_IgnoreAllWarnings : 1 {false};

    /// Treat all warnings as errors: -Werror
    bool m_WarningsAsError : 1 {false};
};

} // namespace OpenAutoIt
