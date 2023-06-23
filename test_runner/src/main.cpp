#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/Interpreter.hpp"
#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/ParseError.hpp"
#include "OpenAutoIt/ParseResult.hpp"
#include "OpenAutoIt/ParseWarning.hpp"
#include "OpenAutoIt/Parser.hpp"
#include "OpenAutoIt/SourceLocation.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include "OpenAutoIt/VirtualMachine.hpp"
#include <phi/algorithm/equal.hpp>
#include <phi/algorithm/string_equals.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/integer.hpp>
#include <phi/core/narrow_cast.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/scope_guard.hpp>
#include <phi/core/types.hpp>

// No idea why we get this warning here from gcc
PHI_GCC_SUPPRESS_WARNING("-Walloc-zero")

#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

struct ExpectedBlock
{
    std::string std_out;
    std::string std_err;
};

struct OutputBuffer
{
    std::string std_out;
    std::string std_err;
};

PHI_CLANG_SUPPRESS_WARNING_PUSH()
PHI_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")
PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")

static OutputBuffer out_buffer;

// Regexes
static const std::regex expected_std_out_regex{R"regex(expect-stdout:\s*"(.*)")regex"};
static const std::regex expected_std_err_regex{R"regex(expect-stderr:\s*"(.*)")regex"};

PHI_CLANG_SUPPRESS_WARNING_POP()

void standard_output_handler(const std::string& message)
{
    out_buffer.std_out += message + '\0';
}

void error_output_handler(const std::string& message)
{
    out_buffer.std_err += message + '\0';
}

void basic_trim(std::string& str)
{
    while (!str.empty())
    {
        const char character = str.back();

        switch (character)
        {
            case '\0':
            case '\n':
            case '\t':
            case '\v':
            case '\b':
            case '\f':
            case ' ':
                str.pop_back();
                break;

            default:
                return;
        }
    }
}

PHI_ATTRIBUTE_PURE phi::boolean expects_matched(const ExpectedBlock& expected_block,
                                                const OutputBuffer&  buffer)
{
    phi::boolean return_value{true};

    // Check std-out
    std::string std_out_str = buffer.std_out;
    basic_trim(std_out_str);

    if (expected_block.std_out != std_out_str)
    {
        std::cout << "Expected stdout mismatch!\n";

        std::cout << "<Expected>\n" << expected_block.std_out << '\n';
        std::cout << "<Actual>\n" << std_out_str << '\n';

        return_value = false;
    }

    // Check std-err
    std::string std_err_str = buffer.std_err;
    basic_trim(std_err_str);

    if (expected_block.std_err != std_err_str)
    {
        std::cout << "Expected stderr mismatch!\n";

        std::cout << "<Expected>\n" << expected_block.std_err << '\n';
        std::cout << "<Actual>\n" << std_err_str << '\n';

        return_value = false;
    }

    return return_value;
}

ExpectedBlock extract_expected_block(const OpenAutoIt::TokenStream& tokens)
{
    ExpectedBlock block;

    for (const OpenAutoIt::Token& token : tokens)
    {
        if (token.GetTokenKind() != OpenAutoIt::TokenKind::Comment)
        {
            continue;
        }

        const phi::string_view                               text = token.GetText();
        std::match_results<phi::string_view::const_iterator> match;

        // stdout
        if (std::regex_search(text.begin(), text.end(), match, expected_std_out_regex))
        {
            // Ignore empty strings
            if (match[1].length() == 0u)
            {
                break;
            }

            block.std_out.reserve(block.std_out.length() +
                                  phi::narrow_cast<std::string::size_type>(match[1].length()) + 1u);

            block.std_out += match[1];
            block.std_out.push_back('\0');
        }

        // stderr
        if (std::regex_search(text.begin(), text.end(), match, expected_std_err_regex))
        {
            // Ignore empty strings
            if (match[1].length() == 0u)
            {
                break;
            }

            block.std_err.reserve(block.std_err.length() +
                                  phi::narrow_cast<std::string::size_type>(match[1].length()) + 1u);

            block.std_err += match[1];
            block.std_err.push_back('\0');
        }
    }

    // Trim output
    basic_trim(block.std_out);
    basic_trim(block.std_err);

    return block;
}

[[nodiscard]] phi::boolean process_file(const std::filesystem::path& file_path)
{
    const std::string base_name = file_path.filename().replace_extension().string();

    const phi::optional<std::string> file_content_opt = OpenAutoIt::read_file(file_path);
    if (!file_content_opt)
    {
        return false;
    }
    const std::string& file_content = file_content_opt.value();

    OpenAutoIt::ParseResult parse_result;

    // Lex the source file
    OpenAutoIt::Lexer lexer{parse_result};
    lexer.ProcessString(phi::string_view{file_content.data(), file_content.length()});

    // Parse the source file
    OpenAutoIt::Parser parser;
    parser.ParseDocument(parse_result);

    // Extract expected block
    const ExpectedBlock expected_block = extract_expected_block(parse_result.m_TokenStream);

    // Setup interpreter
    OpenAutoIt::Interpreter interpreter;
    interpreter.SetDocument(parse_result.m_Document.not_null_observer());

    // Clear buffers
    out_buffer.std_err.clear();
    out_buffer.std_out.clear();

    // Setup VM
    OpenAutoIt::VirtualMachine& vm = interpreter.vm();

    vm.SetupOutputHandler(standard_output_handler, error_output_handler);

    // Interpret the code
    interpreter.Run();

    // Check expected
    return expects_matched(expected_block, out_buffer);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Missing required argument\n";
        return 2;
    }

    // Check that the directory actually exists
    if (!std::filesystem::exists(argv[1]))
    {
        std::cout << "The directory \"" << argv[1] << "\" seems to be missing!\n";
        return 3;
    }

    phi::u64 number_of_tests{0u};
    phi::u64 number_of_test_failures{0u};
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

        std::cout << file.path().string() << " running...\n";

        const phi::boolean result = process_file(path);

        std::cout << file.path().string() << " ";

        number_of_tests += 1u;
        if (result)
        {
            std::cout << "\033[32m"
                      << "[PASS]"
                      << "\033[0m\n";
        }
        else
        {
            std::cout << "\033[31m"
                      << "[FAIL]"
                      << "\033[0m\n";
            number_of_test_failures += 1u;
        }
    }

    // Print some separation
    std::cout << '\n';

    if (number_of_test_failures != 0u)
    {
        std::cout << "\033[31m" << number_of_test_failures.unsafe() << "\033[0m/"
                  << number_of_tests.unsafe() << " tests failed!\n";
        return 1;
    }

    std::cout << "All " << number_of_tests.unsafe() << " tests passed!\n";

    return 0;
}
