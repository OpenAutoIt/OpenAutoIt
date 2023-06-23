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
    Variant();

    Variant(const Variant& other);
    Variant(Variant&& other);

    ~Variant();

    Variant& operator=(const Variant& other);
    Variant& operator=(Variant&& other);

    void Clear();

    [[nodiscard]] Type             GetType() const;
    [[nodiscard]] phi::string_view GetTypeName() const;

    // General type tests
    [[nodiscard]] phi::boolean IsArray() const;
    [[nodiscard]] phi::boolean IsBinary() const;
    [[nodiscard]] phi::boolean IsBoolean() const;
    [[nodiscard]] phi::boolean IsDouble() const;
    [[nodiscard]] phi::boolean IsFunction() const;
    [[nodiscard]] phi::boolean IsInt64() const;
    [[nodiscard]] phi::boolean IsKeyword() const;
    [[nodiscard]] phi::boolean IsPointer() const;
    [[nodiscard]] phi::boolean IsString() const;

    // Specific tests
    [[nodiscard]] phi::boolean IsDefault() const;
    [[nodiscard]] phi::boolean IsNull() const;

    // Access to the underlying types
    [[nodiscard]] array_t&       AsArray();
    [[nodiscard]] const array_t& AsArray() const;

    [[nodiscard]] binary_t&       AsBinary();
    [[nodiscard]] const binary_t& AsBinary() const;

    [[nodiscard]] phi::boolean&       AsBoolean();
    [[nodiscard]] const phi::boolean& AsBoolean() const;

    [[nodiscard]] phi::f64&       AsDouble();
    [[nodiscard]] const phi::f64& AsDouble() const;

    [[nodiscard]] string_t&       AsFunction();
    [[nodiscard]] const string_t& AsFunction() const;

    [[nodiscard]] phi::i64&       AsInt64();
    [[nodiscard]] const phi::i64& AsInt64() const;

    [[nodiscard]] TokenKind&       AsKeyword();
    [[nodiscard]] const TokenKind& AsKeyword() const;

    [[nodiscard]] ptr_t&       AsPointer();
    [[nodiscard]] const ptr_t& AsPointer() const;

    [[nodiscard]] string_t&       AsString();
    [[nodiscard]] const string_t& AsString() const;

    // Casting
    // NOTE: You cannot cast to Array, Function or Keyword
    [[nodiscard]] Variant CastToBinary() const;
    [[nodiscard]] Variant CastToBoolean() const;
    [[nodiscard]] Variant CastToDouble() const;
    [[nodiscard]] Variant CastToInt64() const;
    [[nodiscard]] Variant CastToPointer() const;
    [[nodiscard]] Variant CastToString() const;

    // Operations
    [[nodiscard]] Variant Add(const Variant& other) const;
    [[nodiscard]] Variant Subtract(const Variant& other) const;
    [[nodiscard]] Variant Multiply(const Variant& other) const;
    [[nodiscard]] Variant Divide(const Variant& other) const;
    [[nodiscard]] Variant Concatenate(const Variant& other) const;

    [[nodiscard]] Variant UnaryMinus() const;
    [[nodiscard]] Variant UnaryNot() const;

    // Factory functions

    // Undefined
    [[nodiscard]] static Variant MakeUndefined();

    // Boolean
    [[nodiscard]] static Variant MakeBoolean(phi::boolean value);

    // Double
    [[nodiscard]] static Variant MakeDouble(phi::f64 value);

    // Int
    [[nodiscard]] static Variant MakeInt(phi::i64 value);

    // Keyword
    [[nodiscard]] static Variant MakeKeyword(TokenKind value);

    // Pointer
    [[nodiscard]] static Variant MakePointer(ptr_t value);

    // String
    [[nodiscard]] static Variant MakeString(const char* value);
    [[nodiscard]] static Variant MakeString(phi::string_view value);
    [[nodiscard]] static Variant MakeString(const string_t& value);
    [[nodiscard]] static Variant MakeString(string_t&& value);

private:
    void copy_from(const Variant& other);

    void move_from(Variant&& other);

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
