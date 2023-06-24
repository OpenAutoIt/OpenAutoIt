#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/container/string_view.hpp>
#include <cstddef>
#include <cstdint>
#include <string>

using namespace OpenAutoIt;

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    disable_output();

    phi::string_view source = phi::string_view(reinterpret_cast<const char*>(data), size);

    Lexer lexer;

    (void)lexer.ProcessString("Fuzz.au3", source);

    return 0;
}
