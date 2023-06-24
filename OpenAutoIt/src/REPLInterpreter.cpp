#include "REPLInterpreter.hpp"

#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/Parser.hpp"
#include "OpenAutoIt/SourceManager.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <string>

namespace OpenAutoIt
{

int REPLInterpreter::Run()
{
    out("Welcome to the OpenAutoIt REPL!\n");

    while (m_Running)
    {
        Loop();
    }

    return 0;
}

void REPLInterpreter::Loop()
{
    out("> ");

    std::string input;
    std::getline(std::cin, input);

    if (input.empty())
    {
        return;
    }

    // FIXME: This is a very hackish way to do this
    input = "ConsoleWrite(" + input + ")";

    SourceManager source_manager;
    auto          document = phi::make_not_null_scope<ASTDocument>();

    // Parse the source file
    Parser parser{source_manager};
    parser.ParseString(document, "<repl>", input);

    m_Interpreter.SetDocument(document);
    m_Interpreter.Run();

    // Print new line
    out("\n");
}

} // namespace OpenAutoIt
