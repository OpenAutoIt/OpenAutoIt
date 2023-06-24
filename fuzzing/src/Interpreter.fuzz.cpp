#include "OpenAutoIt/Interpreter.hpp"
#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/Parser.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/compiler_support/unused.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/scope_ptr.hpp>
#include <phi/core/types.hpp>
#include <cstddef>
#include <cstdint>

using namespace OpenAutoIt;

static const constexpr phi::usize MaxNumberOfStatements{1'000u};

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    disable_output();

    phi::string_view source = phi::string_view(reinterpret_cast<const char*>(data), size);

    SourceManager source_manager;
    auto          document = phi::make_not_null_scope<ASTDocument>();

    // Parsing
    Parser parser{source_manager};
    parser.ParseString(document, "Fuzz.au3", source);

    // Interpreting
    Interpreter interpreter;
    interpreter.SetDocument(document);

    // Limit number of executions because of the halting problem
    phi::u64 statements_ran = 0u;
    while (interpreter.vm().CanRun() && statements_ran < MaxNumberOfStatements)
    {
        interpreter.Step();
        ++statements_ran;
    }

    return 0;
}
