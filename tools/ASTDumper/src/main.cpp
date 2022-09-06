#include <OpenAutoIt/Lexer.hpp>
#include <OpenAutoIt/Parser.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/scope_guard.hpp>
#include <cstdio>
#include <filesystem>
#include <iostream>

PHI_CLANG_SUPPRESS_WARNING_PUSH()
PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")
PHI_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
OpenAutoIt::Lexer lexer;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
OpenAutoIt::Parser parser;

PHI_CLANG_SUPPRESS_WARNING_POP()

phi::boolean process_file(const std::filesystem::path& file_path) noexcept
{
    const std::string base_name = file_path.filename().replace_extension().string();

    const phi::optional<std::string> file_content_opt = OpenAutoIt::read_file(file_path);
    if (!file_content_opt)
    {
        std::cout << "Failed to read file " << file_path << "\n";
        return false;
    }
    const std::string& file_content = file_content_opt.value();

    // Lex the source file
    lexer.Reset();
    OpenAutoIt::TokenStream token_stream =
            lexer.ProcessString(phi::string_view{file_content.data(), file_content.length()});

    // Parse the source file
    phi::scope_ptr<OpenAutoIt::ASTDocument> document =
            parser.ParseDocument(phi::move(token_stream));

    // Generate AST Dump
    std::string ast_dump = document->DumpAST();

    // Generate AST Dump file
    std::filesystem::path ast_file_path = file_path;
    ast_file_path.replace_extension(".ast");

    OpenAutoIt::write_file(ast_file_path, ast_dump);

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
