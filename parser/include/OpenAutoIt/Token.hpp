#pragma once

#include "OpenAutoIt/SourceFile.hpp"
#include "OpenAutoIt/SourceLocation.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/move.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/size_t.hpp>
#include <phi/core/types.hpp>

namespace OpenAutoIt
{

class Token
{
public:
    constexpr Token(TokenKind kind, phi::string_view text, SourceLocation location)
        : m_Kind{kind}
        , m_Text{text}
        , m_SourceLocation{phi::move(location)}
        , m_Hint{}
    {}

    template <typename HintT>
    constexpr Token(TokenKind kind, phi::string_view text, SourceLocation location,
                    const HintT hint)
        : m_Kind{kind}
        , m_Text{text}
        , m_SourceLocation{phi::move(location)}
        , m_Hint{static_cast<phi::uint32_t>(hint)}
    {}

    [[nodiscard]] constexpr TokenKind GetTokenKind() const
    {
        return m_Kind;
    }

    [[nodiscard]] constexpr phi::string_view GetText() const
    {
        return m_Text;
    }

    [[nodiscard]] constexpr phi::u64 GetLineNumber() const
    {
        return m_SourceLocation.line_number;
    }

    [[nodiscard]] constexpr phi::u64 GetColumn() const
    {
        return m_SourceLocation.column;
    }

    [[nodiscard]] constexpr phi::observer_ptr<const SourceFile> GetSourceFile() const
    {
        return m_SourceLocation.source_file;
    }

    [[nodiscard]] constexpr SourceLocation GetBeginLocation() const
    {
        return m_SourceLocation;
    }

    [[nodiscard]] constexpr SourceLocation GetEndLocation() const
    {
        return {.source_file = m_SourceLocation.source_file,
                .line_number = m_SourceLocation.line_number,
                .column      = m_SourceLocation.column + m_Text.length()};
    }

    [[nodiscard]] constexpr phi::boolean HasHint() const
    {
        return m_Hint.has_value();
    }

    [[nodiscard]] constexpr phi::u32 GetHint() const
    {
        PHI_ASSERT(HasHint(), "Token doesn not have a hint");

        return m_Hint.value();
    }

    [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr phi::boolean IsBuiltInFunction() const
    {
        const phi::size_t underlying_value = static_cast<phi::size_t>(m_Kind);

        if (underlying_value >= OpenAutoIt::BuiltInFirst &&
            underlying_value <= OpenAutoIt::BuiltInLast)
        {
            return true;
        }

        return false;
    }

    [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr phi::boolean IsKeywordLiteral() const
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

    [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr phi::boolean IsBooleanLiteral() const
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

    [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr phi::boolean IsOperator() const
    {
        switch (m_Kind)
        {
            case TokenKind::OP_Equals:
            case TokenKind::OP_PlusEquals:
            case TokenKind::OP_MinusEquals:
            case TokenKind::OP_MultiplyEquals:
            case TokenKind::OP_DivideEquals:
            case TokenKind::OP_Concatenate:
            case TokenKind::OP_ConcatenateEquals:
            case TokenKind::OP_Plus:
            case TokenKind::OP_Minus:
            case TokenKind::OP_Multiply:
            case TokenKind::OP_Divide:
            case TokenKind::OP_Raise:
            case TokenKind::OP_EqualsEquals:
            case TokenKind::OP_NotEqual:
            case TokenKind::OP_GreaterThan:
            case TokenKind::OP_GreaterThanEqual:
            case TokenKind::OP_LessThan:
            case TokenKind::OP_LessThanEqual:
            case TokenKind::OP_TernaryIf:
            case TokenKind::OP_TernaryElse:
                return true;

            default:
                return false;
        }
    }

    [[nodiscard]] PHI_ATTRIBUTE_CONST constexpr phi::boolean IsMacro() const
    {
        const phi::size_t underlying_value = static_cast<phi::size_t>(m_Kind);

        if (underlying_value >= MacroFirst && underlying_value <= MacroLast)
        {
            return true;
        }

        return false;
    }

private:
    TokenKind               m_Kind;
    phi::string_view        m_Text;
    SourceLocation          m_SourceLocation;
    phi::optional<phi::u32> m_Hint;
};

} // namespace OpenAutoIt
