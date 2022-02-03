#include <catch2/catch_test_macros.hpp>

#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/Tokenizer.hpp"

#define TOKEN_MATCHES(token, type, text, line_number, column)                                      \
    CHECK(token.GetType() == ::OpenAutoIt::Token::Type::type);                                     \
    CHECK(token.GetText() == text);                                                                \
    CHECK(token.GetLineNumber().get() == line_number);                                             \
    CHECK(token.GetColumn().get() == column)

std::vector<OpenAutoIt::Token> res;

TEST_CASE("Tokenizer - Empty")
{
    res = OpenAutoIt::Tokenize("");
    REQUIRE(res.empty());

    res = OpenAutoIt::Tokenize("\0");
    REQUIRE(res.empty());

    res = OpenAutoIt::Tokenize(" ");
    REQUIRE(res.empty());

    res = OpenAutoIt::Tokenize(" \0");
    REQUIRE(res.empty());

    res = OpenAutoIt::Tokenize("  ");
    REQUIRE(res.empty());

    res = OpenAutoIt::Tokenize("          ");
    REQUIRE(res.empty());
}

TEST_CASE("Tokenizer - New lines")
{
    res = OpenAutoIt::Tokenize("\n");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);

    res = OpenAutoIt::Tokenize(" \n");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);

    res = OpenAutoIt::Tokenize("  \n");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 3u);

    res = OpenAutoIt::Tokenize(" \n  ");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);

    res = OpenAutoIt::Tokenize("\n\n");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 1u);

    res = OpenAutoIt::Tokenize(" \n\n");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 1u);

    res = OpenAutoIt::Tokenize(" \n \n");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 2u);

    res = OpenAutoIt::Tokenize(" \n \n   ");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 2u);

    res = OpenAutoIt::Tokenize("\n\n\n");
    REQUIRE(res.size() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 3u, 1u);
}

TEST_CASE("Tokenizer - Comments")
{
    res = OpenAutoIt::Tokenize("; Comment");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), Comment, "; Comment", 1u, 1u);

    res = OpenAutoIt::Tokenize(";");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), Comment, ";", 1u, 1u);

    res = OpenAutoIt::Tokenize(";Blub");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), Comment, ";Blub", 1u, 1u);

    res = OpenAutoIt::Tokenize("\n; What a comment");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), Comment, "; What a comment", 2u, 1u);

    res = OpenAutoIt::Tokenize("\n; What a comment\n");
    REQUIRE(res.size() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), Comment, "; What a comment", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 17u);

    res = OpenAutoIt::Tokenize("; What a comment\n");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), Comment, "; What a comment", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 17u);
}

TEST_CASE("Tokenizer - Identifier")
{
    res = OpenAutoIt::Tokenize("ValidIdentifier");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), Identifier, "ValidIdentifier", 1u, 1u);

    res = OpenAutoIt::Tokenize(" _123");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), Identifier, "_123", 1u, 2u);

    res = OpenAutoIt::Tokenize("a");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), Identifier, "a", 1u, 1u);

    res = OpenAutoIt::Tokenize("A");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), Identifier, "A", 1u, 1u);

    res = OpenAutoIt::Tokenize("a ");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), Identifier, "a", 1u, 1u);

    res = OpenAutoIt::Tokenize("A_\nB");
    REQUIRE(res.size() == 3u);

    TOKEN_MATCHES(res.at(0u), Identifier, "A_", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);
    TOKEN_MATCHES(res.at(2u), Identifier, "B", 2u, 1u);

    res = OpenAutoIt::Tokenize("\na\n");
    REQUIRE(res.size() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), Identifier, "a", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 2u);

    res = OpenAutoIt::Tokenize("A_\n B\n_123");
    REQUIRE(res.size() == 5u);

    TOKEN_MATCHES(res.at(0u), Identifier, "A_", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);
    TOKEN_MATCHES(res.at(2u), Identifier, "B", 2u, 2u);
    TOKEN_MATCHES(res.at(3u), NewLine, "\n", 2u, 3u);
    TOKEN_MATCHES(res.at(4u), Identifier, "_123", 3u, 1u);
}

TEST_CASE("Tokenizer - PreProcessorIdentifier")
{
    res = OpenAutoIt::Tokenize("#a");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), PreProcessorIdentifier, "#a", 1u, 1u);

    res = OpenAutoIt::Tokenize("#Blub");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), PreProcessorIdentifier, "#Blub", 1u, 1u);

    res = OpenAutoIt::Tokenize("  #a");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), PreProcessorIdentifier, "#a", 1u, 3u);

    res = OpenAutoIt::Tokenize("#a\n#b");
    REQUIRE(res.size() == 3u);

    TOKEN_MATCHES(res.at(0u), PreProcessorIdentifier, "#a", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);
    TOKEN_MATCHES(res.at(2u), PreProcessorIdentifier, "#b", 2u, 1u);

    res = OpenAutoIt::Tokenize("\n#a\n");
    REQUIRE(res.size() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), PreProcessorIdentifier, "#a", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 3u);

    res = OpenAutoIt::Tokenize("#a\n #b \n#c");
    REQUIRE(res.size() == 5u);

    TOKEN_MATCHES(res.at(0u), PreProcessorIdentifier, "#a", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);
    TOKEN_MATCHES(res.at(2u), PreProcessorIdentifier, "#b", 2u, 2u);
    TOKEN_MATCHES(res.at(3u), NewLine, "\n", 2u, 5u);
    TOKEN_MATCHES(res.at(4u), PreProcessorIdentifier, "#c", 3u, 1u);
}

TEST_CASE("Tokenizer - VariableIdentifier")
{
    res = OpenAutoIt::Tokenize("$a");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), VariableIdentifier, "$a", 1u, 1u);

    res = OpenAutoIt::Tokenize("$VeryLongVariableNameHere");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), VariableIdentifier, "$VeryLongVariableNameHere", 1u, 1u);

    res = OpenAutoIt::Tokenize("   $a");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), VariableIdentifier, "$a", 1u, 4u);

    res = OpenAutoIt::Tokenize("$a\n");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), VariableIdentifier, "$a", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);

    res = OpenAutoIt::Tokenize("\n$a");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), VariableIdentifier, "$a", 2u, 1u);

    res = OpenAutoIt::Tokenize("\n$a\n");
    REQUIRE(res.size() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), VariableIdentifier, "$a", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 3u);
}

TEST_CASE("Tokenizer - MacroIdentifier")
{
    res = OpenAutoIt::Tokenize("@a");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), MacroIdentifier, "@a", 1u, 1u);

    res = OpenAutoIt::Tokenize("@VeryLongMacroNameHere");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), MacroIdentifier, "@VeryLongMacroNameHere", 1u, 1u);

    res = OpenAutoIt::Tokenize("   @a");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), MacroIdentifier, "@a", 1u, 4u);

    res = OpenAutoIt::Tokenize("@a\n");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), MacroIdentifier, "@a", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);

    res = OpenAutoIt::Tokenize("\n@a");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), MacroIdentifier, "@a", 2u, 1u);

    res = OpenAutoIt::Tokenize("\n@a\n");
    REQUIRE(res.size() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), MacroIdentifier, "@a", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 3u);
}

TEST_CASE("Tokenizer - StringLiteral")
{
    res = OpenAutoIt::Tokenize(R"("")");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"("")", 1u, 1u);

    res = OpenAutoIt::Tokenize(R"('')");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"('')", 1u, 1u);

    res = OpenAutoIt::Tokenize(R"("a")");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"("a")", 1u, 1u);

    res = OpenAutoIt::Tokenize(R"('a')");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"('a')", 1u, 1u);

    res = OpenAutoIt::Tokenize(R"("'")");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"("'")", 1u, 1u);

    res = OpenAutoIt::Tokenize(R"('"')");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"('"')", 1u, 1u);

    res = OpenAutoIt::Tokenize(R"("""")");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"("")", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), StringLiteral, R"("")", 1u, 3u);

    res = OpenAutoIt::Tokenize(R"('''')");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"('')", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), StringLiteral, R"('')", 1u, 3u);
}

TEST_CASE("Tokenizer - IntegerLiteral")
{
    res = OpenAutoIt::Tokenize("1");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "1", 1u, 1u);

    res = OpenAutoIt::Tokenize("12");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "12", 1u, 1u);

    res = OpenAutoIt::Tokenize("12345678900123456789");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "12345678900123456789", 1u, 1u);

    res = OpenAutoIt::Tokenize("\n1");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), IntegerLiteral, "1", 2u, 1u);

    res = OpenAutoIt::Tokenize("1\n");
    REQUIRE(res.size() == 2u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "1", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 2u);

    res = OpenAutoIt::Tokenize("\n1\n");
    REQUIRE(res.size() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), IntegerLiteral, "1", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 2u);
}

TEST_CASE("Tokenizer - Operator")
{
    res = OpenAutoIt::Tokenize("+");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), Operator, "+", 1u, 1u);

    res = OpenAutoIt::Tokenize("-");
    REQUIRE(res.size() == 1u);

    TOKEN_MATCHES(res.at(0u), Operator, "-", 1u, 1u);
}

TEST_CASE("Tokenizer - Invalid")
{
    res = OpenAutoIt::Tokenize(R"(")");
    REQUIRE(res.empty());

    res = OpenAutoIt::Tokenize(R"(')");
    REQUIRE(res.empty());
}
