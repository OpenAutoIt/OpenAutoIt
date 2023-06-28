#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/DiagnosticConsumer.hpp"
#include "OpenAutoIt/DiagnosticEngine.hpp"
#include "OpenAutoIt/IncludeType.hpp"
#include "OpenAutoIt/Interpreter.hpp"
#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/Parser.hpp"
#include "OpenAutoIt/SourceLocation.hpp"
#include "OpenAutoIt/SourceManager.hpp"
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
#include <phi/core/scope_ptr.hpp>
#include <phi/core/types.hpp>

// No idea why we get this warning here from gcc
PHI_GCC_SUPPRESS_WARNING("-Walloc-zero")

#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <regex>
#include <string>

using namespace OpenAutoIt;

// TODO: Actually test with the saved diagnostics here
class TestRunnerDiagnosticConsumer final : public DiagnosticConsumer
{
public:
    void Report(const Diagnostic& diagnostic) override
    {
        if (!diagnostic.IsIgnored())
        {
            diagnostics.push_back(diagnostic);
        }
    }

    std::vector<Diagnostic> diagnostics;
};

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

ExpectedBlock extract_expected_block(const TokenStream& tokens)
{
    ExpectedBlock block;

    for (const Token& token : tokens)
    {
        if (token.GetTokenKind() != TokenKind::Comment)
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

    // Load file
    RealFSSourceManager                 source_manager;
    phi::observer_ptr<const SourceFile> source_file =
            source_manager.LoadFile(file_path.string(), IncludeType::Local);
    if (!source_file)
    {
        std::cerr << "Failed to load file: " << file_path << '\n';
        return false;
    }

    TestRunnerDiagnosticConsumer diagnostic_consumer;
    DiagnosticEngine             diagnostic_engine{&diagnostic_consumer};
    auto                         document = phi::make_not_null_scope<ASTDocument>();

    // Lex the source file
    OpenAutoIt::Lexer lexer{&diagnostic_engine};
    TokenStream       stream = lexer.ProcessFile(source_file.not_null());

    // Extract expected block
    const ExpectedBlock expected_block = extract_expected_block(stream);

    // Parse the source file
    Parser parser{&source_manager, &diagnostic_engine, &lexer};
    parser.ParseTokenStream(document, phi::move(stream), source_file.not_null());

    // Setup interpreter
    Interpreter interpreter;
    interpreter.SetDocument(document);

    // Clear buffers
    out_buffer.std_err.clear();
    out_buffer.std_out.clear();

    if (!diagnostic_engine.HasErrorOccurred())
    {
        // Setup VM
        VirtualMachine& vm = interpreter.vm();

        vm.SetupOutputHandler(standard_output_handler, error_output_handler);

        // Interpret the code
        interpreter.Run();
    }

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
