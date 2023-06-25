#include "OpenAutoIt/DiagnosticBuilder.hpp"

#include "OpenAutoIt/DiagnosticEngine.hpp"
#include <phi/core/move.hpp>

namespace OpenAutoIt
{

DiagnosticBuilder::DiagnosticBuilder(phi::not_null_observer_ptr<DiagnosticEngine> diagnostic_engine)
    : m_DiagnosticEngine{phi::move(diagnostic_engine)}
{}

DiagnosticBuilder::~DiagnosticBuilder()
{
    for (auto& diagnostic : m_Diagnostics)
    {
        m_DiagnosticEngine->Report(diagnostic);
    }
}

} // namespace OpenAutoIt
