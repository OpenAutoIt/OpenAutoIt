#pragma once

#include "OpenAutoIt/ParseResult.hpp"
#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/sized_types.hpp>
#include <phi/core/types.hpp>

namespace OpenAutoIt
{
class Lexer
{
public:
    using iterator = typename phi::string_view::const_iterator;

    explicit Lexer(ParseResult& parse_result) noexcept;

    Lexer(ParseResult& parse_result, phi::string_view source) noexcept;

    void SetInputSource(phi::string_view source) noexcept;

    void Reset() noexcept;

    [[nodiscard]] phi::boolean IsFinished() const noexcept;

    [[nodiscard]] phi::boolean HasInput() const noexcept;

    [[nodiscard]] phi::optional<Token> GetNextToken() noexcept;

    void ProcessAll() noexcept;

    void ProcessString(phi::string_view source) noexcept;

private:
    void ConsumeCurrentCharacter() noexcept;

    void AdvanceToNextLine() noexcept;

    void SkipCurrentCharacter() noexcept;

    [[nodiscard]] constexpr Token ConstructToken(TokenKind kind) noexcept
    {
        Token token{kind,
                    m_Source.substring_view(
                            static_cast<typename phi::string_view::size_type::value_type>(
                                    m_Iterator - m_Source.begin()),
                            1u),
                    m_LineNumber, m_Column};

        // 1 character sized token
        ++m_Column;

        return token;
    }

    [[nodiscard]] constexpr Token ConstructToken(TokenKind kind, iterator token_begin) noexcept
    {
        Token token{kind, TokenText(token_begin), m_LineNumber, m_Column};

        // Consume the amount of characters that make up our new token
        m_Column += static_cast<typename phi::usize::value_type>(m_Iterator - token_begin);

        return token;
    }

    [[nodiscard]] constexpr phi::string_view TokenText(iterator token_begin) noexcept
    {
        return m_Source.substring_view(token_begin, m_Iterator);
    }

    ParseResult&     m_ParseResult;
    phi::string_view m_Source;

    // Lexer state
    iterator m_Iterator;

    phi::boolean m_InsideMultiLineComment{false};

    phi::u64 m_LineNumber{1u};
    phi::u64 m_Column{1u};
};
} // namespace OpenAutoIt
