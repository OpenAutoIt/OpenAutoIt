#include "OpenAutoIt/Parser.hpp"
#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/DiagnosticEngine.hpp"
#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/SourceManager.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/scope_ptr.hpp>

using namespace OpenAutoIt;

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const phi::uint8_t* data, phi::size_t size)
{
    disable_output();

    const phi::string_view source = phi::string_view(reinterpret_cast<const char*>(data), size);

    SourceManager    source_manager;
    DiagnosticEngine diagnostic_engine;
    auto             document = phi::make_not_null_scope<ASTDocument>();

    Parser parser{source_manager, &diagnostic_engine};
    parser.ParseString(document, "Fuzz.au3", source);

    return 0;
}
