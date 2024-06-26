#include "OpenAutoIt/Variant.hpp"

#include "OpenAutoIt/UnsafeOperations.hpp"
#include <phi/algorithm/clamp.hpp>
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/move.hpp>
#include <phi/core/narrow_cast.hpp>
#include <phi/core/types.hpp>
#include <phi/core/unsafe_cast.hpp>
#include <phi/math/abs.hpp>
#include <functional>
#include <string>

PHI_MSVC_SUPPRESS_WARNING(4702) // unreachable code
PHI_GCC_SUPPRESS_WARNING("-Wsuggest-attribute=const")
PHI_CLANG_SUPPRESS_WARNING("-Wswitch-default")

namespace OpenAutoIt
{

PHI_MSVC_SUPPRESS_WARNING_PUSH()
PHI_MSVC_SUPPRESS_WARNING(4582) // constructor is not implicitly called
PHI_MSVC_SUPPRESS_WARNING(4583) // destructor is not implicitly called

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
Variant::Variant()
    : m_Type{Type::String}
    , string{} // NOLINT(readability-redundant-member-init)
{}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
Variant::Variant(const Variant& other)
    : m_Type{Type::Keyword}
    , keyword{TokenKind::KW_Null}
{
    copy_from(other);
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
Variant::Variant(Variant&& other)
    : m_Type{Type::Keyword}
    , keyword{TokenKind::KW_Null}
{
    move_from(phi::move(other));
}

Variant::~Variant()
{
    Clear();
}

PHI_MSVC_SUPPRESS_WARNING_POP()

Variant& Variant::operator=(const Variant& other)
{
    PHI_ASSERT(this != &other);

    Clear();
    copy_from(other);

    return *this;
}

Variant& Variant::operator=(Variant&& other)
{
    Clear();
    move_from(phi::move(other));

    return *this;
}

void Variant::Clear()
{
    switch (m_Type)
    {
        case Type::Array:
            PHI_ASSERT(array.data());

            array.~vector();
            return;

        case Type::String:
        case Type::Function:
            PHI_ASSERT(string.c_str());

            string.~basic_string();
            return;

        case Type::Binary:
            PHI_ASSERT(binary.c_str());

            binary.~basic_string();
            return;

        default:
            // The other types are trivially destructible
            return;
    }

    PHI_ASSERT_NOT_REACHED();
}

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wsuggest-attribute=pure")

PHI_ATTRIBUTE_CONST Variant::Type Variant::GetType() const
{
    return m_Type;
}

PHI_GCC_SUPPRESS_WARNING_POP()

PHI_ATTRIBUTE_PURE phi::string_view Variant::GetTypeName() const
{
    switch (m_Type)
    {
        case Type::Array:
            return "Array";
        case Type::Binary:
            return "Binary";
        case Type::Boolean:
            return "Boolean";
        case Type::Double:
            return "Double";
        case Type::Function:
            return "Function";
        case Type::Int64:
            return "Int64";
        case Type::Keyword:
            return "Keyword";
        case Type::Pointer:
            return "Pointer";
        case Type::String:
            return "String";
    }

    PHI_ASSERT_NOT_REACHED();
    return "";
}

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wsuggest-attribute=pure")

PHI_ATTRIBUTE_CONST phi::boolean Variant::IsInt64() const
{
    return m_Type == Type::Int64;
}

PHI_ATTRIBUTE_CONST phi::boolean Variant::IsDouble() const
{
    return m_Type == Type::Double;
}

PHI_ATTRIBUTE_CONST phi::boolean Variant::IsBoolean() const
{
    return m_Type == Type::Boolean;
}

PHI_ATTRIBUTE_CONST phi::boolean Variant::IsString() const
{
    return m_Type == Type::String;
}

PHI_ATTRIBUTE_CONST phi::boolean Variant::IsNumeric() const
{
    switch (m_Type)
    {
        case Type::Double:
        case Type::Int64:
            return true;

        default:
            return false;
    }
}

PHI_ATTRIBUTE_CONST phi::boolean Variant::IsBinary() const
{
    return m_Type == Type::Binary;
}

PHI_ATTRIBUTE_CONST phi::boolean Variant::IsPointer() const
{
    return m_Type == Type::Pointer;
}

PHI_ATTRIBUTE_CONST phi::boolean Variant::IsArray() const
{
    return m_Type == Type::Array;
}

PHI_ATTRIBUTE_CONST phi::boolean Variant::IsFunction() const
{
    return m_Type == Type::Function;
}

PHI_ATTRIBUTE_CONST phi::boolean Variant::IsKeyword() const
{
    return m_Type == Type::Keyword;
}

PHI_ATTRIBUTE_CONST phi::boolean Variant::IsDefault() const
{
    return m_Type == Type::Keyword && keyword == TokenKind::KW_Default;
}

PHI_ATTRIBUTE_CONST phi::boolean Variant::IsNull() const
{
    return m_Type == Type::Keyword && keyword == TokenKind::KW_Null;
}

PHI_GCC_SUPPRESS_WARNING_POP()

// Access to the underlying types

PHI_ATTRIBUTE_PURE phi::i64& Variant::AsInt64()
{
    PHI_ASSERT(m_Type == Type::Int64);

    return int64;
}

PHI_ATTRIBUTE_PURE const phi::i64& Variant::AsInt64() const
{
    PHI_ASSERT(m_Type == Type::Int64);

    return int64;
}

PHI_ATTRIBUTE_PURE phi::f64& Variant::AsDouble()
{
    PHI_ASSERT(m_Type == Type::Double);

    return floating_point;
}

PHI_ATTRIBUTE_PURE const phi::f64& Variant::AsDouble() const
{
    PHI_ASSERT(m_Type == Type::Double);

    return floating_point;
}

PHI_ATTRIBUTE_PURE phi::boolean& Variant::AsBoolean()
{
    PHI_ASSERT(m_Type == Type::Boolean);

    return boolean;
}

PHI_ATTRIBUTE_PURE const phi::boolean& Variant::AsBoolean() const
{
    PHI_ASSERT(m_Type == Type::Boolean);

    return boolean;
}

PHI_ATTRIBUTE_PURE string_t& Variant::AsString()
{
    PHI_ASSERT(m_Type == Type::String);

    return string;
}

PHI_ATTRIBUTE_PURE const string_t& Variant::AsString() const
{
    PHI_ASSERT(m_Type == Type::String);

    return string;
}

PHI_ATTRIBUTE_PURE binary_t& Variant::AsBinary()
{
    PHI_ASSERT(m_Type == Type::Binary);

    return binary;
}

PHI_ATTRIBUTE_PURE const binary_t& Variant::AsBinary() const
{
    PHI_ASSERT(m_Type == Type::Binary);

    return binary;
}

PHI_ATTRIBUTE_PURE ptr_t& Variant::AsPointer()
{
    PHI_ASSERT(m_Type == Type::Pointer);

    return pointer;
}

PHI_ATTRIBUTE_PURE const ptr_t& Variant::AsPointer() const
{
    PHI_ASSERT(m_Type == Type::Pointer);

    return pointer;
}

PHI_ATTRIBUTE_PURE array_t& Variant::AsArray()
{
    PHI_ASSERT(m_Type == Type::Array);

    return array;
}

PHI_ATTRIBUTE_PURE const array_t& Variant::AsArray() const
{
    PHI_ASSERT(m_Type == Type::Array);

    return array;
}

PHI_ATTRIBUTE_PURE string_t& Variant::AsFunction()
{
    PHI_ASSERT(m_Type == Type::Function);

    return string;
}

PHI_ATTRIBUTE_PURE const string_t& Variant::AsFunction() const
{
    PHI_ASSERT(m_Type == Type::Function);

    return string;
}

PHI_ATTRIBUTE_PURE OpenAutoIt::TokenKind& Variant::AsKeyword()
{
    PHI_ASSERT(m_Type == Type::Keyword);

    return keyword;
}

PHI_ATTRIBUTE_PURE const OpenAutoIt::TokenKind& Variant::AsKeyword() const
{
    PHI_ASSERT(m_Type == Type::Keyword);

    return keyword;
}

PHI_ATTRIBUTE_CONST Variant Variant::CastToBinary() const
{
    // TODO: Implement me

    return {};
}

Variant Variant::CastToBoolean() const
{
    // https://www.autoitscript.com/autoit3/docs/intro/lang_datatypes.htm
    // "Strings and numbers can be used as Booleans. An empty string "" equals Boolean False as does the number 0. Any other number value will be equal to Boolean True"
    // "Only an empty string ("") will be a Boolean False.
    // Any other string values(including a string equal "0") will be a Boolean True."

    switch (m_Type)
    {
        case Type::Array:
        case Type::Function:
        case Type::Keyword:
            // These types are always false regardless of their value
            return Variant::MakeBoolean(false);

        case Type::Binary: {
            // TODO:
            return {};
        }

        case Type::Boolean:
            // Nothing todo as we already have a boolean
            return *this;

        case Type::Double: {
            const phi::f64 value = AsDouble();

            PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wfloat-equal")

            // Everything apart from "0.0" is considered true
            return MakeBoolean(value.unsafe() != 0.0);

            PHI_GCC_SUPPRESS_WARNING_POP()
        }

        case Type::Int64: {
            const phi::i64 value = AsInt64();

            // Everything apart from "0" is considered true
            return MakeBoolean(value != 0);
        }

        case Type::Pointer: {
            const ptr_t value = AsPointer();

            // Everything apart from "nullptr/0" is considered true
            return MakeBoolean(value != 0);
        }

        case Type::String: {
            const string_t value = AsString();

            // Every apart from the empty string "" is considered true
            return MakeBoolean(!value.empty());
        }
    }

    PHI_ASSERT_NOT_REACHED();
    return {};
}

PHI_ATTRIBUTE_CONST Variant Variant::CastToDouble() const
{
    // TODO
    return {};
}

PHI_ATTRIBUTE_CONST Variant Variant::CastToInt64() const
{
    switch (m_Type)
    {
        case Type::Boolean: {
            return MakeInt(AsBoolean() ? 1 : 0);
        }

        case Type::Double:
            return MakeInt(ConvertDoubleToInt64());

        // Nothing todo here since we're already an int
        case Type::Int64:
            return *this;

        // Keywords are always 0
        case Type::Keyword: {
            return MakeInt(0);
        }

        case Type::Pointer: {
            return MakeInt(static_cast<phi::int64_t>(AsPointer()));
        }

        case Type::String: {
            const string_t value = AsString();

            const phi::int64_t int64_value = std::strtol(value.c_str(), nullptr, 10);

            return MakeInt(int64_value);
        }

        default:
            return Variant::MakeInt(0);
    }
}

PHI_ATTRIBUTE_CONST Variant Variant::CastToPointer() const
{
    // TODO:
    return {};
}

Variant Variant::CastToString() const
{
    // https://www.autoitscript.com/autoit3/docs/functions/String.htm
    switch (m_Type)
    {
        case Type::Array: {
            // TODO:
            return {};
        }

        case Type::Binary: {
            // TODO:
            return {};
        }

        case Type::Boolean: {
            const phi::boolean value = AsBoolean();

            static const constexpr phi::string_view string_true{"True"};
            static const constexpr phi::string_view string_false{"False"};

            return MakeString(value ? string_true : string_false);
        }

        case Type::Double: {
            const phi::f64 value = AsDouble();

            // TODO: std::to_string outputs our values with trailing zeros which is not what we want
            return MakeString(std::to_string(value.unsafe()));
        }

        case Type::Function: {
            // TODO:

            return {};
        }

        case Type::Int64: {
            const phi::i64 value = AsInt64();

            return MakeString(std::to_string(value.unsafe()));
        }

        case Type::Keyword: {
            const OpenAutoIt::TokenKind value = AsKeyword();

            static const constexpr phi::string_view string_default{"Default"};

            switch (value)
            {
                case OpenAutoIt::TokenKind::KW_Default:
                    return MakeString(string_default);

                // NOTE: Null actually returns an empty string
                case OpenAutoIt::TokenKind::KW_Null:
                    return {};

                default:
                    PHI_ASSERT_NOT_REACHED();
            }

            PHI_ASSERT_NOT_REACHED();
            break;
        }

        case Type::Pointer: {
            // TODO:
            return {};
        }

        case Type::String:
            // Return as is
            return *this;
    }

    PHI_ASSERT_NOT_REACHED();
    return {};
}

Variant Variant::CastToNumeric() const
{
    switch (m_Type)
    {
        // Nothing todo here
        case Type::Int64:
        case Type::Double:
            return *this;

        case Type::String: {
            // TODO: Instead of converting the same string twice, we could write our own function to do this
            string_t value = AsString();

            // First attempt to convert to a double
            char*        double_end_ptr = value.data() + value.length();
            const double double_value   = strtod(value.c_str(), &double_end_ptr);

            char*              int64_end_ptr = value.data() + value.length();
            const phi::int64_t int64_value   = std::strtol(value.c_str(), &int64_end_ptr, 10);

            // Use the double value if that parsed more otherwise use the int64
            if (double_end_ptr > int64_end_ptr)
            {
                return MakeDouble(double_value);
            }

            return MakeInt(int64_value);
        }

        // All other types are simply cast to int64
        default:
            return CastToInt64();
    }
}

Variant Variant::Add(const Variant& other) const
{
    (void)other;
    // TODO:
    return {};
}

Variant Variant::Subtract(const Variant& other) const
{
    (void)other;
    // TODO:
    return {};
}

Variant Variant::Multiply(const Variant& other) const
{
    (void)other;
    // TODO:
    return {};
}

Variant Variant::Divide(const Variant& other) const
{
    (void)other;
    // TODO:
    return {};
}

Variant Variant::Concatenate(const Variant& other) const
{
    const Variant this_string  = CastToString();
    const Variant other_string = other.CastToString();

    const string_t string_value = this_string.AsString() + other_string.AsString();

    return Variant::MakeString(phi::move(string_value));
}

// https://www.autoitscript.com/autoit3/docs/functions/Abs.htm
// NOTE: The documentation is actually wrong here. The String "1" returns 1 not 0
//       as the string is first cast to an integer and then the abs function is called.
//       The same goes for "2.0" which returns 2.0.
Variant Variant::Abs() const
{
    switch (m_Type)
    {
        case Type::Double:
            return MakeDouble(phi::abs(AsDouble()));

        case Type::Int64:
            return MakeInt(UnsafeAbs(AsInt64()));

        case Type::String: {
            // For a string we first convert to a numeric (double or int64)
            const Variant numeric = CastToNumeric();
            const Variant abs     = numeric.Abs();

            return phi::move(abs);
        }

        // For all other types cast to int and the call abs
        default:
            return CastToInt64().Abs();
    }
}

Variant Variant::UnaryMinus() const
{
    switch (m_Type)
    {
        case Type::Double:
            return MakeDouble(-AsDouble());

        case Type::Int64:
            return MakeInt(UnsafeNegate(AsInt64()));

        case Type::String:
            return CastToNumeric().UnaryMinus();

        default:
            return {};
    }
}

Variant Variant::UnaryNot() const
{
    // TODO: Implement me

    return {};
}

PHI_ATTRIBUTE_CONST Variant Variant::MakeUndefined()
{
    return Variant{};
}

PHI_ATTRIBUTE_CONST Variant Variant::MakeBoolean(phi::boolean value)
{
    Variant variant;

    variant.m_Type  = Type::Boolean;
    variant.boolean = value;

    return variant;
}

PHI_ATTRIBUTE_CONST Variant Variant::MakeDouble(phi::f64 value)
{
    Variant variant;

    variant.m_Type         = Type::Double;
    variant.floating_point = value;

    return variant;
}

PHI_ATTRIBUTE_CONST Variant Variant::MakeInt(phi::i64 value)
{
    Variant variant;

    variant.m_Type = Type::Int64;
    variant.int64  = value;

    return variant;
}

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wsuggest-attribute=const")

PHI_ATTRIBUTE_PURE Variant Variant::MakeKeyword(OpenAutoIt::TokenKind value)
{
    PHI_ASSERT(value == TokenKind::KW_Default || value == TokenKind::KW_Null);

    Variant variant;

    variant.m_Type  = Type::Keyword;
    variant.keyword = value;

    return variant;
}

PHI_GCC_SUPPRESS_WARNING_POP()

PHI_ATTRIBUTE_CONST Variant Variant::MakePointer(ptr_t value)
{
    Variant variant;

    variant.m_Type  = Type::Pointer;
    variant.pointer = value;

    return variant;
}

Variant Variant::MakeString(const char* value)
{
    Variant variant;

    variant.m_Type = Type::String;
    variant.string = value;

    return variant;
}

Variant Variant::MakeString(phi::string_view value)
{
    Variant variant;

    // Reserve enough space for the string plus the null terminator
    variant.string.reserve(value.length().unsafe());

    variant.m_Type = Type::String;
    variant.string = string_t{value.data(), value.length().unsafe()};

    return variant;
}

Variant Variant::MakeString(const string_t& value)
{
    Variant variant;

    variant.m_Type = Type::String;
    variant.string = value;

    return variant;
}

Variant Variant::MakeString(string_t&& value)
{
    Variant variant;

    variant.m_Type = Type::String;
    variant.string = phi::move(value);

    return variant;
}

void Variant::copy_from(const Variant& other)
{
    m_Type = other.m_Type;

    switch (m_Type)
    {
        case Type::Array:
            new (&array) array_t(other.array);
            return;

        case Type::Binary:
            new (&binary) binary_t(other.binary);
            return;

        case Type::Boolean:
            boolean = other.boolean;
            return;

        case Type::Double:
            floating_point = other.floating_point;
            return;

        case Type::Function:
            new (&string) string_t(other.string);
            return;

        case Type::Int64:
            int64 = other.int64;
            return;

        case Type::Keyword:
            keyword = other.keyword;
            return;

        case Type::Pointer:
            pointer = other.pointer;
            return;

        case Type::String:
            new (&string) string_t(other.string);
            return;
    }

    PHI_ASSERT_NOT_REACHED();
}

void Variant::move_from(Variant&& other)
{
    m_Type = other.m_Type;

    switch (m_Type)
    {
        case Type::Array:
            new (&array) array_t(phi::move(other.array));
            return;

        case Type::Binary:
            new (&binary) binary_t(phi::move(other.binary));
            return;

        case Type::Boolean:
            boolean = other.boolean;
            return;

        case Type::Double:
            floating_point = other.floating_point;
            return;

        case Type::Function:
            new (&string) string_t(phi::move(other.string));
            return;

        case Type::Int64:
            int64 = other.int64;
            return;

        case Type::Keyword:
            keyword = other.keyword;
            return;

        case Type::Pointer:
            pointer = other.pointer;
            return;

        case Type::String:
            new (&string) string_t(phi::move(other.string));
            return;
    }

    PHI_ASSERT_NOT_REACHED();
}

// TODO: Documentation talks about "correcting" floating point errors when converting to int64
phi::i64 Variant::ConvertDoubleToInt64() const
{
    PHI_ASSERT(IsDouble());

    static constexpr const phi::f64 low_bound  = phi::narrow_cast<phi::f64>(phi::i64::min());
    static constexpr const phi::f64 high_bound = phi::unsafe_cast<phi::f64>(phi::i64::max());

    const phi::f64 double_value = phi::clamp(AsDouble(), low_bound, high_bound);

    return phi::unsafe_cast<phi::i64>(double_value);
}

} // namespace OpenAutoIt
