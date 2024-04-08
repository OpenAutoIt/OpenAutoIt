#include "OpenAutoIt/DiagnosticConsumer.hpp"

#include "OpenAutoIt/Diagnostic.hpp"
#include "OpenAutoIt/DiagnosticIds.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <iostream>

namespace OpenAutoIt
{

namespace
{
    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wreturn-type")
    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4702) // unreachable code

    [[nodiscard]] constexpr const char* GetDiagnosticLevelName(
            const DiagnosticLevel& diagnostic_level)
    {
        switch (diagnostic_level)
        {
            case DiagnosticLevel::Note:
                return "note";
            case DiagnosticLevel::Warning:
                return "warning";
            case DiagnosticLevel::Error:
                return "error";
            case DiagnosticLevel::FatalError:
                return "fatal error";

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }

    PHI_MSVC_SUPPRESS_WARNING_POP()
    PHI_GCC_SUPPRESS_WARNING_POP()

    [[nodiscard]] const char* GetDiagnosticLevelName(const Diagnostic& diagnostic)
    {
        return GetDiagnosticLevelName(diagnostic.GetLevel());
    }

} // namespace

void IgnoreDiagnosticConsumer::Report(const Diagnostic& /*diagnostic*/)
{}

void DefaultDiagnosticConsumer::Report(const Diagnostic& diagnostic)
{
    PHI_ASSERT(!diagnostic.IsNote());

    // Don't care about ignored diagnostics
    if (diagnostic.IsIgnored())
    {
        return;
    }

    std::ostream& output = diagnostic.IsErrorOrFatal() ? std::cerr : std::cout;

    const DiagnosticId id = diagnostic.GetId();

    // Print the diagnostic itself
    output << diagnostic.GetLocation() << ": " << GetDiagnosticLevelName(diagnostic) << ": "
           << diagnostic.GetMessage();
    if (id != DiagnosticId::None && DiagnosticIdHasFlagName(id))
    {
        output << " [" << DiagnosticIdFlagName(id) << "]";
    }
    output << '\n';

    // TODO: Print the source text with error marker

    // Print any attached notes
    for (const auto& note : diagnostic.GetNotes())
    {
        PHI_ASSERT(note.IsNote());

        output << note.GetLocation() << ": " << GetDiagnosticLevelName(DiagnosticLevel::Note)
               << ": " << note.GetMessage() << '\n';
    }
}

} // namespace OpenAutoIt
