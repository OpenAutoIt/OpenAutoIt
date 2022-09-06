#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/Parser.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/integer.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/scope_guard.hpp>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

PHI_CLANG_SUPPRESS_WARNING_PUSH()
PHI_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")
PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")

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

    // Read AST Dump file
    std::filesystem::path ast_file_path = file_path;
    ast_file_path.replace_extension(".ast");

    const phi::optional<std::string> ast_file_content_opt = OpenAutoIt::read_file(ast_file_path);
    if (!ast_file_content_opt)
    {
        std::cout << "The ast dump file \"" << ast_file_path.string() << "\" doesn't exist\n";
        return false;
    }
    const std::string& ast_file_content = ast_file_content_opt.value();

    // Compare AST Dump
    if (ast_dump != ast_file_content)
    {
        std::cout << "AST Dump mismatch \"" << file_path.string() << "\"\n";
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Missing required argument\n";
        return 2;
    }

    // Check directory actually exists
    if (!std::filesystem::exists(argv[1]))
    {
        std::cout << "The directory \"" << argv[1] << "\" seems to be missing!\n";
        return 3;
    }

    phi::boolean all_tests_passed{true};
    for (const auto& file : std::filesystem::recursive_directory_iterator{argv[1]})
    {
        // Skip non files
        if (!file.is_regular_file())
        {
            continue;
        }

        // Get file infos
        const auto& path      = file.path();
        const auto& extension = path.extension();

        // Skip non ".au3" files
        if (extension.string() != ".au3")
        {
            continue;
        }

        std::cout << file << '\n';
        phi::boolean result = process_file(path);
        all_tests_passed    = all_tests_passed && result;
    }

    if (!all_tests_passed)
    {
        std::cout << "Some tests failed!\n";
        return 1;
    }

    return 0;
}
