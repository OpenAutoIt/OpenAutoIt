#include "OpenAutoIt/AST/ASTDocument.hpp"
#include "OpenAutoIt/DiagnosticEngine.hpp"
#include "OpenAutoIt/Interpreter.hpp"
#include "OpenAutoIt/Lexer.hpp"
#include "OpenAutoIt/Parser.hpp"
#include "OpenAutoIt/SourceManager.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include "OpenAutoIt/Utililty.hpp"
#include <phi/compiler_support/unused.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/scope_ptr.hpp>
#include <phi/core/types.hpp>

using namespace OpenAutoIt;

static const constexpr phi::usize MaxNumberOfStatements{1'000u};
static const constexpr phi::usize MaxNumberOfVirtualFiles{26u};

phi::boolean ParseVirtualFiles(VirtualSourceManager& source_manager, const phi::string_view source)
{
    phi::boolean     parsing_filename{true};
    phi::string_view file_name;
    phi::usize       parse_starting = 0u;
    phi::usize       virtual_files  = 0u;

    for (phi::usize index = 0u; index < source.length(); ++index)
    {
        const char character = source[index];

        // Null character switches from filename -> content and vice versa
        if (character == '\0')
        {
            if (virtual_files >= MaxNumberOfVirtualFiles)
            {
                return false;
            }

            if (parsing_filename)
            {
                file_name = source.substring_view(parse_starting, index - parse_starting);
                if (file_name.is_empty())
                {
                    return false;
                }
            }
            else
            {
                // Parsed file name and content
                source_manager.LoadFileFromMemory(
                        file_name, source.substring_view(parse_starting, index - parse_starting));
                ++virtual_files;
            }

            parsing_filename = !parsing_filename;
            parse_starting   = index + 1u;
        }
    }

    return true;
}

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const phi::uint8_t* data, phi::size_t size)
{
    disable_output();

    phi::string_view source       = phi::string_view(reinterpret_cast<const char*>(data), size);
    phi::string_view files_source = source;

    for (phi::usize index = 0u; index < source.length(); ++index)
    {
        const char character = source[index];
        if (character == '\0')
        {
            source.remove_suffix(source.length() - index);
            files_source.remove_prefix(index + 1u);
            break;
        }
    }

    VirtualSourceManager source_manager;
    DiagnosticEngine     diagnostic_engine;
    Lexer                lexer{&diagnostic_engine};
    auto                 document = phi::make_not_null_scope<ASTDocument>();

    if (!ParseVirtualFiles(source_manager, files_source))
    {
        return 0;
    }

    // Parsing
    Parser parser{&source_manager, &diagnostic_engine, &lexer};
    parser.ParseString(document, "Fuzz.au3", source);

    // Interpreting
    Interpreter interpreter;
    interpreter.SetDocument(document);

    // Limit number of executions because of the halting problem
    phi::u64 statements_ran = 0u;
    while (interpreter.vm().CanRun() && statements_ran < MaxNumberOfStatements)
    {
        interpreter.Step();
        ++statements_ran;
    }

    return 0;
}
