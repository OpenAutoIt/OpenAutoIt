#pragma once

#include <phi/container/string_view.hpp>
#include <phi/core/types.hpp>

namespace OpenAutoIt
{
    class Token
    {
    public:
        enum class Type
        {
            Invalid,
            EndOfFile,
            NewLine,
            VariableIdentifier,
            MacroIdentifier,
            Identifier,
            IntegerLiteral,
            FloatLiteral,
            StringLiteral,
            PreProcessorIdentifier,
            Operator,
            Comment,
        };

        constexpr Token(Type type, phi::string_view text, phi::u64 line_number,
                        phi::u64 column) noexcept
            : m_Type{type}
            , m_Text{text}
            , m_LineNumber{line_number}
            , m_Column{column}
        {}

        [[nodiscard]] constexpr Type GetType() const noexcept
        {
            return m_Type;
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

    private:
        Type             m_Type;
        phi::string_view m_Text;
        phi::u64         m_LineNumber;
        phi::u64         m_Column;
    };
} // namespace OpenAutoIt
