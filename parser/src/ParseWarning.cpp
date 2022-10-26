#include "OpenAutoIt/ParseWarning.hpp"

namespace OpenAutoIt
{
    ParseWarning ParseWarning::EmbeddedNullCharacter(phi::u64 line, phi::u64 column) noexcept
    {
        ParseWarning warn;

        warn.m_Type = ParseWarningType::EmbeddedNullCharacter;

        warn.m_LocationBegin.line_number = line;
        warn.m_LocationBegin.column      = column;

        warn.m_LocationEnd.line_number = line;
        warn.m_LocationEnd.column      = column;

        return warn;
    }
} // namespace OpenAutoIt
