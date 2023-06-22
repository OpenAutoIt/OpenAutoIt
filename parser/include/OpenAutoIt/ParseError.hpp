#pragma once

#include "OpenAutoIt/ParseNote.hpp"
#include "OpenAutoIt/SourceLocation.hpp"
#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include <phi/core/source_location.hpp>
#include <forward_list>

namespace OpenAutoIt
{
enum class ParseErrorType
{
    UnexpectedToken,

    COUNT,
};

struct ParseErrorUnexpectedToken
{
    TokenKind m_Kind;
};

class ParseError
{
public:
    static ParseError UnexpectedToken(const Token& token) noexcept;

    ParseErrorType               m_Type;
    SourceLocation               m_LocationBegin;
    SourceLocation               m_LocationEnd;
    std::forward_list<ParseNote> m_Notes;

    // TODO: This should be private and guarded by getter with asserts
    union
    {
        ParseErrorUnexpectedToken m_UnexpectedToken;
    };
};
} // namespace OpenAutoIt
