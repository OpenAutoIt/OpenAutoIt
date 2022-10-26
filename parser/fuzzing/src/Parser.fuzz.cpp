#include "OpenAutoIt/ParseResult.hpp"
#include <OpenAutoIt/AST/ASTDocument.hpp>
#include <OpenAutoIt/Lexer.hpp>
#include <OpenAutoIt/Parser.hpp>
#include <phi/container/string_view.hpp>
#include <cstddef>
#include <cstdint>

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    const phi::string_view source = phi::string_view(reinterpret_cast<const char*>(data), size);

    OpenAutoIt::ParseResult parse_result;

    OpenAutoIt::Lexer lexer{parse_result, source};
    lexer.ProcessAll();

    OpenAutoIt::Parser parser;
    parser.ParseDocument(parse_result);

    const volatile phi::scope_ptr<OpenAutoIt::ASTDocument> doc{phi::move(parse_result.m_Document)};

    // Ignore result
    (void)doc;

    return 0;
}
