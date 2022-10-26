#include "OpenAutoIt/ParseWarning.hpp"

#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/warning.hpp>

namespace OpenAutoIt
{
    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wsuggest-attribute=pure")

    PHI_ATTRIBUTE_CONST ParseWarning ParseWarning::EmbeddedNullCharacter(phi::u64 line,
                                                                         phi::u64 column) noexcept
    {
        ParseWarning warn;

        warn.m_Type = ParseWarningType::EmbeddedNullCharacter;

        warn.m_LocationBegin.line_number = line;
        warn.m_LocationBegin.column      = column;

        warn.m_LocationEnd.line_number = line;
        warn.m_LocationEnd.column      = column;

        return warn;
    }

    PHI_GCC_SUPPRESS_WARNING_POP()
} // namespace OpenAutoIt
