#include "OpenAutoIt/ParseResult.hpp"
#include <OpenAutoIt/Lexer.hpp>
#include <phi/container/string_view.hpp>
#include <cstddef>
#include <cstdint>
#include <string>

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    phi::string_view source = phi::string_view(reinterpret_cast<const char*>(data), size);

    OpenAutoIt::ParseResult parse_result;
    OpenAutoIt::Lexer       lexer{parse_result, source};

    lexer.ProcessAll();

    return 0;
}
