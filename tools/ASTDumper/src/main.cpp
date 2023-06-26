#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/DiagnosticConsumer.hpp"
#include "OpenAutoIt/DiagnosticEngine.hpp"
#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/Parser.hpp"
#include "OpenAutoIt/SourceManager.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/scope_guard.hpp>
#include <phi/core/scope_ptr.hpp>
#include <filesystem>
#include <iostream>

using namespace OpenAutoIt;

PHI_CLANG_SUPPRESS_WARNING_PUSH()
PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")
PHI_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
DefaultDiagnosticConsumer diagnostic_consumer;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
DiagnosticEngine diagnostic_engine{&diagnostic_consumer};
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
RealFSSourceManager source_manager;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
Lexer lexer{&diagnostic_engine};
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
Parser parser{&source_manager, &diagnostic_engine, &lexer};

PHI_CLANG_SUPPRESS_WARNING_POP()

phi::boolean process_file(const std::filesystem::path& file_path)
{
    const std::string base_name = file_path.filename().replace_extension().string();
    auto              document  = phi::make_not_null_scope<ASTDocument>();

    parser.ParseFile(document, phi::string_view(file_path.string()));

    if (diagnostic_engine.HasErrorOccurred())
    {
        std::cout << "Cannot dump file '" << file_path.string() << "' which contains errors!\n";
        return false;
    }

    // Generate AST Dump
    const std::string ast_dump = document->DumpAST();

    // Generate AST Dump file
    std::filesystem::path ast_file_path = file_path;
    ast_file_path.replace_extension(".ast");

    write_file(ast_file_path, ast_dump);

    std::cout << "AST Dumped to \"" << ast_file_path.string() << "\"\n";

    return true;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Missing arguments!\n";
        return 1;
    }

    for (int index{1}; index < argc; ++index)
    {
        process_file(argv[index]);
    }

    return 0;
}
