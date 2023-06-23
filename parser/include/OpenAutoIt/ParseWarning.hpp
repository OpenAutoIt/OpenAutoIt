#pragma once

#include "OpenAutoIt/ParseNote.hpp"
#include "OpenAutoIt/SourceLocation.hpp"
#include <forward_list>

namespace OpenAutoIt
{
enum class ParseWarningType
{
    EmbeddedNullCharacter,

    COUNT,
};

class ParseWarning
{
public:
    static ParseWarning EmbeddedNullCharacter(phi::u64 line, phi::u64 column);

    ParseWarningType             m_Type;
    SourceLocation               m_LocationBegin;
    SourceLocation               m_LocationEnd;
    std::forward_list<ParseNote> m_Notes;

    // TODO: This should be private and guarded by getter with asserts
    union warning_data
    {};
};
} // namespace OpenAutoIt
