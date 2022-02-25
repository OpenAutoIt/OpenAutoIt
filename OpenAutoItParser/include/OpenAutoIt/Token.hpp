#pragma once

#include "TokenKind.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/types.hpp>

namespace OpenAutoIt
{
    class Token
    {
    public:
        constexpr Token(TokenKind kind, phi::string_view text, phi::u64 line_number,
                        phi::u64 column) noexcept
            : m_Kind{kind}
            , m_Text{text}
            , m_LineNumber{line_number}
            , m_Column{column}
            , m_Hint{}
        {}

        template <typename HintT>
        constexpr Token(TokenKind kind, phi::string_view text, phi::u64 line_number,
                        phi::u64 column, const HintT hint) noexcept
            : m_Kind{kind}
            , m_Text{text}
            , m_LineNumber{line_number}
            , m_Column{column}
            , m_Hint{static_cast<phi::uint32_t>(hint)}
        {}

        [[nodiscard]] constexpr TokenKind GetTokenKind() const noexcept
        {
            return m_Kind;
        }

        [[nodiscard]] constexpr phi::string_view GetText() const noexcept
        {
            return m_Text;
        }

        [[nodiscard]] constexpr phi::u64 GetLineNumber() const noexcept
        {
            return m_LineNumber;
        }

        [[nodiscard]] constexpr phi::u64 GetColumn() const noexcept
        {
            return m_Column;
        }

        [[nodiscard]] constexpr phi::boolean HasHint() const noexcept
        {
            return m_Hint.has_value();
        }

        [[nodiscard]] constexpr phi::u32 GetHint() const noexcept
        {
            PHI_DBG_ASSERT(HasHint(), "Token doesn not have a hint");

            return m_Hint.value();
        }

    private:
        TokenKind               m_Kind;
        phi::string_view        m_Text;
        phi::u64                m_LineNumber;
        phi::u64                m_Column;
        phi::optional<phi::u32> m_Hint;
    };
} // namespace OpenAutoIt
