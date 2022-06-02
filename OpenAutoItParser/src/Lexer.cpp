#include "OpenAutoIt/Lexer.hpp"

#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include <phi/container/array.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/types.hpp>
#include <algorithm>
#include <array>
#include <map>
#include <string>

template <typename KeyT, typename ValueT, std::size_t SizeT>
class LookUpMap
{
public:
    constexpr LookUpMap(const std::array<std::pair<KeyT, ValueT>, SizeT>& data,
                        ValueT                                            default_value) noexcept
        : m_Data(data)
        , m_Default(default_value)
    {}

    [[nodiscard]] constexpr ValueT at(const KeyT& key) const
    {
        const auto itr = std::find_if(begin(m_Data), end(m_Data),
                                      [&key](const auto& v) { return v.first == key; });

        if (itr != end(m_Data))
        {
            return itr->second;
        }

        return m_Default;
    }

private:
    std::array<std::pair<KeyT, ValueT>, SizeT> m_Data;
    ValueT                                     m_Default;
};

static constexpr std::array<std::pair<phi::string_view, OpenAutoIt::TokenKind>, 102u> MacroValues{{
        {"@appdatacommondir", OpenAutoIt::TokenKind::MK_AppDataCommonDir},
        {"@appdatadir", OpenAutoIt::TokenKind::MK_AppDataDir},
        {"@autoitexe", OpenAutoIt::TokenKind::MK_AutoItExe},
        {"@autoitpid", OpenAutoIt::TokenKind::MK_AutoItPID},
        {"@autoitversion", OpenAutoIt::TokenKind::MK_AutoItVersion},
        {"@autoitx64", OpenAutoIt::TokenKind::MK_AutoItX64},
        {"@com_eventobj", OpenAutoIt::TokenKind::MK_COM_EventObj},
        {"@commonfilesdir", OpenAutoIt::TokenKind::MK_CommonFilesDir},
        {"@compiled", OpenAutoIt::TokenKind::MK_Compiled},
        {"@computername", OpenAutoIt::TokenKind::MK_ComputerName},
        {"@comspec", OpenAutoIt::TokenKind::MK_ComSpec},
        {"@cpuarch", OpenAutoIt::TokenKind::MK_CPUArch},
        {"@cr", OpenAutoIt::TokenKind::MK_CR},
        {"@crlf", OpenAutoIt::TokenKind::MK_CRLF},
        {"@desktopcommondir", OpenAutoIt::TokenKind::MK_DesktopCommonDir},
        {"@desktopdepth", OpenAutoIt::TokenKind::MK_DesktopDepth},
        {"@desktopdir", OpenAutoIt::TokenKind::MK_DesktopDir},
        {"@desktopheight", OpenAutoIt::TokenKind::MK_DesktopHeight},
        {"@desktoprefresh", OpenAutoIt::TokenKind::MK_DesktopRefresh},
        {"@desktopwidth", OpenAutoIt::TokenKind::MK_DesktopWidth},
        {"@documentscommondir", OpenAutoIt::TokenKind::MK_DocumentsCommonDir},
        {"@error", OpenAutoIt::TokenKind::MK_error},
        {"@exitcode", OpenAutoIt::TokenKind::MK_exitCode},
        {"@exitmethod", OpenAutoIt::TokenKind::MK_exitMethod},
        {"@extended", OpenAutoIt::TokenKind::MK_extended},
        {"@favoritescommondir", OpenAutoIt::TokenKind::MK_FavoritesCommonDir},
        {"@favoritesdir", OpenAutoIt::TokenKind::MK_FavoritesDir},
        {"@gui_ctrlhandle", OpenAutoIt::TokenKind::MK_GUI_CtrlHandle},
        {"@gui_ctrlid", OpenAutoIt::TokenKind::MK_GUI_CtrlId},
        {"@gui_dragfile", OpenAutoIt::TokenKind::MK_GUI_DragFile},
        {"@gui_dragid", OpenAutoIt::TokenKind::MK_GUI_DragId},
        {"@gui_dropid", OpenAutoIt::TokenKind::MK_GUI_DropId},
        {"@gui_winhandle", OpenAutoIt::TokenKind::MK_GUI_WinHandle},
        {"@homedrive", OpenAutoIt::TokenKind::MK_HomeDrive},
        {"@homepath", OpenAutoIt::TokenKind::MK_HomePath},
        {"@homeshare", OpenAutoIt::TokenKind::MK_HomeShare},
        {"@hotkeypressed", OpenAutoIt::TokenKind::MK_HotKeyPressed},
        {"@hour", OpenAutoIt::TokenKind::MK_HOUR},
        {"@ipaddress1", OpenAutoIt::TokenKind::MK_IPAddress1},
        {"@ipaddress2", OpenAutoIt::TokenKind::MK_IPAddress2},
        {"@ipaddress3", OpenAutoIt::TokenKind::MK_IPAddress3},
        {"@ipaddress4", OpenAutoIt::TokenKind::MK_IPAddress4},
        {"@kblayout", OpenAutoIt::TokenKind::MK_KBLayout},
        {"@lf", OpenAutoIt::TokenKind::MK_LF},
        {"@localappdatadir", OpenAutoIt::TokenKind::MK_LocalAppDataDir},
        {"@logondnsdomain", OpenAutoIt::TokenKind::MK_LogonDNSDomain},
        {"@logondomain", OpenAutoIt::TokenKind::MK_LogonDomain},
        {"@logonserver", OpenAutoIt::TokenKind::MK_LogonServer},
        {"@mday", OpenAutoIt::TokenKind::MK_MDAY},
        {"@min", OpenAutoIt::TokenKind::MK_MIN},
        {"@mon", OpenAutoIt::TokenKind::MK_MON},
        {"@msec", OpenAutoIt::TokenKind::MK_MSEC},
        {"@muilang", OpenAutoIt::TokenKind::MK_MUILang},
        {"@mydocumentsdir", OpenAutoIt::TokenKind::MK_MyDocumentsDir},
        {"@numparams", OpenAutoIt::TokenKind::MK_NumParams},
        {"@osarch", OpenAutoIt::TokenKind::MK_OSArch},
        {"@osbuild", OpenAutoIt::TokenKind::MK_OSBuild},
        {"@oslang", OpenAutoIt::TokenKind::MK_OSLang},
        {"@osservicepack", OpenAutoIt::TokenKind::MK_OSServicePack},
        {"@ostype", OpenAutoIt::TokenKind::MK_OSType},
        {"@osversion", OpenAutoIt::TokenKind::MK_OSVersion},
        {"@programfilesdir", OpenAutoIt::TokenKind::MK_ProgramFilesDir},
        {"@programscommondir", OpenAutoIt::TokenKind::MK_ProgramsCommonDir},
        {"@programsdir", OpenAutoIt::TokenKind::MK_ProgramsDir},
        {"@scriptdir", OpenAutoIt::TokenKind::MK_ScriptDir},
        {"@scriptfullpath", OpenAutoIt::TokenKind::MK_ScriptFullPath},
        {"@scriptlinenumber", OpenAutoIt::TokenKind::MK_ScriptLineNumber},
        {"@scriptname", OpenAutoIt::TokenKind::MK_ScriptName},
        {"@sec", OpenAutoIt::TokenKind::MK_SEC},
        {"@startmenucommondir", OpenAutoIt::TokenKind::MK_StartMenuCommonDir},
        {"@startmenudir", OpenAutoIt::TokenKind::MK_StartMenuDir},
        {"@startupcommondir", OpenAutoIt::TokenKind::MK_StartupCommonDir},
        {"@startupdir", OpenAutoIt::TokenKind::MK_StartupDir},
        {"@sw_disable", OpenAutoIt::TokenKind::MK_SW_DISABLE},
        {"@sw_enable", OpenAutoIt::TokenKind::MK_SW_ENABLE},
        {"@sw_hide", OpenAutoIt::TokenKind::MK_SW_HIDE},
        {"@sw_lock", OpenAutoIt::TokenKind::MK_SW_LOCK},
        {"@sw_maximize", OpenAutoIt::TokenKind::MK_SW_MAXIMIZE},
        {"@sw_minimize", OpenAutoIt::TokenKind::MK_SW_MINIMIZE},
        {"@sw_restore", OpenAutoIt::TokenKind::MK_SW_RESTORE},
        {"@sw_show", OpenAutoIt::TokenKind::MK_SW_SHOW},
        {"@sw_showdefault", OpenAutoIt::TokenKind::MK_SW_SHOWDEFAULT},
        {"@sw_showmaximized", OpenAutoIt::TokenKind::MK_SW_SHOWMAXIMIZED},
        {"@sw_showminimized", OpenAutoIt::TokenKind::MK_SW_SHOWMINIMIZED},
        {"@sw_showminnoactive", OpenAutoIt::TokenKind::MK_SW_SHOWMINNOACTIVE},
        {"@sw_showna", OpenAutoIt::TokenKind::MK_SW_SHOWNA},
        {"@sw_shownoactivate", OpenAutoIt::TokenKind::MK_SW_SHOWNOACTIVATE},
        {"@sw_shownormal", OpenAutoIt::TokenKind::MK_SW_SHOWNORMAL},
        {"@sw_unlock", OpenAutoIt::TokenKind::MK_SW_UNLOCK},
        {"@systemdir", OpenAutoIt::TokenKind::MK_SystemDir},
        {"@tab", OpenAutoIt::TokenKind::MK_TAB},
        {"@tempdir", OpenAutoIt::TokenKind::MK_TempDir},
        {"@tray_id", OpenAutoIt::TokenKind::MK_TRAY_ID},
        {"@trayiconflashing", OpenAutoIt::TokenKind::MK_TrayIconFlashing},
        {"@trayiconvisible", OpenAutoIt::TokenKind::MK_TrayIconVisible},
        {"@username", OpenAutoIt::TokenKind::MK_UserName},
        {"@userprofiledir", OpenAutoIt::TokenKind::MK_UserProfileDir},
        {"@wday", OpenAutoIt::TokenKind::MK_WDAY},
        {"@windowsdir", OpenAutoIt::TokenKind::MK_WindowsDir},
        {"@workingdir", OpenAutoIt::TokenKind::MK_WorkingDir},
        {"@yday", OpenAutoIt::TokenKind::MK_YDAY},
        {"@year", OpenAutoIt::TokenKind::MK_YEAR},
}};

[[nodiscard]] OpenAutoIt::TokenKind lookup_macro(phi::string_view token) noexcept
{
    static constexpr auto map =
            LookUpMap<phi::string_view, OpenAutoIt::TokenKind, MacroValues.size()>(
                    MacroValues, OpenAutoIt::TokenKind::NotAToken);

    std::string str{token.begin(), token.end()};
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return map.at({str});
}

static constexpr std::array<std::pair<phi::string_view, OpenAutoIt::TokenKind>, 10u>
        PreProcessorValues{{
                {"#comments-start", OpenAutoIt::TokenKind::PP_CommentsStart},
                {"#comments-end", OpenAutoIt::TokenKind::PP_CommentsEnd},
                {"#cs", OpenAutoIt::TokenKind::PP_CS},
                {"#ce", OpenAutoIt::TokenKind::PP_CE},
                {"#include", OpenAutoIt::TokenKind::PP_Include},
                {"#include-once", OpenAutoIt::TokenKind::PP_IncludeOnce},
                {"#notrayicon", OpenAutoIt::TokenKind::PP_NoTrayIcon},
                {"#onautoitstartregister", OpenAutoIt::TokenKind::PP_OnAutoItStartRegister},
                {"#pragma", OpenAutoIt::TokenKind::PP_Pragma},
                {"#requireadmin", OpenAutoIt::TokenKind::PP_RequireAdmin},
        }};

[[nodiscard]] OpenAutoIt::TokenKind lookup_pre_processor(phi::string_view token) noexcept
{
    static constexpr auto map =
            LookUpMap<phi::string_view, OpenAutoIt::TokenKind, PreProcessorValues.size()>(
                    PreProcessorValues, OpenAutoIt::TokenKind::NotAToken);

    std::string str{token.begin(), token.end()};
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return map.at({str});
}

static constexpr std::array<std::pair<phi::string_view, OpenAutoIt::TokenKind>, 43u> KeyWordsValues{
        {{"false", OpenAutoIt::TokenKind::KW_False},
         {"true", OpenAutoIt::TokenKind::KW_True},
         {"continuecase", OpenAutoIt::TokenKind::KW_ContinueCase},
         {"continueloop", OpenAutoIt::TokenKind::KW_ContinueLoop},
         {"default", OpenAutoIt::TokenKind::KW_Default},
         {"dim", OpenAutoIt::TokenKind::KW_Dim},
         {"local", OpenAutoIt::TokenKind::KW_Local},
         {"global", OpenAutoIt::TokenKind::KW_Global},
         {"const", OpenAutoIt::TokenKind::KW_Const},
         {"do", OpenAutoIt::TokenKind::KW_Do},
         {"until", OpenAutoIt::TokenKind::KW_Until},
         {"enum", OpenAutoIt::TokenKind::KW_Enum},
         {"exit", OpenAutoIt::TokenKind::KW_Exit},
         {"exitloop", OpenAutoIt::TokenKind::KW_ExitLoop},
         {"for", OpenAutoIt::TokenKind::KW_For},
         {"to", OpenAutoIt::TokenKind::KW_To},
         {"step", OpenAutoIt::TokenKind::KW_Step},
         {"next", OpenAutoIt::TokenKind::KW_Next},
         {"in", OpenAutoIt::TokenKind::KW_In},
         {"func", OpenAutoIt::TokenKind::KW_Func},
         {"return", OpenAutoIt::TokenKind::KW_Return},
         {"endfunc", OpenAutoIt::TokenKind::KW_EndFunc},
         {"if", OpenAutoIt::TokenKind::KW_If},
         {"then", OpenAutoIt::TokenKind::KW_Then},
         {"endif", OpenAutoIt::TokenKind::KW_EndIf},
         {"elseif", OpenAutoIt::TokenKind::KW_ElseIf},
         {"else", OpenAutoIt::TokenKind::KW_Else},
         {"null", OpenAutoIt::TokenKind::KW_Null},
         {"redim", OpenAutoIt::TokenKind::KW_ReDim},
         {"select", OpenAutoIt::TokenKind::KW_Select},
         {"case", OpenAutoIt::TokenKind::KW_Case},
         {"endselect", OpenAutoIt::TokenKind::KW_EndSelect},
         {"static", OpenAutoIt::TokenKind::KW_Static},
         {"switch", OpenAutoIt::TokenKind::KW_Switch},
         {"endswitch", OpenAutoIt::TokenKind::KW_EndSwitch},
         {"volatile", OpenAutoIt::TokenKind::KW_Volatile},
         {"while", OpenAutoIt::TokenKind::KW_While},
         {"wend", OpenAutoIt::TokenKind::KW_WEnd},
         {"with", OpenAutoIt::TokenKind::KW_With},
         {"endwith", OpenAutoIt::TokenKind::KW_EndWith},
         {"and", OpenAutoIt::TokenKind::KW_And},
         {"or", OpenAutoIt::TokenKind::KW_Or},
         {"not", OpenAutoIt::TokenKind::KW_Not}}};

[[nodiscard]] OpenAutoIt::TokenKind lookup_identifier(phi::string_view token) noexcept
{
    static constexpr auto map =
            LookUpMap<phi::string_view, OpenAutoIt::TokenKind, KeyWordsValues.size()>(
                    KeyWordsValues, OpenAutoIt::TokenKind::FunctionIdentifier);

    std::string str{token.begin(), token.end()};
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return map.at({str});
}

static constexpr std::array<std::pair<phi::string_view, OpenAutoIt::TokenKind>, 20u> OperatorValues{
        {{"=", OpenAutoIt::TokenKind::OP_Equals},
         {"+=", OpenAutoIt::TokenKind::OP_PlusEquals},
         {"-=", OpenAutoIt::TokenKind::OP_MinusEquals},
         {"*=", OpenAutoIt::TokenKind::OP_MultiplyEquals},
         {"/=", OpenAutoIt::TokenKind::OP_DivideEquals},
         {"&", OpenAutoIt::TokenKind::OP_Concatenate},
         {"&=", OpenAutoIt::TokenKind::OP_ConcatenateEquals},
         {"+", OpenAutoIt::TokenKind::OP_Plus},
         {"-", OpenAutoIt::TokenKind::OP_Minus},
         {"*", OpenAutoIt::TokenKind::OP_Multiply},
         {"/", OpenAutoIt::TokenKind::OP_Divide},
         {"^", OpenAutoIt::TokenKind::OP_Raise},
         {"==", OpenAutoIt::TokenKind::OP_EqualsEquals},
         {"<>", OpenAutoIt::TokenKind::OP_NotEqual},
         {">", OpenAutoIt::TokenKind::OP_GreaterThan},
         {">=", OpenAutoIt::TokenKind::OP_GreaterThanEqual},
         {"<", OpenAutoIt::TokenKind::OP_LessThan},
         {"<=", OpenAutoIt::TokenKind::OP_LessThanEqual},
         {"?", OpenAutoIt::TokenKind::OP_TernaryIf},
         {":", OpenAutoIt::TokenKind::OP_TernaryElse}}};

[[nodiscard]] OpenAutoIt::TokenKind lookup_operator(phi::string_view token) noexcept
{
    static constexpr auto map =
            LookUpMap<phi::string_view, OpenAutoIt::TokenKind, OperatorValues.size()>(
                    OperatorValues, OpenAutoIt::TokenKind::NotAToken);

    return map.at(token);
}

[[nodiscard]] constexpr phi::boolean is_skip_character(const char c) noexcept
{
    switch (c)
    {
        case ' ':
        case '\v':
        case '\t':
        case '\b':
        case '\f':
            return true;
        default:
            return false;
    }
}

[[nodiscard]] constexpr phi::boolean is_numeric(const char c) noexcept
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

[[nodiscard]] constexpr phi::boolean is_alpha(const char c) noexcept
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

[[nodiscard]] constexpr phi::boolean is_alpha_numeric(const char c) noexcept
{
    return is_numeric(c) || is_alpha(c);
}

[[nodiscard]] constexpr phi::boolean is_valid_identifier_char(const char c) noexcept
{
    return is_alpha_numeric(c) || c == '_';
}

[[nodiscard]] constexpr phi::boolean is_valid_pp_char(const char c) noexcept
{
    return is_alpha_numeric(c) || c == '-';
}

[[nodiscard]] constexpr phi::boolean is_two_part_operator(const char c) noexcept
{
    switch (c)
    {
        case '=':
        case '+':
        case '-':
        case '*':
        case '/':
        case '&':
        case '<':
        case '>':
            return true;

        default:
            return false;
    }
}

[[nodiscard]] constexpr phi::boolean is_single_operator(const char c) noexcept
{
    switch (c)
    {
        case '^':
        case '?':
        case ':':
            return true;

        default:
            return false;
    }
}

namespace OpenAutoIt
{
    Lexer::Lexer() noexcept
        : m_Iterator{m_Source.begin()}
    {}

    Lexer::Lexer(phi::string_view source) noexcept
        : m_Source{source}
        , m_Iterator{source.begin()}
    {}

    void Lexer::SetInputSource(phi::string_view source) noexcept
    {
        m_Source = source;

        Reset();
    }

    void Lexer::Reset() noexcept
    {
        m_Iterator = m_Source.begin();

        m_InsideMultiLineComment = false;

        m_LineNumber = 1u;
        m_Column     = 1u;
    }

    phi::boolean Lexer::IsFinished() const noexcept
    {
        return m_Iterator == m_Source.end();
    }

    phi::boolean Lexer::HasInput() const noexcept
    {
        return !m_Source.is_empty();
    }

    phi::optional<Token> Lexer::GetNextToken() noexcept
    {
        // TODO: #cs and #ce are not correctly parsed as comments

        while (!IsFinished())
        {
            char current_character = *m_Iterator;

            /* Embeded null character */

            if (current_character == '\0')
            {
                // TODO: Warn embeded null character
                SkipCurrentCharacter();
            }

            /* Multiline comments */

            else if (m_InsideMultiLineComment)
            {
                iterator begin_of_multiline_comment           = m_Iterator;
                phi::u64 begining_line_of_multiline_comment   = m_LineNumber;
                phi::u64 begining_column_of_multiline_comment = m_Column;

                while (!IsFinished())
                {
                    current_character = *m_Iterator;

                    // Check for end comment multiline
                    if (current_character == '#')
                    {
                        iterator begin_of_token = m_Iterator;
                        ConsumeCurrentCharacter();

                        while (!IsFinished())
                        {
                            current_character = *m_Iterator;

                            if (is_valid_pp_char(current_character))
                            {
                                ConsumeCurrentCharacter();
                                continue;
                            }

                            break;
                        }

                        TokenKind pre_processor_token_kind =
                                lookup_pre_processor(TokenText(begin_of_token));

                        if (pre_processor_token_kind == TokenKind::PP_CE ||
                            pre_processor_token_kind == TokenKind::PP_CommentsEnd)
                        {
                            m_InsideMultiLineComment = false;

                            // Go back the size of the parsed end token so we can reparse it in the normal pre processor parser
                            m_Iterator -= TokenText(begin_of_token).length().unsafe();

                            Token token{TokenKind::Comment, TokenText(begin_of_multiline_comment),
                                        begining_line_of_multiline_comment,
                                        begining_column_of_multiline_comment};

                            return token;
                        }
                    }
                    else if (current_character == '\n')
                    {
                        ConsumeCurrentCharacter();
                        AdvanceToNextLine();
                    }
                    else
                    {
                        // Otherwise simply consume the character
                        ConsumeCurrentCharacter();
                        m_Column += 1u;
                    }
                }
            }

            /* Skip characters */

            else if (is_skip_character(current_character))
            {
                SkipCurrentCharacter();
            }

            /* New Lines */

            else if (current_character == '\n')
            {
                Token new_line_token = ConstructToken(TokenKind::NewLine);

                ConsumeCurrentCharacter();
                AdvanceToNextLine();

                return new_line_token;
            }

            /* Comment */

            else if (current_character == ';')
            {
                iterator begin_of_token = m_Iterator;
                ConsumeCurrentCharacter();

                while (!IsFinished())
                {
                    current_character = *m_Iterator;

                    if (current_character != '\n')
                    {
                        ConsumeCurrentCharacter();
                        continue;
                    }

                    break;
                }

                return ConstructToken(TokenKind::Comment, begin_of_token);
            }

            /* Macros */

            else if (current_character == '@')
            {
                iterator begin_of_token = m_Iterator;
                ConsumeCurrentCharacter();

                while (!IsFinished())
                {
                    current_character = *m_Iterator;

                    if (is_valid_identifier_char(current_character))
                    {
                        ConsumeCurrentCharacter();
                        continue;
                    }

                    break;
                }

                // Emit token
                return ConstructToken(lookup_macro(TokenText(begin_of_token)), begin_of_token);
            }

            /* Variable identifier */

            else if (current_character == '$')
            {
                iterator begin_of_token = m_Iterator;
                ConsumeCurrentCharacter();

                while (!IsFinished())
                {
                    current_character = *m_Iterator;

                    if (is_valid_identifier_char(current_character))
                    {
                        ConsumeCurrentCharacter();
                        continue;
                    }

                    break;
                }

                // Emit Token
                return ConstructToken(TokenKind::VariableIdentifier, begin_of_token);
            }

            /* PreProcessor directive */

            else if (current_character == '#')
            {
                iterator begin_of_token = m_Iterator;
                ConsumeCurrentCharacter();

                while (!IsFinished())
                {
                    current_character = *m_Iterator;

                    if (is_valid_pp_char(current_character))
                    {
                        ConsumeCurrentCharacter();
                        continue;
                    }

                    break;
                }

                // Check for start of multiline comment
                TokenKind pre_processor_token_kind =
                        lookup_pre_processor(TokenText(begin_of_token));

                if (pre_processor_token_kind == TokenKind::PP_CS ||
                    pre_processor_token_kind == TokenKind::PP_CommentsStart)
                {
                    m_InsideMultiLineComment = true;
                }

                return ConstructToken(pre_processor_token_kind, begin_of_token);
            }

            /* SingleQuoteStringLiteral */

            else if (current_character == '\'')
            {
                iterator     begin_of_token = m_Iterator;
                phi::boolean did_terminate  = false;
                ConsumeCurrentCharacter();

                while (!IsFinished())
                {
                    current_character = *m_Iterator;
                    ConsumeCurrentCharacter();

                    if (current_character == '\'')
                    {
                        did_terminate = true;
                        break;
                    }
                }

                if (did_terminate)
                {
                    return ConstructToken(TokenKind::StringLiteral, begin_of_token);
                }

                // TODO: Warn unterminated string literal
            }

            /* DoubleQuoteStringLiteral */

            else if (current_character == '\"')
            {
                iterator     begin_of_token = m_Iterator;
                phi::boolean did_terminate  = false;
                ConsumeCurrentCharacter();

                while (!IsFinished())
                {
                    current_character = *m_Iterator;
                    ConsumeCurrentCharacter();

                    if (current_character == '\"')
                    {
                        did_terminate = true;
                        break;
                    }
                }

                if (did_terminate)
                {
                    return ConstructToken(TokenKind::StringLiteral, begin_of_token);
                }
            }

            /* Number Literals */

            else if (is_numeric(current_character))
            {
                // TODO: Support float literals
                // TODO: Suppport hex literals

                iterator begin_of_token = m_Iterator;
                ConsumeCurrentCharacter();

                while (!IsFinished())
                {
                    current_character = *m_Iterator;

                    if (is_numeric(current_character))
                    {
                        ConsumeCurrentCharacter();
                        continue;
                    }

                    break;
                }

                return ConstructToken(TokenKind::IntegerLiteral, begin_of_token);
            }

            /* Operators */

            else if (is_two_part_operator(current_character))
            {
                iterator begin_of_token = m_Iterator;
                ConsumeCurrentCharacter();

                if (!IsFinished())
                {
                    if (*m_Iterator == '=' || (*begin_of_token == '<' && *m_Iterator == '>'))
                    {
                        // We have an actual two part operator
                        ConsumeCurrentCharacter();
                    }
                }

                return ConstructToken(lookup_operator(TokenText(begin_of_token)), begin_of_token);
            }

            else if (is_single_operator(current_character))
            {
                Token token = ConstructToken(lookup_operator({m_Iterator}));

                ConsumeCurrentCharacter();

                return token;
            }

            /* Punctioation */

            else if (current_character == ',')
            {
                Token token = ConstructToken(TokenKind::Comma);

                ConsumeCurrentCharacter();

                return token;
            }

            else if (current_character == '(')
            {
                Token token = ConstructToken(TokenKind::LParen);

                ConsumeCurrentCharacter();

                return token;
            }

            else if (current_character == ')')
            {
                Token token = ConstructToken(TokenKind::RParen);

                ConsumeCurrentCharacter();

                return token;
            }

            else if (current_character == '.')
            {
                Token token = ConstructToken(TokenKind::Dot);

                ConsumeCurrentCharacter();

                return token;
            }

            else if (current_character == '[')
            {
                Token token = ConstructToken(TokenKind::LSquare);

                ConsumeCurrentCharacter();

                return token;
            }

            else if (current_character == ']')
            {
                Token token = ConstructToken(TokenKind::RSquare);

                ConsumeCurrentCharacter();

                return token;
            }

            /* Identifier */

            else if (is_valid_identifier_char(current_character))
            {
                iterator begin_of_token = m_Iterator;
                ConsumeCurrentCharacter();

                while (!IsFinished())
                {
                    current_character = *m_Iterator;

                    if (is_valid_identifier_char(current_character))
                    {
                        ConsumeCurrentCharacter();
                        continue;
                    }

                    break;
                }

                return ConstructToken(lookup_identifier(TokenText(begin_of_token)), begin_of_token);
            }

            /* Unknown/Unexpected character */

            else
            {
                // TODO: Warn unexpected character encountered
                SkipCurrentCharacter();
            }
        }

        return {};
    }

    TokenStream Lexer::ProcessAll() noexcept
    {
        TokenStream stream;

        while (!IsFinished())
        {
            phi::optional<Token> maybe_token = GetNextToken();

            if (maybe_token.has_value())
            {
                stream.emplace_back(maybe_token.value());
            }
        }

        stream.finalize();
        return stream;
    }

    TokenStream Lexer::ProcessString(phi::string_view source) noexcept
    {
        SetInputSource(source);
        return ProcessAll();
    }

    void Lexer::ConsumeCurrentCharacter() noexcept
    {
        ++m_Iterator;
    }

    void Lexer::AdvanceToNextLine() noexcept
    {
        ++m_LineNumber;
        m_Column = 1u;
    }

    void Lexer::SkipCurrentCharacter() noexcept
    {
        ConsumeCurrentCharacter();
        ++m_Column;
    }
} // namespace OpenAutoIt
