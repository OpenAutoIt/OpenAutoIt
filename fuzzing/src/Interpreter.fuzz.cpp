#include "OpenAutoIt/ParseResult.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include <OpenAutoIt/AST/ASTDocument.hpp>
#include <OpenAutoIt/Interpreter.hpp>
#include <OpenAutoIt/Lexer.hpp>
#include <OpenAutoIt/Parser.hpp>
#include <phi/compiler_support/unused.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/scope_ptr.hpp>
#include <phi/core/types.hpp>
#include <cstddef>
#include <cstdint>

static const constexpr phi::usize MaxNumberOfStatements{1'000u};

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    phi::string_view source = phi::string_view(reinterpret_cast<const char*>(data), size);

    OpenAutoIt::ParseResult parse_result;

    // Lexing
    OpenAutoIt::Lexer lexer{parse_result, source};
    lexer.ProcessAll();

    // Parsing
    OpenAutoIt::Parser parser{};
    parser.ParseDocument(parse_result);

    // Ignore if document was null
    if (!parse_result.m_Document)
    {
        return 0;
    }

    // Interpreting
    OpenAutoIt::Interpreter interpreter{parse_result.m_Document.not_null_observer()};

    // Prevent output
    interpreter.vm().OverwriteIOSreams(nullptr, nullptr);

    // Limit number of executions because of the halting problem
    phi::u64 statements_ran = 0u;
    while (interpreter.vm().CanRun() && statements_ran < MaxNumberOfStatements)
    {
        interpreter.Step();
        ++statements_ran;
    }

    return 0;
}
