#include <catch2/catch_test_macros.hpp>

#include <OpenAutoIt/AST/ASTBinaryExpression.hpp>
#include <OpenAutoIt/AST/ASTDocument.hpp>
#include <OpenAutoIt/AST/ASTIntegerLiteral.hpp>
#include <OpenAutoIt/AST/ASTVariableDeclaration.hpp>
#include <OpenAutoIt/Lexer.hpp>
#include <OpenAutoIt/Parser.hpp>
#include <OpenAutoIt/TokenStream.hpp>
#include <OpenAutoIt/VariableScope.hpp>
#include <phi/core/assert.hpp>

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static OpenAutoIt::Lexer lexer;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static OpenAutoIt::Parser parser;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static OpenAutoIt::TokenStream res;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static phi::scope_ptr<OpenAutoIt::ASTDocument> doc;

#define TOKEN_MATCHES(token, kind, text, line_number, column)                                      \
    CHECK(token.GetTokenKind() == ::OpenAutoIt::TokenKind::kind);                                  \
    CHECK(token.GetText() == text);                                                                \
    CHECK(token.GetLineNumber().unsafe() == line_number);                                          \
    CHECK(token.GetColumn().unsafe() == column)

template <typename IsT, typename OriginT>
[[nodiscard]] bool is(const OriginT org) noexcept
{
    return dynamic_cast<IsT>(org) != nullptr;
}

template <typename IsT, typename OriginT>
[[nodiscard]] IsT as(const OriginT origin) noexcept
{
    IsT ret = dynamic_cast<IsT>(origin);
    PHI_ASSERT(ret != nullptr, "as expression failed");
    return ret;
}

TEST_CASE("Parser - Playground")
{
    res = lexer.ProcessString("$var = 1 + 2");
    REQUIRE(res.size().unsafe() == 5u);

    TOKEN_MATCHES(res.at(0u), VariableIdentifier, "$var", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), OP_Equals, "=", 1u, 6u);
    TOKEN_MATCHES(res.at(2u), IntegerLiteral, "1", 1u, 8u);
    TOKEN_MATCHES(res.at(3u), OP_Plus, "+", 1u, 10u);
    TOKEN_MATCHES(res.at(4u), IntegerLiteral, "2", 1u, 12u);

    doc = parser.ParseDocument(phi::move(res));

    REQUIRE(doc->m_Children.size() == 1u);

    auto* first    = doc->m_Children.at(0u).get();
    auto* var_decl = as<OpenAutoIt::ASTVariableDeclaration*>(first);

    CHECK_FALSE(var_decl->m_IsConst);
    CHECK_FALSE(var_decl->m_IsStatic);
    CHECK(var_decl->m_Scope == OpenAutoIt::VariableScope::Auto);
    CHECK(var_decl->m_VariableName == "var");

    auto* init_exp = var_decl->m_InitialValueExpression.get();
    auto* bin_exp  = as<OpenAutoIt::ASTBinaryExpression*>(init_exp);

    REQUIRE(bin_exp->m_LHS);
    auto* lhs_exp         = bin_exp->m_LHS.get();
    auto* lhs_int_literal = as<OpenAutoIt::ASTIntegerLiteral*>(lhs_exp);

    CHECK(lhs_int_literal->m_Value.unsafe() == 1);

    CHECK(bin_exp->m_Operator == OpenAutoIt::TokenKind::OP_Plus);

    REQUIRE(bin_exp->m_RHS);
    auto* rhs_exp         = bin_exp->m_RHS.get();
    auto* rhs_int_literal = as<OpenAutoIt::ASTIntegerLiteral*>(rhs_exp);

    CHECK(rhs_int_literal->m_Value.unsafe() == 2);

    std::cout << doc->DumpAST();
}
