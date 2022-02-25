#include <OpenAutoIt/Lexer.hpp>
#include <OpenAutoIt/ParseAST.hpp>
#include <phi/container/string_view.hpp>
#include <cstddef>
#include <cstdint>

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    // Ensure no zero size and that all strings are zero terminated
    if (size == 0 || data[size - 1] != '\0')
    {
        return 0;
    }

    phi::string_view source = phi::string_view(reinterpret_cast<const char*>(data), size);

    // Tokenize it
    /*
    auto res = OpenAutoIt::Tokenize(source);

    // Parse it
    auto ast = OpenAutoIt::ParseAST(res);
    */

    // Ignore result
    //(void)res;

    return 0;
}
