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
#include <phi/compiler_support/warning.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/integer.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/scope_guard.hpp>
#include <phi/core/types.hpp>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

struct WarningInfo
{
    OpenAutoIt::ParseWarningType type;
    OpenAutoIt::SourceLocation   location;
};

struct ErrorInfo
{
    OpenAutoIt::ParseErrorType type;
    OpenAutoIt::SourceLocation location;
};

struct ExpectedBlock
{
    std::vector<std::string> std_out;
    std::vector<std::string> std_err;
    std::vector<WarningInfo> warnings;
};

struct GlobalBuffer
{
    std::vector<std::string> std_out;
    std::vector<std::string> std_err;

    void begin(const ExpectedBlock& expected) noexcept
    {
        std_out.clear();
        std_err.clear();

        // Reserve space matching the expected block
        std_out.reserve(expected.std_out.size());
        std_err.reserve(expected.std_err.size());
    }
};

PHI_CLANG_SUPPRESS_WARNING_PUSH()
PHI_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")
PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
OpenAutoIt::ParseResult parse_result;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
OpenAutoIt::Lexer lexer{parse_result};
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
OpenAutoIt::Parser parser;

// Buffer
GlobalBuffer buffer;

// Regexes
static const std::regex expected_std_out_regex{R"regex(expect-stdout:\s*"(.*)")regex"};
static const std::regex expected_std_err_regex{R"regex(expect-stderr:\s*"(.*)")regex"};

PHI_CLANG_SUPPRESS_WARNING_POP()

std::string null_terminated_string(phi::string_view text) noexcept
{
    if (text.is_empty())
    {
        return {};
    }

    if (text.back() == '\0')
    {
        return std::string(text);
    }

    std::string string;

    string.reserve(text.length().unsafe() + 1u);
    string = text;
    string.push_back('\0');

    return string;
}

phi::boolean vector_equals(const std::vector<std::string>& lhs,
                           const std::vector<std::string>& rhs) noexcept
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }

    return phi::equal(lhs.begin(), lhs.end(), rhs.begin());
}

// TODO: Print expected actual etc.
phi::boolean expects_matched(const ExpectedBlock& expected_block) noexcept
{
    phi::boolean return_value{true};

    // Check std-out
    if (!vector_equals(expected_block.std_out, buffer.std_out))
    {
        return_value = false;
    }

    // Check std-out
    if (!vector_equals(expected_block.std_err, buffer.std_err))
    {
        return_value = false;
    }

    return return_value;
}

ExpectedBlock extract_expected_block(const OpenAutoIt::TokenStream& tokens) noexcept
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
            block.std_out.emplace_back(null_terminated_string(match[1].str()));
            PHI_ASSERT(block.std_out.back().empty() || block.std_out.back().back() == '\0');
        }

        // stderr
        if (std::regex_search(text.begin(), text.end(), match, expected_std_err_regex))
        {
            block.std_err.emplace_back(null_terminated_string(match[1].str()));
            PHI_ASSERT(block.std_err.back().empty() || block.std_err.back().back() == '\0');
        }
    }

    return block;
}

phi::boolean process_file(const std::filesystem::path& file_path) noexcept
{
    const std::string base_name = file_path.filename().replace_extension().string();

    const phi::optional<std::string> file_content_opt = OpenAutoIt::read_file(file_path);
    if (!file_content_opt)
    {
        return false;
    }
    const std::string& file_content = file_content_opt.value();

    parse_result.Clear();

    // Lex the source file
    lexer.Reset();
    lexer.ProcessString(phi::string_view{file_content.data(), file_content.length()});

    // Parse the source file
    parser.ParseDocument(parse_result);

    // Extract expected block
    const ExpectedBlock expected_block = extract_expected_block(parse_result.m_TokenStream);

    // Setup interpret
    OpenAutoIt::Interpreter interpreter{parse_result.m_Document.not_null_observer()};

    // Setup VM
    buffer.begin(expected_block);

    // TODO: Use getter function
    OpenAutoIt::VirtualMachine& vm = interpreter.m_VirtualMachine;

    vm.m_ConsoleWrite = [](phi::string_view text) noexcept { buffer.std_out.emplace_back(text); };
    vm.m_ConsoleWriteError = [](phi::string_view text) noexcept {
        buffer.std_err.emplace_back(text);
    };

    // Interpret the code
    interpreter.Run();

    // Check expected
    return expects_matched(expected_block);
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

        std::cout << "[Check] \"" << file.path().string() << "\"...\n";

        const phi::boolean result = process_file(path);

        number_of_tests += 1u;
        if (result)
        {
            std::cout << "[Pass]  \"" << file.path().string() << "\"\n";
        }
        else
        {
            std::cout << "[Fail]  \"" << file.path().string() << "\"\n";
            number_of_test_failures += 1u;
        }
    }

    // Print some separation
    std::cout << '\n';

    if (number_of_test_failures != 0u)
    {
        std::cout << number_of_test_failures.unsafe() << '/' << number_of_tests.unsafe()
                  << " tests failed!\n";
        return 1;
    }

    std::cout << "All " << number_of_tests.unsafe() << " tests passed!\n";

    return 0;
}
