#include <OpenAutoIt/Lexer.hpp>
#include <OpenAutoIt/Parser.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/scope_guard.hpp>
#include <cstdio>
#include <filesystem>
#include <iostream>

OpenAutoIt::Lexer  lexer;
OpenAutoIt::Parser parser;

phi::optional<std::string> read_file(const std::filesystem::path& file_path) noexcept
{
    // Check that the file actually exists
    if (!std::filesystem::exists(file_path))
    {
        return {};
    }
    std::FILE* file        = std::fopen(file_path.c_str(), "r");
    auto       scope_guard = phi::make_scope_guard([&file]() { (void)std::fclose(file); });

    if (file != nullptr)
    {
        (void)std::fseek(file, 0, SEEK_END);

        const std::size_t size = std::ftell(file);
        if (size != -1)
        {
            std::rewind(file);

            // Create empty string of desired size
            std::string str(size, '\0');

            // Read data
            (void)std::fread(str.data(), sizeof(std::string::value_type), size, file);

            return phi::move(str);
        }
    }

    return {};
}

phi::boolean write_file(const std::filesystem::path& file_path, std::string_view data) noexcept
{
    std::FILE* file        = std::fopen(file_path.c_str(), "w");
    auto       scope_guard = phi::make_scope_guard([&file]() { (void)std::fclose(file); });

    if (file != nullptr)
    {
        (void)std::fwrite(data.data(), sizeof(data.front()), data.size(), file);
        return true;
    }

    return false;
}

phi::boolean process_file(const std::filesystem::path& file_path) noexcept
{
    const std::string base_name = file_path.filename().replace_extension().string();

    const phi::optional<std::string> file_content_opt = read_file(file_path);
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
