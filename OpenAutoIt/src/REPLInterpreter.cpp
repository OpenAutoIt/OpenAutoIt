#include "REPLInterpreter.hpp"

#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/Parser.hpp"
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

        OpenAutoIt::ParseResult parse_result;

        // Lex the input
        OpenAutoIt::Lexer lexer{parse_result};
        lexer.ProcessString(input);

        // Parse the source file
        OpenAutoIt::Parser parser;
        parser.ParseDocument(parse_result);

        m_Interpreter.SetDocument(parse_result.m_Document.not_null_observer());
        m_Interpreter.Run();

        // Print new line
        out("\n");
    }
} // namespace OpenAutoIt
