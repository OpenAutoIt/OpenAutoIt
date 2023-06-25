#pragma once

#include "OpenAutoIt/Diagnostic.hpp"
#include "OpenAutoIt/DiagnosticIds.hpp"
#include "OpenAutoIt/SourceLocation.hpp"
#include <fmt/format.h>
#include <phi/core/observer_ptr.hpp>

namespace OpenAutoIt
{

class DiagnosticEngine;

/// Helper class for constructing a Diagnostic
class DiagnosticBuilder
{
public:
    DiagnosticBuilder(phi::not_null_observer_ptr<DiagnosticEngine> diagnostic_engine);

    DiagnosticBuilder(const DiagnosticBuilder&) = delete;
    DiagnosticBuilder(DiagnosticBuilder&&)      = delete;

    DiagnosticBuilder& operator=(const DiagnosticBuilder&) = delete;
    DiagnosticBuilder& operator=(DiagnosticBuilder&&)      = delete;

    ~DiagnosticBuilder();

    template <typename... ArgsT>
    DiagnosticBuilder& Note(SourceLocation location, std::string_view format_string,
                            ArgsT&&... args)
    {
        PHI_ASSERT(!m_Diagnostics.empty());

        m_Diagnostics.back().AddNote(
                Diagnostic{DiagnosticId::None, DiagnosticLevel::Note, location,
                           std::string{fmt::format(fmt::runtime(format_string),
                                                   phi::forward<ArgsT>(args)...)}});

        return *this;
    }

    template <typename... ArgsT>
    DiagnosticBuilder& Warning(SourceLocation location, std::string_view format_string,
                               ArgsT&&... args)
    {
        m_Diagnostics.emplace_back(
                Diagnostic{DiagnosticId::None, DiagnosticLevel::Warning, location,
                           fmt::format(fmt::runtime(format_string), phi::forward<ArgsT>(args)...)});

        return *this;
    }

    template <typename... ArgsT>
    DiagnosticBuilder& Warning(DiagnosticId id, SourceLocation location, ArgsT&&... args)
    {
        m_Diagnostics.emplace_back(Diagnostic{
                id, DiagnosticLevel::Warning, location,
                fmt::format(fmt::runtime(DiagnosticIdMessage(id)), phi::forward<ArgsT>(args)...)});

        return *this;
    }

    template <typename... ArgsT>
    DiagnosticBuilder& Error(DiagnosticId id, SourceLocation location, ArgsT&&... args)
    {
        m_Diagnostics.emplace_back(Diagnostic{
                id, DiagnosticLevel::Error, location,
                fmt::format(fmt::runtime(DiagnosticIdMessage(id)), phi::forward<ArgsT>(args)...)});

        return *this;
    }

    template <typename... ArgsT>
    DiagnosticBuilder& FatalError(DiagnosticId id, SourceLocation location, ArgsT&&... args)
    {
        m_Diagnostics.emplace_back(Diagnostic{
                id, DiagnosticLevel::FatalError, location,
                fmt::format(fmt::runtime(DiagnosticIdMessage(id)), phi::forward<ArgsT>(args)...)});

        return *this;
    }

    template <typename... ArgsT>
    DiagnosticBuilder& FatalError(SourceLocation location, std::string_view format_string,
                                  ArgsT&&... args)
    {
        m_Diagnostics.emplace_back(
                Diagnostic{DiagnosticId::None, DiagnosticLevel::FatalError, location,
                           fmt::format(fmt::runtime(format_string), phi::forward<ArgsT>(args)...)});

        return *this;
    }

private:
    phi::not_null_observer_ptr<DiagnosticEngine> m_DiagnosticEngine;

    std::vector<Diagnostic> m_Diagnostics;
};

} // namespace OpenAutoIt
