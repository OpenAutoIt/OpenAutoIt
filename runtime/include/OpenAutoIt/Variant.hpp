#pragma once

#include "OpenAutoIt/TokenKind.hpp"
#include <phi/compiler_support/char8_t.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/types.hpp>
#include <string>
#include <vector>

namespace OpenAutoIt
{
class Variant;

using array_t  = std::vector<Variant>;
using binary_t = std::basic_string<char8_t>;
using ptr_t    = phi::uintptr_t;
using string_t = std::string;

// https://www.autoitscript.com/autoit3/docs/intro/lang_datatypes.htm
// https://www.autoitscript.com/autoit3/docs/intro/lang_variables.htm
class Variant
{
public:
    enum class Type
    {
        Array,
        Binary,
        Boolean,
        Double,
        Function,
        Int64,
        Keyword,
        Pointer,
        String,
        // TODO: DllStruct?
    };

    // Default constructor
    Variant() noexcept;

    Variant(const Variant& other) noexcept;
    Variant(Variant&& other) noexcept;

    ~Variant() noexcept;

    Variant& operator=(const Variant& other) noexcept;
    Variant& operator=(Variant&& other) noexcept;

    void Clear() noexcept;

    [[nodiscard]] Type             GetType() const noexcept;
    [[nodiscard]] phi::string_view GetTypeName() const noexcept;

    // General type tests
    [[nodiscard]] phi::boolean IsArray() const noexcept;
    [[nodiscard]] phi::boolean IsBinary() const noexcept;
    [[nodiscard]] phi::boolean IsBoolean() const noexcept;
    [[nodiscard]] phi::boolean IsDouble() const noexcept;
    [[nodiscard]] phi::boolean IsFunction() const noexcept;
    [[nodiscard]] phi::boolean IsInt64() const noexcept;
    [[nodiscard]] phi::boolean IsKeyword() const noexcept;
    [[nodiscard]] phi::boolean IsPointer() const noexcept;
    [[nodiscard]] phi::boolean IsString() const noexcept;

    // Specific tests
    [[nodiscard]] phi::boolean IsDefault() const noexcept;
    [[nodiscard]] phi::boolean IsNull() const noexcept;

    // Access to the underlying types
    [[nodiscard]] array_t&       AsArray() noexcept;
    [[nodiscard]] const array_t& AsArray() const noexcept;

    [[nodiscard]] binary_t&       AsBinary() noexcept;
    [[nodiscard]] const binary_t& AsBinary() const noexcept;

    [[nodiscard]] phi::boolean&       AsBoolean() noexcept;
    [[nodiscard]] const phi::boolean& AsBoolean() const noexcept;

    [[nodiscard]] phi::f64&       AsDouble() noexcept;
    [[nodiscard]] const phi::f64& AsDouble() const noexcept;

    [[nodiscard]] string_t&       AsFunction() noexcept;
    [[nodiscard]] const string_t& AsFunction() const noexcept;

    [[nodiscard]] phi::i64&       AsInt64() noexcept;
    [[nodiscard]] const phi::i64& AsInt64() const noexcept;

    [[nodiscard]] TokenKind&       AsKeyword() noexcept;
    [[nodiscard]] const TokenKind& AsKeyword() const noexcept;

    [[nodiscard]] ptr_t&       AsPointer() noexcept;
    [[nodiscard]] const ptr_t& AsPointer() const noexcept;

    [[nodiscard]] string_t&       AsString() noexcept;
    [[nodiscard]] const string_t& AsString() const noexcept;

    // Casting
    // NOTE: You cannot cast to Array, Function or Keyword
    [[nodiscard]] Variant CastToBinary() const noexcept;
    [[nodiscard]] Variant CastToBoolean() const noexcept;
    [[nodiscard]] Variant CastToDouble() const noexcept;
    [[nodiscard]] Variant CastToInt64() const noexcept;
    [[nodiscard]] Variant CastToPointer() const noexcept;
    [[nodiscard]] Variant CastToString() const noexcept;

    // Factory functions

    // Undefined
    [[nodiscard]] static Variant MakeUndefined() noexcept;

    // Boolean
    [[nodiscard]] static Variant MakeBoolean(phi::boolean value) noexcept;

    // Double
    [[nodiscard]] static Variant MakeDouble(phi::f64 value) noexcept;

    // Int
    [[nodiscard]] static Variant MakeInt(phi::i64 value) noexcept;

    // Keyword
    [[nodiscard]] static Variant MakeKeyword(TokenKind value) noexcept;

    // Pointer
    [[nodiscard]] static Variant MakePointer(ptr_t value) noexcept;

    // String
    [[nodiscard]] static Variant MakeString(const char* value) noexcept;
    [[nodiscard]] static Variant MakeString(phi::string_view value) noexcept;
    [[nodiscard]] static Variant MakeString(const string_t& value) noexcept;
    [[nodiscard]] static Variant MakeString(string_t&& value) noexcept;

private:
    void copy_from(const Variant& other) noexcept;

    void move_from(Variant&& other) noexcept;

    Type m_Type;

    union
    {
        array_t      array;
        binary_t     binary;
        phi::boolean boolean;
        phi::f64     floating_point;
        phi::i64     int64;
        TokenKind    keyword;
        ptr_t        pointer;
        string_t     string; // Can also hold a Function
    };
};
} // namespace OpenAutoIt
