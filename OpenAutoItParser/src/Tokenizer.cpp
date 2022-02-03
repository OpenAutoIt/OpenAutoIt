#include "OpenAutoIt/Tokenizer.hpp"

#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <csignal>
#include <exception>

#define DEBUG_TOKENIZER() 0

#if DEBUG_TOKENIZER()
#    include <fmt/core.h>
#    include <magic_enum.hpp>
#    include <cstdio>
#    include <iostream>

#    define DEBUG_TOKENIZER_PRINT(...)                                                             \
        fmt::print("[Debug]: " __VA_ARGS__);                                                       \
        std::putc('\n', stdout);                                                                   \
        std::cout.flush()

#else
#    define DEBUG_TOKENIZER_PRINT(...) /* Nothing */
#endif

#define BEGIN_STATE(state_name)                                                                    \
    case State::state_name: {                                                                      \
        DEBUG_TOKENIZER_PRINT("Entering state: \"{:s}\"", magic_enum::enum_name(State::state_name));

#define END_STATE()                                                                                \
    PHI_ASSERT_NOT_REACHED();                                                                      \
    break;                                                                                         \
    }

#define SWITCH_TO(new_state)                                                                       \
    current_state = State::new_state;                                                              \
    DEBUG_TOKENIZER_PRINT("Switching to new state: \"{:s}\"",                                      \
                          magic_enum::enum_name(State::new_state));                                \
    continue

#define EMIT_TOKEN(type)                                                                           \
    DEBUG_TOKENIZER_PRINT("Emitting token type: \"{:s}\", text: \"{:s}\", {:d}:{:d}",              \
                          magic_enum::enum_name(type),                                             \
                          data.substr(token_begin, std::max((index - token_begin).get(), 1ul)),    \
                          line_number.get(), (column - (index - token_begin)).get());              \
    tokens.emplace_back(                                                                           \
            Token(type, data.substr(token_begin, std::max((index - token_begin).get(), 1ul)),      \
                  line_number, column - (index - token_begin)));

#define EMIT_NEW_LINE()                                                                            \
    EMIT_TOKEN(Token::Type::NewLine);                                                              \
    ++line_number;                                                                                 \
    column = 1u

#define EMIT_END_OF_FILE() EMIT_TOKEN(Token::Type::EndOfFile)

#define CONSUME_CURRENT_CHARACTER()                                                                \
    DEBUG_TOKENIZER_PRINT("Consumed character \'{:s}\'", pp_char(current_character));              \
    ++column;                                                                                      \
    ++data_ptr;                                                                                    \
    ++index

#define SKIP_CHARACTER()                                                                           \
    DEBUG_TOKENIZER_PRINT("Skipping character \'{:s}\'", pp_char(current_character));              \
    ++column;                                                                                      \
    ++data_ptr;                                                                                    \
    ++index;                                                                                       \
    continue

#define FINISH_TOKENIZING()                                                                        \
    DEBUG_TOKENIZER_PRINT("Finishing tokenization");                                               \
    finished = true;                                                                               \
    break

#define START_OF_NEW_TOKEN()                                                                       \
    token_begin = index;                                                                           \
    DEBUG_TOKENIZER_PRINT("Begin of token at {:d}, {:s}", token_begin.get(),                       \
                          pp_char(data.at(token_begin)))

#define CONTINUE_IN_SAME_STATE()                                                                   \
    DEBUG_TOKENIZER_PRINT("Continuing in same state: \"{:s}\"",                                    \
                          magic_enum::enum_name(current_state));                                   \
    continue

enum class State
{
    Normal,
    ParseIdentifier,
    ParseComment,
    ParseDoubleQuoteStringLiteral,
    ParseSingleQuoteStringLiteral,
    ParseNumericLiteral,
    ParseFloatLiteral,
    ParseOperator,
};

[[nodiscard]] constexpr phi::boolean is_space(char c) noexcept
{
    switch (c)
    {
        case ' ':
        case '\t':
        case '\v':
            return true;
        default:
            return false;
    }
}

[[nodiscard]] constexpr phi::boolean is_numeric(char c) noexcept
{
    switch (c)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true;
        default:
            return false;
    }
}

[[nodiscard]] constexpr phi::boolean is_operator(char c) noexcept
{
    switch (c)
    {
        // Mathematical operators
        case '+':
        case '-':
        case '/':
        case '*':
        case '^':
        // Concatenation
        case '&':
        // Comparision
        case '=':
        case '<':
        case '>':
        // Ternary operator
        case '?':
        case ':':
            return true;
        default:
            return false;
    }
}

[[nodiscard]] constexpr phi::boolean is_alpha(char c) noexcept
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

[[nodiscard]] constexpr phi::boolean is_alpha_numeric(char c) noexcept
{
    return is_numeric(c) || is_alpha(c);
}

[[nodiscard]] constexpr phi::boolean is_valid_identifier_char(char c) noexcept
{
    return is_alpha_numeric(c) || c == '_';
}

[[nodiscard]] std::string pp_char(char c) noexcept
{
    switch (c)
    {
        case '\0':
            return "\\0";
        case '\t':
            return "\\t";
        case '\n':
            return "\\n";
        default:
            return {c};
    }
}

namespace OpenAutoIt
{
    [[nodiscard]] std::vector<Token> Tokenize(phi::string_view data) noexcept
    {
        DEBUG_TOKENIZER_PRINT("--- Begin tokenizing \"{:s}\"", data);

        std::vector<Token> tokens;

        phi::u64 line_number = 1u;
        phi::u64 column      = 1u;
        phi::u64 token_begin = 0u;

        State       current_state = State::Normal;
        Token::Type current_token_type{Token::Type::Invalid};

        phi::boolean finished{false};
        const char*  data_ptr = data.begin();

        for (phi::u64 index{0u}; !finished;)
        {
            char current_character = *data_ptr;

            DEBUG_TOKENIZER_PRINT("Current character: \'{}\', 0x{:02X}", pp_char(current_character),
                                  current_character);

            switch (current_state)
            {
                BEGIN_STATE(Normal)
                {
                    if (current_character == '\0')
                    {
                        FINISH_TOKENIZING();
                    }
                    else if (current_character == '\n')
                    {
                        START_OF_NEW_TOKEN();
                        CONSUME_CURRENT_CHARACTER();
                        EMIT_NEW_LINE();
                        CONTINUE_IN_SAME_STATE();
                    }
                    else if (is_space(current_character))
                    {
                        SKIP_CHARACTER();
                    }
                    else if (current_character == '#')
                    {
                        current_token_type = Token::Type::PreProcessorIdentifier;
                        START_OF_NEW_TOKEN();
                        CONSUME_CURRENT_CHARACTER();
                        SWITCH_TO(ParseIdentifier);
                    }
                    else if (current_character == '$')
                    {
                        current_token_type = Token::Type::VariableIdentifier;
                        START_OF_NEW_TOKEN();
                        CONSUME_CURRENT_CHARACTER();
                        SWITCH_TO(ParseIdentifier);
                    }
                    else if (current_character == '@')
                    {
                        current_token_type = Token::Type::MacroIdentifier;
                        START_OF_NEW_TOKEN();
                        CONSUME_CURRENT_CHARACTER();
                        SWITCH_TO(ParseIdentifier);
                    }
                    else if (current_character == ';')
                    {
                        START_OF_NEW_TOKEN();
                        CONSUME_CURRENT_CHARACTER();
                        SWITCH_TO(ParseComment);
                    }
                    else if (current_character == '\'')
                    {
                        START_OF_NEW_TOKEN();
                        CONSUME_CURRENT_CHARACTER();
                        SWITCH_TO(ParseSingleQuoteStringLiteral);
                    }
                    else if (current_character == '"')
                    {
                        START_OF_NEW_TOKEN();
                        CONSUME_CURRENT_CHARACTER();
                        SWITCH_TO(ParseDoubleQuoteStringLiteral);
                    }
                    else if (is_numeric(current_character))
                    {
                        START_OF_NEW_TOKEN();
                        CONSUME_CURRENT_CHARACTER();
                        SWITCH_TO(ParseNumericLiteral);
                    }
                    else if (is_operator(current_character))
                    {
                        START_OF_NEW_TOKEN();
                        CONSUME_CURRENT_CHARACTER();
                        EMIT_TOKEN(Token::Type::Operator);
                        CONTINUE_IN_SAME_STATE();
                    }
                    else if (is_valid_identifier_char(current_character))
                    {
                        current_token_type = Token::Type::Identifier;
                        START_OF_NEW_TOKEN();
                        SWITCH_TO(ParseIdentifier);
                    }
                    else
                    {
                        // Must be an invalid character
                        DEBUG_TOKENIZER_PRINT("Warning invalid character");
                        SKIP_CHARACTER();
                    }
                }
                END_STATE()

                BEGIN_STATE(ParseIdentifier)
                {
                    if (current_character == '\0')
                    {
                        EMIT_TOKEN(current_token_type);
                        FINISH_TOKENIZING();
                    }
                    else if (is_valid_identifier_char(current_character))
                    {
                        CONSUME_CURRENT_CHARACTER();
                        CONTINUE_IN_SAME_STATE();
                    }
                    else
                    {
                        EMIT_TOKEN(current_token_type);
                        SWITCH_TO(Normal);
                    }
                }
                END_STATE()

                BEGIN_STATE(ParseComment)
                {
                    if (current_character == '\0')
                    {
                        EMIT_TOKEN(Token::Type::Comment);
                        FINISH_TOKENIZING();
                    }
                    else if (current_character == '\n')
                    {
                        EMIT_TOKEN(Token::Type::Comment);
                        SWITCH_TO(Normal);
                    }
                    else
                    {
                        CONSUME_CURRENT_CHARACTER();
                        CONTINUE_IN_SAME_STATE();
                    }
                }
                END_STATE()

                BEGIN_STATE(ParseSingleQuoteStringLiteral)
                {
                    if (current_character == '\0')
                    {
                        DEBUG_TOKENIZER_PRINT("Unterminated SingleQuoteStringLiteral!");
                        FINISH_TOKENIZING();
                    }
                    else if (current_character == '\'')
                    {
                        CONSUME_CURRENT_CHARACTER();
                        EMIT_TOKEN(Token::Type::StringLiteral);
                        SWITCH_TO(Normal);
                    }
                    else
                    {
                        CONSUME_CURRENT_CHARACTER();
                        CONTINUE_IN_SAME_STATE();
                    }
                }
                END_STATE()

                BEGIN_STATE(ParseDoubleQuoteStringLiteral)
                {
                    if (current_character == '\0')
                    {
                        DEBUG_TOKENIZER_PRINT("Unterminated DoubleQuoteStringLiteral!");
                        FINISH_TOKENIZING();
                    }
                    else if (current_character == '"')
                    {
                        CONSUME_CURRENT_CHARACTER();
                        EMIT_TOKEN(Token::Type::StringLiteral);
                        SWITCH_TO(Normal);
                    }
                    else
                    {
                        CONSUME_CURRENT_CHARACTER();
                        CONTINUE_IN_SAME_STATE();
                    }
                }
                END_STATE()

                BEGIN_STATE(ParseNumericLiteral)
                {
                    if (is_numeric(current_character))
                    {
                        CONSUME_CURRENT_CHARACTER();
                        CONTINUE_IN_SAME_STATE();
                    }
                    else
                    {
                        EMIT_TOKEN(Token::Type::IntegerLiteral);
                        SWITCH_TO(Normal);
                    }
                }
                END_STATE()

                default:
                    PHI_ASSERT_NOT_REACHED();
            }
        }

        DEBUG_TOKENIZER_PRINT("--- Finished tokenizing into {:d} tokens", tokens.size());

        return tokens;
    }
} // namespace OpenAutoIt
