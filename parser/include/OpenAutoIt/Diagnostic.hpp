#pragma once

#include "OpenAutoIt/DiagnosticIds.hpp"
#include "OpenAutoIt/SourceLocation.hpp"
#include <phi/core/boolean.hpp>
#include <string>
#include <vector>

namespace OpenAutoIt
{

enum class DiagnosticLevel
{
    Ignored, /// A warning which has been ignored
    Note,    /// Notes display extra information about a warning or error
    Warning, /// A potential problem in the source code
    Error, /// A recoverable error in the source code or a warning which which has been elevated to error
    FatalError, /// A non-recoverable error in the source code
};

class Diagnostic
{
public:
    Diagnostic(DiagnosticId id, DiagnosticLevel level, SourceLocation location,
               std::string&& message);

    [[nodiscard]] DiagnosticId    GetId() const;
    [[nodiscard]] DiagnosticLevel GetLevel() const;
    [[nodiscard]] SourceLocation  GetLocation() const;
    [[nodiscard]] std::string     GetMessage() const;

    [[nodiscard]] phi::boolean IsIgnored() const;
    [[nodiscard]] phi::boolean IsNote() const;
    [[nodiscard]] phi::boolean IsWarning() const;
    [[nodiscard]] phi::boolean IsError() const;
    [[nodiscard]] phi::boolean IsFatalError() const;

    [[nodiscard]] phi::boolean IsErrorOrFatal() const;

    void SetType(DiagnosticLevel level);
    void UpgradeWarningToError();
    void IgnoreWarning();

    [[nodiscard]] const std::vector<Diagnostic>& GetNotes() const;
    void                                         AddNote(Diagnostic&& note);

private:
    DiagnosticId            m_Id;
    DiagnosticLevel         m_Level;
    SourceLocation          m_Location;
    std::string             m_Message;
    std::vector<Diagnostic> m_Notes;
};

} // namespace OpenAutoIt
