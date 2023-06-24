#pragma once

#include "OpenAutoIt/SourceFile.hpp"
#include "OpenAutoIt/SourceLocation.hpp"
#include "OpenAutoIt/SourceManager.hpp"
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

    Lexer();

    TokenStream ProcessString(phi::string_view file_name, phi::string_view source);

    TokenStream ProcessFile(phi::observer_ptr<const SourceFile> source);

private:
    void Reset();

    [[nodiscard]] phi::boolean IsFinished() const;

    [[nodiscard]] phi::optional<Token> GetNextToken();

    void ConsumeCurrentCharacter();

    void AdvanceToNextLine();

    void SkipCurrentCharacter();

    [[nodiscard]] constexpr Token ConstructToken(TokenKind kind)
    {
        Token token{kind,
                    m_Source.substring_view(
                            static_cast<typename phi::string_view::size_type::value_type>(
                                    m_Iterator - m_Source.cbegin()),
                            1u),
                    BuildSourceLocation(m_LineNumber, m_Column)};

        // 1 character sized token
        ++m_Column;

        return token;
    }

    [[nodiscard]] constexpr Token ConstructToken(TokenKind kind, iterator token_begin)
    {
        Token token{kind, TokenText(token_begin), BuildSourceLocation(m_LineNumber, m_Column)};

        // Consume the amount of characters that make up our new token
        m_Column += static_cast<typename phi::usize::value_type>(m_Iterator - token_begin);

        return token;
    }

    [[nodiscard]] constexpr phi::string_view TokenText(iterator token_begin)
    {
        return m_Source.substring_view(token_begin, m_Iterator);
    }

    [[nodiscard]] constexpr SourceLocation BuildSourceLocation(phi::u64 line_number,
                                                               phi::u64 column) const
    {
        return {m_SourceFile, line_number, column};
    }

    phi::observer_ptr<const SourceFile> m_SourceFile;
    phi::string_view                    m_Source;

    // Lexer state
    iterator m_Iterator;

    phi::boolean m_InsideMultiLineComment{false};

    phi::u64 m_LineNumber{1u};
    phi::u64 m_Column{1u};
};

} // namespace OpenAutoIt
