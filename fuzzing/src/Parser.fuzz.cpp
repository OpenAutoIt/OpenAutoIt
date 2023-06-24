#include "OpenAutoIt/Parser.hpp"
#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/SourceManager.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/container/string_view.hpp>
#include <phi/core/scope_ptr.hpp>
#include <cstddef>
#include <cstdint>

using namespace OpenAutoIt;

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    disable_output();

    const phi::string_view source = phi::string_view(reinterpret_cast<const char*>(data), size);

    SourceManager source_manager;
    auto          document = phi::make_not_null_scope<ASTDocument>();

    Parser parser{source_manager};
    parser.ParseString(document, "Fuzz.au3", source);

    return 0;
}
