#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/ParseResult.hpp"
#include <OpenAutoIt/Interpreter.hpp>
#include <OpenAutoIt/Lexer.hpp>
#include <OpenAutoIt/Parser.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        // TODO: Enter REPL mode
        return -1;
    }

    // TODO: Options etc.

    // TODO: Move to seperate function

    const char* file_path = argv[1];

    const phi::optional<std::string> file_content_opt = OpenAutoIt::read_file(file_path);
    if (!file_content_opt)
    {
        std::cout << "Failed to read file \"" << file_path << "\"\n";
        return -1;
    }
    const std::string& file_content = file_content_opt.value();

    OpenAutoIt::ParseResult parse_result;

    // Lex the source file
    OpenAutoIt::Lexer       lexer{parse_result};
    lexer.ProcessString(phi::string_view{file_content.data(), file_content.length()});

    // Parse the source file
    OpenAutoIt::Parser parser;
    parser.ParseDocument(parse_result);

    // TODO: Check if parsed successfully
    if (!parse_result.m_Document)
    {
        return 1;
    }

    OpenAutoIt::Interpreter interpreter{parse_result.m_Document.not_null_observer()};
    interpreter.Run();

    return 0;
}
