#include <phi/test/test_macros.hpp>

#include <OpenAutoIt/AST/ASTBinaryExpression.hpp>
#include <OpenAutoIt/AST/ASTDocument.hpp>
#include <OpenAutoIt/AST/ASTIntegerLiteral.hpp>
#include <OpenAutoIt/AST/ASTVariableAssignment.hpp>
#include <OpenAutoIt/Lexer.hpp>
#include <OpenAutoIt/Parser.hpp>
#include <OpenAutoIt/TokenStream.hpp>
#include <OpenAutoIt/VariableScope.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>

PHI_CLANG_SUPPRESS_WARNING_PUSH()
PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")
PHI_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static OpenAutoIt::Lexer lexer;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static OpenAutoIt::Parser parser;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static OpenAutoIt::TokenStream res;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static phi::scope_ptr<OpenAutoIt::ASTDocument> doc;

PHI_CLANG_SUPPRESS_WARNING_POP()

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
