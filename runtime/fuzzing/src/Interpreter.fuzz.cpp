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

static const constexpr phi::usize          MaxNumberOfStatements{1'000u};
static phi::usize                          NumberOfStatementsRan{0u};
phi::observer_ptr<OpenAutoIt::Interpreter> current_interpreter{nullptr};

// TODO: Remove once hack is fixed
void pre_action(phi::not_null_observer_ptr<OpenAutoIt::ASTStatement> /*statement*/)
{
    NumberOfStatementsRan += 1u;

    if (NumberOfStatementsRan >= MaxNumberOfStatements)
    {
        current_interpreter->m_VirtualMachine.m_GracefullyHalt = true;
    }
}

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    // Reset counter
    NumberOfStatementsRan = 0u;

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

    // TODO: Remove me once the hackisch way to limiting number of statements is removed
    current_interpreter = &interpreter;

    // Inject methods for pre actions
    interpreter.m_VirtualMachine.SetPreStatementAction(pre_action);

    // Prevent output
    interpreter.m_VirtualMachine.m_ConsoleWrite      = [](phi::string_view) {};
    interpreter.m_VirtualMachine.m_ConsoleWriteError = [](phi::string_view) {};

    interpreter.Run();

    return 0;
}
