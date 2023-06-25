#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/DiagnosticEngine.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/container/string_view.hpp>

using namespace OpenAutoIt;

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const phi::uint8_t* data, phi::size_t size)
{
    disable_output();

    phi::string_view source = phi::string_view(reinterpret_cast<const char*>(data), size);

    DiagnosticEngine diagnostic_engine;

    Lexer lexer{&diagnostic_engine};

    (void)lexer.ProcessString("Fuzz.au3", source);

    return 0;
}
