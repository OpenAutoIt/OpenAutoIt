#pragma once

#include "TokenKind.hpp"
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
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
            PHI_ASSERT(HasHint(), "Token doesn not have a hint");

            return m_Hint.value();
        }

        [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr phi::boolean IsBuiltInFunction() const noexcept
        {
            const phi::size_t underlying_value = static_cast<phi::size_t>(m_Kind);

            if (underlying_value >= OpenAutoIt::BuiltInFirst &&
                underlying_value <= OpenAutoIt::BuiltInLast)
            {
                return true;
            }

            return false;
        }

        [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr phi::boolean IsKeywordLiteral() const noexcept
        {
            switch (m_Kind)
            {
                case TokenKind::KW_Default:
                case TokenKind::KW_Null:
                    return true;

                default:
                    return false;
            }
        }

        [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr phi::boolean IsBooleanLiteral() const noexcept
        {
            switch (m_Kind)
            {
                case TokenKind::KW_True:
                case TokenKind::KW_False:
                    return true;

                default:
                    return false;
            }
        }

    private:
        TokenKind               m_Kind;
        phi::string_view        m_Text;
        phi::u64                m_LineNumber;
        phi::u64                m_Column;
        phi::optional<phi::u32> m_Hint;
    };
} // namespace OpenAutoIt
