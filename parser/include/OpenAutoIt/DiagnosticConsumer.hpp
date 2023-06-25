#pragma once

#include "OpenAutoIt/Diagnostic.hpp"
#include <phi/core/boolean.hpp>
#include <phi/core/types.hpp>

namespace OpenAutoIt
{

/// Abstract class for consuming diagnostics
class DiagnosticConsumer
{
public:
    DiagnosticConsumer()          = default;
    virtual ~DiagnosticConsumer() = default;

    DiagnosticConsumer(const DiagnosticConsumer&) = delete;
    DiagnosticConsumer(DiagnosticConsumer&&)      = delete;

    DiagnosticConsumer& operator=(const DiagnosticConsumer&) = delete;
    DiagnosticConsumer& operator=(DiagnosticConsumer&&)      = delete;

    virtual void Report(const Diagnostic& /*diagnostic*/) = 0;
};

/// A diagnostic consumer which simply ignores the diagnostics
class IgnoreDiagnosticConsumer final : public DiagnosticConsumer
{
public:
    void Report(const Diagnostic& /*diagnostic*/) override;
};

/// The default consumer prints all warnings to stdout and errors to stderr
class DefaultDiagnosticConsumer final : public DiagnosticConsumer
{
public:
    void Report(const Diagnostic& diagnostic) override;
};

} // namespace OpenAutoIt
