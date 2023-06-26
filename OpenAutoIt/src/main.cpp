#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/DiagnosticConsumer.hpp"
#include "OpenAutoIt/DiagnosticEngine.hpp"
#include "OpenAutoIt/Interpreter.hpp"
#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/Parser.hpp"
#include "OpenAutoIt/SourceFile.hpp"
#include "OpenAutoIt/SourceManager.hpp"
#include "REPLInterpreter.hpp"
#include <phi/core/boolean.hpp>
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

    phi::string_view file_path = argv[1];

    RealFSSourceManager       source_manager;
    DefaultDiagnosticConsumer diagnostic_consumer;
    DiagnosticEngine          diagnostic_engine{&diagnostic_consumer};
    Lexer                     lexer{&diagnostic_engine};
    auto                      document = phi::make_not_null_scope<ASTDocument>();

    // Parse the source file
    OpenAutoIt::Parser parser{&source_manager, &diagnostic_engine, &lexer};
    parser.ParseFile(document, file_path);

    // Print info about diagnostics
    if (diagnostic_engine.GetNumberOfWarnings() > 0u)
    {
        const phi::boolean multiple_warnings = diagnostic_engine.GetNumberOfWarnings() > 1u;

        std::cout << diagnostic_engine.GetNumberOfWarnings().unsafe() << " warning"
                  << (multiple_warnings ? "s" : "") << " generated.\n";
        return 1;
    }

    if (diagnostic_engine.HasErrorOccurred())
    {
        const phi::boolean multiple_errors = diagnostic_engine.GetNumberOfError() > 1u;

        std::cout << diagnostic_engine.GetNumberOfError().unsafe() << " error"
                  << (multiple_errors ? "s" : "") << " generated.\n";
        return 1;
    }

    OpenAutoIt::Interpreter interpreter;
    interpreter.vm().SetupOutputHandler(standard_output_handler, error_output_handler);

    interpreter.SetDocument(document);
    interpreter.Run();

    const phi::u32 exit_code = interpreter.vm().GetExitCode();

    return exit_code.unsafe();
}
