#include <phi/test/test_macros.hpp>

#include <OpenAutoIt/Utililty.hpp>
#include <phi/container/string_view.hpp>

TEST_CASE("Utility.string_equals_ignore_case")
{
    REQUIRE(OpenAutoIt::string_equals_ignore_case("", ""));
    REQUIRE_FALSE(OpenAutoIt::string_equals_ignore_case("", "1"));
    REQUIRE_FALSE(OpenAutoIt::string_equals_ignore_case("1", ""));

    REQUIRE(OpenAutoIt::string_equals_ignore_case("1", "1"));
    REQUIRE(OpenAutoIt::string_equals_ignore_case("a", "a"));
    REQUIRE(OpenAutoIt::string_equals_ignore_case("a", "A"));
    REQUIRE(OpenAutoIt::string_equals_ignore_case("A", "a"));
    REQUIRE(OpenAutoIt::string_equals_ignore_case("A", "A"));

    REQUIRE(OpenAutoIt::string_equals_ignore_case("Hello World", "hello world"));
    REQUIRE(OpenAutoIt::string_equals_ignore_case("Hello World", "hEllO wOrLD"));
    REQUIRE(OpenAutoIt::string_equals_ignore_case("Hello World", "HELLO WORLD"));
    REQUIRE_FALSE(OpenAutoIt::string_equals_ignore_case("hello world", "hillo world"));
}