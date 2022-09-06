#include <OpenAutoIt/Lexer.hpp>
#include <phi/container/string_view.hpp>
#include <cstddef>
#include <cstdint>
#include <string>

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    phi::string_view source = phi::string_view(reinterpret_cast<const char*>(data), size);

    OpenAutoIt::Lexer lexer{source};

    volatile OpenAutoIt::TokenStream res = lexer.ProcessAll();

    // Ignore result
    (void)res;

    return 0;
}
