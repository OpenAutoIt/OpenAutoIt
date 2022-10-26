#include "OpenAutoIt/ParseError.hpp"

namespace OpenAutoIt
{
    ParseError ParseError::UnexpectedToken(const Token& token) noexcept
    {
        ParseError err;

        err.m_Type = ParseErrorType::UnexpectedToken;

        err.m_LocationBegin.line_number = token.GetLineNumber();
        err.m_LocationBegin.column      = token.GetColumn();

        err.m_LocationEnd.line_number = token.GetLineNumber();
        err.m_LocationEnd.column      = token.GetColumn() + token.GetText().length();

        err.m_UnexpectedToken.m_Kind = token.GetTokenKind();

        return err;
    }
} // namespace OpenAutoIt
