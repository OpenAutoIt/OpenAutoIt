#pragma once

#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>

namespace OpenAutoIt
{

class FunctionReference
{
public:
    explicit FunctionReference(const phi::string_view function_name)
        : m_IsBuiltIn{false}
        , m_FunctionName{function_name}
    {}

    explicit FunctionReference(const Token& built_in_function)
        : m_IsBuiltIn{true}
        , m_BuiltInFunction{built_in_function.GetTokenKind()}
    {
        PHI_ASSERT(built_in_function.IsBuiltInFunction());
    }

    [[nodiscard]] phi::boolean IsBuiltIn() const
    {
        return m_IsBuiltIn;
    }

    [[nodiscard]] phi::string_view FunctionName() const
    {
        return m_IsBuiltIn ? enum_name(m_BuiltInFunction) : m_FunctionName;
    }

    [[nodiscard]] TokenKind BuiltIn() const
    {
        PHI_ASSERT(IsBuiltIn());

        return m_BuiltInFunction;
    }

    [[nodiscard]] phi::string_view Function() const
    {
        PHI_ASSERT(!IsBuiltIn());

        return m_FunctionName;
    }

private:
    phi::boolean m_IsBuiltIn;
    union
    {
        TokenKind        m_BuiltInFunction;
        phi::string_view m_FunctionName;
    };
};

} // namespace OpenAutoIt
