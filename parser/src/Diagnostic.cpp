#include "OpenAutoIt/Diagnostic.hpp"

#include "OpenAutoIt/DiagnosticIds.hpp"

namespace OpenAutoIt
{

Diagnostic::Diagnostic(DiagnosticId id, DiagnosticLevel level, SourceLocation location,
                       std::string&& message)
    : m_Id{id}
    , m_Level{level}
    , m_Location{phi::move(location)}
    , m_Message{phi::move(message)}
{}

DiagnosticId Diagnostic::GetId() const
{
    return m_Id;
}

DiagnosticLevel Diagnostic::GetLevel() const
{
    return m_Level;
}

SourceLocation Diagnostic::GetLocation() const
{
    return m_Location;
}

std::string Diagnostic::GetMessage() const
{
    return m_Message;
}

phi::boolean Diagnostic::IsIgnored() const
{
    return m_Level == DiagnosticLevel::Ignored;
}

phi::boolean Diagnostic::IsNote() const
{
    return m_Level == DiagnosticLevel::Note;
}

phi::boolean Diagnostic::IsWarning() const
{
    return m_Level == DiagnosticLevel::Warning;
}

phi::boolean Diagnostic::IsError() const
{
    return m_Level == DiagnosticLevel::Error;
}

phi::boolean Diagnostic::IsFatalError() const
{
    return m_Level == DiagnosticLevel::FatalError;
}

phi::boolean Diagnostic::IsErrorOrFatal() const
{
    return IsError() || IsFatalError();
}

void Diagnostic::SetType(DiagnosticLevel level)
{
    m_Level = level;
}

void Diagnostic::UpgradeWarningToError()
{
    if (IsWarning())
    {
        SetType(DiagnosticLevel::Error);
    }
}

void Diagnostic::IgnoreWarning()
{
    if (IsWarning())
    {
        SetType(DiagnosticLevel::Ignored);
    }
}

const std::vector<Diagnostic>& Diagnostic::GetNotes() const
{
    return m_Notes;
}

void Diagnostic::AddNote(Diagnostic&& note)
{
    PHI_ASSERT(note.IsNote());

    m_Notes.emplace_back(note);
}

} // namespace OpenAutoIt
