#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/Interpreter.hpp"
#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/Parser.hpp"
#include "OpenAutoIt/SourceFile.hpp"
#include "OpenAutoIt/SourceManager.hpp"
#include "REPLInterpreter.hpp"
#include <phi/core/scope_ptr.hpp>
#include <iostream>

using namespace OpenAutoIt;

void standard_output_handler(const std::string& message)
{
    std::cout << message;
}

void error_output_handler(const std::string& message)
{
    std::cerr << message;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        OpenAutoIt::REPLInterpreter repl;
        return repl.Run();
    }

    // TODO: Options etc.

    // TODO: Move to separate function

    std::filesystem::path file_path = argv[1];

    SourceManager source_manager;
    auto          document = phi::make_not_null_scope<ASTDocument>();

    // Parse the source file
    OpenAutoIt::Parser parser{source_manager};
    parser.ParseFile(document, file_path);

    // TODO: Check if parsed successfully

    OpenAutoIt::Interpreter interpreter;
    interpreter.vm().SetupOutputHandler(standard_output_handler, error_output_handler);

    interpreter.SetDocument(document);
    interpreter.Run();

    const phi::u32 exit_code = interpreter.vm().GetExitCode();

    return exit_code.unsafe();
}
