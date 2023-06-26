#include "OpenAutoIt/DiagnosticConsumer.hpp"

#include "OpenAutoIt/DiagnosticIds.hpp"
#include <phi/core/assert.hpp>
#include <iostream>

namespace OpenAutoIt
{

namespace
{
    const char* GetDiagnosticLevelName(const Diagnostic& diagnostic)
    {
        switch (diagnostic.GetLevel())
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
        output << note.GetLocation() << ": " << GetDiagnosticLevelName(note) << ": "
               << note.GetMessage() << '\n';
    }
}

} // namespace OpenAutoIt
