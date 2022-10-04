#include <phi/test/test_macros.hpp>

#include <OpenAutoIt/Lexer.hpp>
#include <OpenAutoIt/Token.hpp>
#include <OpenAutoIt/TokenKind.hpp>
#include <OpenAutoIt/TokenStream.hpp>
#include <phi/compiler_support/unused.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/types.hpp>
#include <phi/text/to_lower_case.hpp>
#include <phi/text/to_upper_case.hpp>
#include <cmath>

#define TOKEN_MATCHES(token, kind, text, line_number, column)                                      \
    CHECK(token.GetTokenKind() == ::OpenAutoIt::TokenKind::kind);                                  \
    CHECK(token.GetText() == text);                                                                \
    CHECK(token.GetLineNumber().unsafe() == line_number);                                          \
    CHECK(token.GetColumn().unsafe() == column)

TEST_CASE("Lexer - default constructor")
{
    OpenAutoIt::Lexer lexer;

    CHECK(lexer.IsFinished());
    CHECK_FALSE(lexer.HasInput());
}

TEST_CASE("Lexer - construct with source")
{
    OpenAutoIt::Lexer lexer{"Hello World"};

    CHECK_FALSE(lexer.IsFinished());
    CHECK(lexer.HasInput());
}

PHI_CLANG_SUPPRESS_WARNING_PUSH()
PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")
PHI_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables))
static OpenAutoIt::Lexer lexer;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables))
static OpenAutoIt::TokenStream res;

PHI_CLANG_SUPPRESS_WARNING_POP()

void test_all_spellings_of(phi::string_view text, OpenAutoIt::TokenKind expected_kind)
{
    // Don't run these tests for debug builds since there VERY slow
#if defined(PHI_DEBUG)
    PHI_UNUSED_PARAMETER(text);
    PHI_UNUSED_PARAMETER(expected_kind);
#else
    std::string str;
    std::size_t max_index = static_cast<std::size_t>(std::pow(2u, text.length().unsafe()));

    for (phi::usize index{0u}; index < max_index; ++index)
    {
        str.clear();
        str.reserve(text.length().unsafe());

        // construct string
        for (phi::usize str_index{0u}; str_index < text.length(); ++str_index)
        {
            if ((index.unsafe() >> str_index.unsafe()) & 0x1)
            {
                str.push_back(phi::to_upper_case(text.at(str_index)));
            }
            else
            {
                str.push_back(phi::to_lower_case(text.at(str_index)));
            }
        }

        res = lexer.ProcessString({str.data(), str.length()});
        REQUIRE(res.size().unsafe() == 1u);

        CHECK(res.at(0u).GetTokenKind() == expected_kind);
    }
#endif
}

TEST_CASE("Lexer - Lex empty")
{
    res = lexer.ProcessString("");
    REQUIRE(res.is_empty());

    res = lexer.ProcessString("\0");
    REQUIRE(res.is_empty());

    res = lexer.ProcessString(" ");
    REQUIRE(res.is_empty());

    res = lexer.ProcessString(" \0");
    REQUIRE(res.is_empty());

    res = lexer.ProcessString("          ");
    REQUIRE(res.is_empty());
}

TEST_CASE("Lexer - Lex new lines")
{
    // 1 New line
    res = lexer.ProcessString("\n");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);

    res = lexer.ProcessString(" \n");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);

    res = lexer.ProcessString("  \n");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 3u);

    res = lexer.ProcessString("\n ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);

    res = lexer.ProcessString("\n  ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);

    // 2 New lines
    res = lexer.ProcessString("\n\n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 1u);

    res = lexer.ProcessString(" \n\n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 1u);

    res = lexer.ProcessString("\n \n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 2u);

    res = lexer.ProcessString("\n\n ");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 1u);

    res = lexer.ProcessString(" \n \n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 2u);

    res = lexer.ProcessString(" \n\n ");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 1u);

    res = lexer.ProcessString("\n \n ");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 2u);

    res = lexer.ProcessString(" \n \n ");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 2u);

    // 3 new lines
    res = lexer.ProcessString("\n\n\n");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 3u, 1u);
}

TEST_CASE("Lexer - Lex comments")
{
    res = lexer.ProcessString(";");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), Comment, ";", 1u, 1u);

    res = lexer.ProcessString(" ;");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), Comment, ";", 1u, 2u);

    res = lexer.ProcessString("; ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), Comment, "; ", 1u, 1u);

    res = lexer.ProcessString("; Comment");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), Comment, "; Comment", 1u, 1u);

    res = lexer.ProcessString(";blub");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), Comment, ";blub", 1u, 1u);

    // NewLine -> Comment
    res = lexer.ProcessString("\n;");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), Comment, ";", 2u, 1u);

    res = lexer.ProcessString(" \n;");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), Comment, ";", 2u, 1u);

    res = lexer.ProcessString("\n ;");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), Comment, ";", 2u, 2u);

    res = lexer.ProcessString("\n; ");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), Comment, "; ", 2u, 1u);

    res = lexer.ProcessString(" \n ;");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), Comment, ";", 2u, 2u);

    res = lexer.ProcessString(" \n; ");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), Comment, "; ", 2u, 1u);

    res = lexer.ProcessString(" \n ; ");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), Comment, "; ", 2u, 2u);

    res = lexer.ProcessString("\n; What a comment");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), Comment, "; What a comment", 2u, 1u);

    // Comment -> NewLine
    res = lexer.ProcessString(";\n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), Comment, ";", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 2u);

    res = lexer.ProcessString(" ;\n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), Comment, ";", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);

    res = lexer.ProcessString("; \n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), Comment, "; ", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);

    res = lexer.ProcessString(";\n ");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), Comment, ";", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 2u);

    res = lexer.ProcessString(" ; \n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), Comment, "; ", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 4u);

    res = lexer.ProcessString("; \n ");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), Comment, "; ", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);

    res = lexer.ProcessString(" ; \n ");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), Comment, "; ", 1u, 2u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 4u);

    res = lexer.ProcessString("; What a comment\n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), Comment, "; What a comment", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 17u);
}

TEST_CASE("Lexer - Lex Macros")
{
    res = lexer.ProcessString("@AppDataCommonDir");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), MK_AppDataCommonDir, "@AppDataCommonDir", 1u, 1u);

    res = lexer.ProcessString(" @AppDataCommonDir");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), MK_AppDataCommonDir, "@AppDataCommonDir", 1u, 2u);

    res = lexer.ProcessString("@AppDataCommonDir ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), MK_AppDataCommonDir, "@AppDataCommonDir", 1u, 1u);

    res = lexer.ProcessString("@AppDataCommonDir\n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), MK_AppDataCommonDir, "@AppDataCommonDir", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 18u);

    res = lexer.ProcessString("\n@AppDataCommonDir");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), MK_AppDataCommonDir, "@AppDataCommonDir", 2u, 1u);

    res = lexer.ProcessString("\n@AppDataCommonDir\n");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), MK_AppDataCommonDir, "@AppDataCommonDir", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 18u);

    res = lexer.ProcessString("@AppDataCommonDir\n@AppDataCommonDir");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), MK_AppDataCommonDir, "@AppDataCommonDir", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 18u);
    TOKEN_MATCHES(res.at(2u), MK_AppDataCommonDir, "@AppDataCommonDir", 2u, 1u);

    SECTION("Test all macros")
    {
        test_all_spellings_of("@AppDataCommonDir", OpenAutoIt::TokenKind::MK_AppDataCommonDir);
        test_all_spellings_of("@AppDataDir", OpenAutoIt::TokenKind::MK_AppDataDir);
        test_all_spellings_of("@AutoItExe", OpenAutoIt::TokenKind::MK_AutoItExe);
        test_all_spellings_of("@AutoItPID", OpenAutoIt::TokenKind::MK_AutoItPID);
        test_all_spellings_of("@AutoItVersion", OpenAutoIt::TokenKind::MK_AutoItVersion);
        test_all_spellings_of("@AutoItX64", OpenAutoIt::TokenKind::MK_AutoItX64);
        test_all_spellings_of("@COM_EventObj", OpenAutoIt::TokenKind::MK_COM_EventObj);
        test_all_spellings_of("@CommonFilesDir", OpenAutoIt::TokenKind::MK_CommonFilesDir);
        test_all_spellings_of("@Compiled", OpenAutoIt::TokenKind::MK_Compiled);
        test_all_spellings_of("@ComputerName", OpenAutoIt::TokenKind::MK_ComputerName);
        test_all_spellings_of("@ComSpec", OpenAutoIt::TokenKind::MK_ComSpec);
        test_all_spellings_of("@CPUArch", OpenAutoIt::TokenKind::MK_CPUArch);
        test_all_spellings_of("@CR", OpenAutoIt::TokenKind::MK_CR);
        test_all_spellings_of("@CRLF", OpenAutoIt::TokenKind::MK_CRLF);
        test_all_spellings_of("@DesktopCommonDir", OpenAutoIt::TokenKind::MK_DesktopCommonDir);
        test_all_spellings_of("@DesktopDepth", OpenAutoIt::TokenKind::MK_DesktopDepth);
        test_all_spellings_of("@DesktopDir", OpenAutoIt::TokenKind::MK_DesktopDir);
        test_all_spellings_of("@DesktopHeight", OpenAutoIt::TokenKind::MK_DesktopHeight);
        test_all_spellings_of("@DesktopRefresh", OpenAutoIt::TokenKind::MK_DesktopRefresh);
        test_all_spellings_of("@DesktopWidth", OpenAutoIt::TokenKind::MK_DesktopWidth);
        test_all_spellings_of("@DocumentsCommonDir", OpenAutoIt::TokenKind::MK_DocumentsCommonDir);
        test_all_spellings_of("@error", OpenAutoIt::TokenKind::MK_error);
        test_all_spellings_of("@exitCode", OpenAutoIt::TokenKind::MK_exitCode);
        test_all_spellings_of("@exitMethod", OpenAutoIt::TokenKind::MK_exitMethod);
        test_all_spellings_of("@extended", OpenAutoIt::TokenKind::MK_extended);
        test_all_spellings_of("@FavoritesCommonDir", OpenAutoIt::TokenKind::MK_FavoritesCommonDir);
        test_all_spellings_of("@FavoritesDir", OpenAutoIt::TokenKind::MK_FavoritesDir);
        test_all_spellings_of("@GUI_CtrlHandle", OpenAutoIt::TokenKind::MK_GUI_CtrlHandle);
        test_all_spellings_of("@GUI_CtrlId", OpenAutoIt::TokenKind::MK_GUI_CtrlId);
        test_all_spellings_of("@GUI_DragFile", OpenAutoIt::TokenKind::MK_GUI_DragFile);
        test_all_spellings_of("@GUI_DragId", OpenAutoIt::TokenKind::MK_GUI_DragId);
        test_all_spellings_of("@GUI_DropId", OpenAutoIt::TokenKind::MK_GUI_DropId);
        test_all_spellings_of("@GUI_WinHandle", OpenAutoIt::TokenKind::MK_GUI_WinHandle);
        test_all_spellings_of("@HomeDrive", OpenAutoIt::TokenKind::MK_HomeDrive);
        test_all_spellings_of("@HomePath", OpenAutoIt::TokenKind::MK_HomePath);
        test_all_spellings_of("@HomeShare", OpenAutoIt::TokenKind::MK_HomeShare);
        test_all_spellings_of("@HotKeyPressed", OpenAutoIt::TokenKind::MK_HotKeyPressed);
        test_all_spellings_of("@HOUR", OpenAutoIt::TokenKind::MK_HOUR);
        test_all_spellings_of("@IPAddress1", OpenAutoIt::TokenKind::MK_IPAddress1);
        test_all_spellings_of("@IPAddress2", OpenAutoIt::TokenKind::MK_IPAddress2);
        test_all_spellings_of("@IPAddress3", OpenAutoIt::TokenKind::MK_IPAddress3);
        test_all_spellings_of("@IPAddress4", OpenAutoIt::TokenKind::MK_IPAddress4);
        test_all_spellings_of("@KBLayout", OpenAutoIt::TokenKind::MK_KBLayout);
        test_all_spellings_of("@LF", OpenAutoIt::TokenKind::MK_LF);
        test_all_spellings_of("@LocalAppDataDir", OpenAutoIt::TokenKind::MK_LocalAppDataDir);
        test_all_spellings_of("@LogonDNSDomain", OpenAutoIt::TokenKind::MK_LogonDNSDomain);
        test_all_spellings_of("@LogonDomain", OpenAutoIt::TokenKind::MK_LogonDomain);
        test_all_spellings_of("@LogonServer", OpenAutoIt::TokenKind::MK_LogonServer);
        test_all_spellings_of("@MDAY", OpenAutoIt::TokenKind::MK_MDAY);
        test_all_spellings_of("@MIN", OpenAutoIt::TokenKind::MK_MIN);
        test_all_spellings_of("@MON", OpenAutoIt::TokenKind::MK_MON);
        test_all_spellings_of("@MSEC", OpenAutoIt::TokenKind::MK_MSEC);
        test_all_spellings_of("@MUILang", OpenAutoIt::TokenKind::MK_MUILang);
        test_all_spellings_of("@MyDocumentsDir", OpenAutoIt::TokenKind::MK_MyDocumentsDir);
        test_all_spellings_of("@NumParams", OpenAutoIt::TokenKind::MK_NumParams);
        test_all_spellings_of("@OSArch", OpenAutoIt::TokenKind::MK_OSArch);
        test_all_spellings_of("@OSBuild", OpenAutoIt::TokenKind::MK_OSBuild);
        test_all_spellings_of("@OSLang", OpenAutoIt::TokenKind::MK_OSLang);
        test_all_spellings_of("@OSServicePack", OpenAutoIt::TokenKind::MK_OSServicePack);
        test_all_spellings_of("@OSType", OpenAutoIt::TokenKind::MK_OSType);
        test_all_spellings_of("@OSVersion", OpenAutoIt::TokenKind::MK_OSVersion);
        test_all_spellings_of("@ProgramFilesDir", OpenAutoIt::TokenKind::MK_ProgramFilesDir);
        test_all_spellings_of("@ProgramsCommonDir", OpenAutoIt::TokenKind::MK_ProgramsCommonDir);
        test_all_spellings_of("@ProgramsDir", OpenAutoIt::TokenKind::MK_ProgramsDir);
        test_all_spellings_of("@ScriptDir", OpenAutoIt::TokenKind::MK_ScriptDir);
        test_all_spellings_of("@ScriptFullPath", OpenAutoIt::TokenKind::MK_ScriptFullPath);
        test_all_spellings_of("@ScriptLineNumber", OpenAutoIt::TokenKind::MK_ScriptLineNumber);
        test_all_spellings_of("@ScriptName", OpenAutoIt::TokenKind::MK_ScriptName);
        test_all_spellings_of("@SEC", OpenAutoIt::TokenKind::MK_SEC);
        test_all_spellings_of("@StartMenuCommonDir", OpenAutoIt::TokenKind::MK_StartMenuCommonDir);
        test_all_spellings_of("@StartMenuDir", OpenAutoIt::TokenKind::MK_StartMenuDir);
        test_all_spellings_of("@StartupCommonDir", OpenAutoIt::TokenKind::MK_StartupCommonDir);
        test_all_spellings_of("@StartupDir", OpenAutoIt::TokenKind::MK_StartupDir);
        test_all_spellings_of("@SW_DISABLE", OpenAutoIt::TokenKind::MK_SW_DISABLE);
        test_all_spellings_of("@SW_ENABLE", OpenAutoIt::TokenKind::MK_SW_ENABLE);
        test_all_spellings_of("@SW_HIDE", OpenAutoIt::TokenKind::MK_SW_HIDE);
        test_all_spellings_of("@SW_LOCK", OpenAutoIt::TokenKind::MK_SW_LOCK);
        test_all_spellings_of("@SW_MAXIMIZE", OpenAutoIt::TokenKind::MK_SW_MAXIMIZE);
        test_all_spellings_of("@SW_MINIMIZE", OpenAutoIt::TokenKind::MK_SW_MINIMIZE);
        test_all_spellings_of("@SW_RESTORE", OpenAutoIt::TokenKind::MK_SW_RESTORE);
        test_all_spellings_of("@SW_SHOW", OpenAutoIt::TokenKind::MK_SW_SHOW);
        test_all_spellings_of("@SW_SHOWDEFAULT", OpenAutoIt::TokenKind::MK_SW_SHOWDEFAULT);
        test_all_spellings_of("@SW_SHOWMAXIMIZED", OpenAutoIt::TokenKind::MK_SW_SHOWMAXIMIZED);
        test_all_spellings_of("@SW_SHOWMINIMIZED", OpenAutoIt::TokenKind::MK_SW_SHOWMINIMIZED);
        test_all_spellings_of("@SW_SHOWMINNOACTIVE", OpenAutoIt::TokenKind::MK_SW_SHOWMINNOACTIVE);
        test_all_spellings_of("@SW_SHOWNA", OpenAutoIt::TokenKind::MK_SW_SHOWNA);
        test_all_spellings_of("@SW_SHOWNOACTIVATE", OpenAutoIt::TokenKind::MK_SW_SHOWNOACTIVATE);
        test_all_spellings_of("@SW_SHOWNORMAL", OpenAutoIt::TokenKind::MK_SW_SHOWNORMAL);
        test_all_spellings_of("@SW_UNLOCK", OpenAutoIt::TokenKind::MK_SW_UNLOCK);
        test_all_spellings_of("@SystemDir", OpenAutoIt::TokenKind::MK_SystemDir);
        test_all_spellings_of("@TAB", OpenAutoIt::TokenKind::MK_TAB);
        test_all_spellings_of("@TempDir", OpenAutoIt::TokenKind::MK_TempDir);
        test_all_spellings_of("@TRAY_ID", OpenAutoIt::TokenKind::MK_TRAY_ID);
        test_all_spellings_of("@TrayIconFlashing", OpenAutoIt::TokenKind::MK_TrayIconFlashing);
        test_all_spellings_of("@TrayIconVisible", OpenAutoIt::TokenKind::MK_TrayIconVisible);
        test_all_spellings_of("@UserName", OpenAutoIt::TokenKind::MK_UserName);
        test_all_spellings_of("@UserProfileDir", OpenAutoIt::TokenKind::MK_UserProfileDir);
        test_all_spellings_of("@WDAY", OpenAutoIt::TokenKind::MK_WDAY);
        test_all_spellings_of("@WindowsDir", OpenAutoIt::TokenKind::MK_WindowsDir);
        test_all_spellings_of("@WorkingDir", OpenAutoIt::TokenKind::MK_WorkingDir);
        test_all_spellings_of("@YDAY", OpenAutoIt::TokenKind::MK_YDAY);
        test_all_spellings_of("@YEAR", OpenAutoIt::TokenKind::MK_YEAR);
    }
}

TEST_CASE("Lexer - Lex VariableIdentifier")
{
    res = lexer.ProcessString("$a");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), VariableIdentifier, "$a", 1u, 1u);

    res = lexer.ProcessString(" $a");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), VariableIdentifier, "$a", 1u, 2u);

    res = lexer.ProcessString("$a ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), VariableIdentifier, "$a", 1u, 1u);

    res = lexer.ProcessString(" $a ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), VariableIdentifier, "$a", 1u, 2u);

    res = lexer.ProcessString("$VeryLongVariableNameHere");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), VariableIdentifier, "$VeryLongVariableNameHere", 1u, 1u);

    res = lexer.ProcessString("$a\n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), VariableIdentifier, "$a", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);

    res = lexer.ProcessString("\n$a");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), VariableIdentifier, "$a", 2u, 1u);

    res = lexer.ProcessString("\n$a\n");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), VariableIdentifier, "$a", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 3u);

    res = lexer.ProcessString("$a\n$a");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), VariableIdentifier, "$a", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);
    TOKEN_MATCHES(res.at(2u), VariableIdentifier, "$a", 2u, 1u);
}

TEST_CASE("Lexer - Lex FunctionIdentifier")
{
    res = lexer.ProcessString("ValidIdentifier");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "ValidIdentifier", 1u, 1u);

    res = lexer.ProcessString(" ValidIdentifier");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "ValidIdentifier", 1u, 2u);

    res = lexer.ProcessString("ValidIdentifier ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "ValidIdentifier", 1u, 1u);

    res = lexer.ProcessString("_123");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "_123", 1u, 1u);

    res = lexer.ProcessString(" _123");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "_123", 1u, 2u);

    res = lexer.ProcessString("_123 ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "_123", 1u, 1u);

    res = lexer.ProcessString("a");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "a", 1u, 1u);

    res = lexer.ProcessString(" a");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "a", 1u, 2u);

    res = lexer.ProcessString("a ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "a", 1u, 1u);

    res = lexer.ProcessString(" a ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "a", 1u, 2u);

    res = lexer.ProcessString("A");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "A", 1u, 1u);

    res = lexer.ProcessString(" A");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "A", 1u, 2u);

    res = lexer.ProcessString("A ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "A", 1u, 1u);

    res = lexer.ProcessString(" A ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "A", 1u, 2u);

    res = lexer.ProcessString("A_\nB");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "A_", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);
    TOKEN_MATCHES(res.at(2u), FunctionIdentifier, "B", 2u, 1u);

    res = lexer.ProcessString("\na\n");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), FunctionIdentifier, "a", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 2u);

    res = lexer.ProcessString("A_\n B\n_123");
    REQUIRE(res.size().unsafe() == 5u);

    TOKEN_MATCHES(res.at(0u), FunctionIdentifier, "A_", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 3u);
    TOKEN_MATCHES(res.at(2u), FunctionIdentifier, "B", 2u, 2u);
    TOKEN_MATCHES(res.at(3u), NewLine, "\n", 2u, 3u);
    TOKEN_MATCHES(res.at(4u), FunctionIdentifier, "_123", 3u, 1u);
}

TEST_CASE("Lexer - Lex PreProcessorIdentifier")
{
    res = lexer.ProcessString("#include");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), PP_Include, "#include", 1u, 1u);

    res = lexer.ProcessString(" #include");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), PP_Include, "#include", 1u, 2u);

    res = lexer.ProcessString("#include ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), PP_Include, "#include", 1u, 1u);

    res = lexer.ProcessString(" #include ");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), PP_Include, "#include", 1u, 2u);

    res = lexer.ProcessString("#include\n#include");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), PP_Include, "#include", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 9u);
    TOKEN_MATCHES(res.at(2u), PP_Include, "#include", 2u, 1u);

    res = lexer.ProcessString("\n#include\n");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), PP_Include, "#include", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 9u);

    SECTION("Test all available macros")
    {
        test_all_spellings_of("#Comments-Start", OpenAutoIt::TokenKind::PP_CommentsStart);
        test_all_spellings_of("#Comments-End", OpenAutoIt::TokenKind::PP_CommentsEnd);
        test_all_spellings_of("#cs", OpenAutoIt::TokenKind::PP_CommentsStart);
        test_all_spellings_of("#ce", OpenAutoIt::TokenKind::PP_CommentsEnd);
        test_all_spellings_of("#include", OpenAutoIt::TokenKind::PP_Include);
        test_all_spellings_of("#include-once", OpenAutoIt::TokenKind::PP_IncludeOnce);
        test_all_spellings_of("#NoTrayIcon", OpenAutoIt::TokenKind::PP_NoTrayIcon);
        test_all_spellings_of("#OnAutoItStartRegister",
                              OpenAutoIt::TokenKind::PP_OnAutoItStartRegister);
        test_all_spellings_of("#pragma", OpenAutoIt::TokenKind::PP_Pragma);
        test_all_spellings_of("#RequireAdmin", OpenAutoIt::TokenKind::PP_RequireAdmin);
    }
}

TEST_CASE("Lexer - Lex Multiline comments")
{
    res = lexer.ProcessString("#cs Hello This is a comment#ce");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), PP_CommentsStart, "#cs", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), Comment, " Hello This is a comment", 1u, 4u);
    TOKEN_MATCHES(res.at(2u), PP_CommentsEnd, "#ce", 1u, 28u);

    res = lexer.ProcessString("#cs\n#ce");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), PP_CommentsStart, "#cs", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), Comment, "\n", 1u, 4u);
    TOKEN_MATCHES(res.at(2u), PP_CommentsEnd, "#ce", 2u, 1u);

    res = lexer.ProcessString("#cs\nHello i am a comment\n#ce");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), PP_CommentsStart, "#cs", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), Comment, "\nHello i am a comment\n", 1u, 4u);
    TOKEN_MATCHES(res.at(2u), PP_CommentsEnd, "#ce", 3u, 1u);
}

TEST_CASE("Lexer - Lex Keywords")
{
    res = lexer.ProcessString("false");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), KW_False, "false", 1u, 1u);

    res = lexer.ProcessString("false\n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), KW_False, "false", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 6u);

    res = lexer.ProcessString("\nfalse");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), KW_False, "false", 2u, 1u);

    res = lexer.ProcessString("\nfalse\n");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), KW_False, "false", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 6u);

    res = lexer.ProcessString("false\nfalse");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), KW_False, "false", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 6u);
    TOKEN_MATCHES(res.at(2u), KW_False, "false", 2u, 1u);

    SECTION("Test all keywords")
    {
        test_all_spellings_of("False", OpenAutoIt::TokenKind::KW_False);
        test_all_spellings_of("True", OpenAutoIt::TokenKind::KW_True);
        test_all_spellings_of("ContinueCase", OpenAutoIt::TokenKind::KW_ContinueCase);
        test_all_spellings_of("ContinueLoop", OpenAutoIt::TokenKind::KW_ContinueLoop);
        test_all_spellings_of("Default", OpenAutoIt::TokenKind::KW_Default);
        test_all_spellings_of("Dim", OpenAutoIt::TokenKind::KW_Dim);
        test_all_spellings_of("Local", OpenAutoIt::TokenKind::KW_Local);
        test_all_spellings_of("Global", OpenAutoIt::TokenKind::KW_Global);
        test_all_spellings_of("Const", OpenAutoIt::TokenKind::KW_Const);
        test_all_spellings_of("Do", OpenAutoIt::TokenKind::KW_Do);
        test_all_spellings_of("Until", OpenAutoIt::TokenKind::KW_Until);
        test_all_spellings_of("Enum", OpenAutoIt::TokenKind::KW_Enum);
        test_all_spellings_of("Exit", OpenAutoIt::TokenKind::KW_Exit);
        test_all_spellings_of("ExitLoop", OpenAutoIt::TokenKind::KW_ExitLoop);
        test_all_spellings_of("For", OpenAutoIt::TokenKind::KW_For);
        test_all_spellings_of("To", OpenAutoIt::TokenKind::KW_To);
        test_all_spellings_of("Step", OpenAutoIt::TokenKind::KW_Step);
        test_all_spellings_of("Next", OpenAutoIt::TokenKind::KW_Next);
        test_all_spellings_of("In", OpenAutoIt::TokenKind::KW_In);
        test_all_spellings_of("Func", OpenAutoIt::TokenKind::KW_Func);
        test_all_spellings_of("Return", OpenAutoIt::TokenKind::KW_Return);
        test_all_spellings_of("ByRef", OpenAutoIt::TokenKind::KW_ByRef);
        test_all_spellings_of("EndFunc", OpenAutoIt::TokenKind::KW_EndFunc);
        test_all_spellings_of("If", OpenAutoIt::TokenKind::KW_If);
        test_all_spellings_of("Then", OpenAutoIt::TokenKind::KW_Then);
        test_all_spellings_of("EndIf", OpenAutoIt::TokenKind::KW_EndIf);
        test_all_spellings_of("ElseIf", OpenAutoIt::TokenKind::KW_ElseIf);
        test_all_spellings_of("Else", OpenAutoIt::TokenKind::KW_Else);
        test_all_spellings_of("Null", OpenAutoIt::TokenKind::KW_Null);
        test_all_spellings_of("ReDim", OpenAutoIt::TokenKind::KW_ReDim);
        test_all_spellings_of("Select", OpenAutoIt::TokenKind::KW_Select);
        test_all_spellings_of("Case", OpenAutoIt::TokenKind::KW_Case);
        test_all_spellings_of("EndSelect", OpenAutoIt::TokenKind::KW_EndSelect);
        test_all_spellings_of("Static", OpenAutoIt::TokenKind::KW_Static);
        test_all_spellings_of("Switch", OpenAutoIt::TokenKind::KW_Switch);
        test_all_spellings_of("EndSwitch", OpenAutoIt::TokenKind::KW_EndSwitch);
        test_all_spellings_of("Volatile", OpenAutoIt::TokenKind::KW_Volatile);
        test_all_spellings_of("While", OpenAutoIt::TokenKind::KW_While);
        test_all_spellings_of("WEnd", OpenAutoIt::TokenKind::KW_WEnd);
        test_all_spellings_of("With", OpenAutoIt::TokenKind::KW_With);
        test_all_spellings_of("EndWith", OpenAutoIt::TokenKind::KW_EndWith);
        test_all_spellings_of("And", OpenAutoIt::TokenKind::KW_And);
        test_all_spellings_of("Or", OpenAutoIt::TokenKind::KW_Or);
        test_all_spellings_of("Not", OpenAutoIt::TokenKind::KW_Not);
    }
}

TEST_CASE("Lexer - Lex StringLiterals")
{
    res = lexer.ProcessString(R"("")");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"("")", 1u, 1u);

    res = lexer.ProcessString(R"('')");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"('')", 1u, 1u);

    res = lexer.ProcessString(R"("a")");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"("a")", 1u, 1u);

    res = lexer.ProcessString(R"('a')");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"('a')", 1u, 1u);

    res = lexer.ProcessString(R"("'")");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"("'")", 1u, 1u);

    res = lexer.ProcessString(R"('"')");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"('"')", 1u, 1u);

    // TODO: Is this correct?
    // NOTE: Apperently this is parsed as a string with simply "
    res = lexer.ProcessString(R"("""")");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"("")", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), StringLiteral, R"("")", 1u, 3u);

    res = lexer.ProcessString(R"('''')");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"('')", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), StringLiteral, R"('')", 1u, 3u);

    // Autoit does not support character escaping
    res = lexer.ProcessString(R"("\")");
    REQUIRE(res.size().unsafe() == 1u);

    // FIXME: This line doesn't compile on MSVC? Bug??
#if PHI_COMPILER_IS_NOT(MSVC)
    TOKEN_MATCHES(res.at(0u), StringLiteral, R"("\")", 1u, 1u);
#endif

    res = lexer.ProcessString(R"('\')");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), StringLiteral, R"('\')", 1u, 1u);

    SECTION("Untermianted StringLiterals")
    {
        res = lexer.ProcessString(R"(")");
        REQUIRE(res.is_empty());

        res = lexer.ProcessString(R"(')");
        REQUIRE(res.is_empty());
    }
}

TEST_CASE("Lexer - Lex IntegerLiterals")
{
    res = lexer.ProcessString("0");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "0", 1u, 1u);

    res = lexer.ProcessString("1");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "1", 1u, 1u);

    res = lexer.ProcessString("12");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "12", 1u, 1u);

    res = lexer.ProcessString("12345678900123456789");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "12345678900123456789", 1u, 1u);

    res = lexer.ProcessString("\n1");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), IntegerLiteral, "1", 2u, 1u);

    res = lexer.ProcessString("1\n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "1", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 2u);

    res = lexer.ProcessString("\n1\n");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), IntegerLiteral, "1", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 2u);

    res = lexer.ProcessString("1\n1");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "1", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(2u), IntegerLiteral, "1", 2u, 1u);
}

TEST_CASE("Lexer - Hex Literals")
{
    res = lexer.ProcessString("0x0");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "0x0", 1u, 1u);

    res = lexer.ProcessString("0X0");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "0X0", 1u, 1u);

    res = lexer.ProcessString("0x1");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "0x1", 1u, 1u);

    res = lexer.ProcessString("0X1");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "0X1", 1u, 1u);

    res = lexer.ProcessString("0x1234567890");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "0x1234567890", 1u, 1u);

    res = lexer.ProcessString("0xabcdefABCDEF");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), IntegerLiteral, "0xabcdefABCDEF", 1u, 1u);
}

TEST_CASE("Lexer - Lex Operator")
{
    res = lexer.ProcessString("=");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_Equals, "=", 1u, 1u);

    res = lexer.ProcessString("=\n");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), OP_Equals, "=", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 2u);

    res = lexer.ProcessString("\n=");
    REQUIRE(res.size().unsafe() == 2u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), OP_Equals, "=", 2u, 1u);

    res = lexer.ProcessString("\n=\n");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), NewLine, "\n", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), OP_Equals, "=", 2u, 1u);
    TOKEN_MATCHES(res.at(2u), NewLine, "\n", 2u, 2u);

    res = lexer.ProcessString("=\n=");
    REQUIRE(res.size().unsafe() == 3u);

    TOKEN_MATCHES(res.at(0u), OP_Equals, "=", 1u, 1u);
    TOKEN_MATCHES(res.at(1u), NewLine, "\n", 1u, 2u);
    TOKEN_MATCHES(res.at(2u), OP_Equals, "=", 2u, 1u);

    res = lexer.ProcessString("+=");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_PlusEquals, "+=", 1u, 1u);

    res = lexer.ProcessString("-=");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_MinusEquals, "-=", 1u, 1u);

    res = lexer.ProcessString("*=");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_MultiplyEquals, "*=", 1u, 1u);

    res = lexer.ProcessString("/=");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_DivideEquals, "/=", 1u, 1u);

    res = lexer.ProcessString("+");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_Plus, "+", 1u, 1u);

    res = lexer.ProcessString("-");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_Minus, "-", 1u, 1u);

    res = lexer.ProcessString("*");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_Multiply, "*", 1u, 1u);

    res = lexer.ProcessString("/");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_Divide, "/", 1u, 1u);

    res = lexer.ProcessString("^");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_Raise, "^", 1u, 1u);

    res = lexer.ProcessString("==");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_EqualsEquals, "==", 1u, 1u);

    res = lexer.ProcessString("<>");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_NotEqual, "<>", 1u, 1u);

    res = lexer.ProcessString(">");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_GreaterThan, ">", 1u, 1u);

    res = lexer.ProcessString(">=");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_GreaterThanEqual, ">=", 1u, 1u);

    res = lexer.ProcessString("<");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_LessThan, "<", 1u, 1u);

    res = lexer.ProcessString("<=");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_LessThanEqual, "<=", 1u, 1u);

    res = lexer.ProcessString("?");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_TernaryIf, "?", 1u, 1u);

    res = lexer.ProcessString(":");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), OP_TernaryElse, ":", 1u, 1u);
}

TEST_CASE("Lexer - Lex Punctioation")
{
    res = lexer.ProcessString(",");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), Comma, ",", 1u, 1u);

    res = lexer.ProcessString("(");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), LParen, "(", 1u, 1u);

    res = lexer.ProcessString(")");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), RParen, ")", 1u, 1u);

    res = lexer.ProcessString(".");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), Dot, ".", 1u, 1u);

    res = lexer.ProcessString("[");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), LSquare, "[", 1u, 1u);

    res = lexer.ProcessString("]");
    REQUIRE(res.size().unsafe() == 1u);

    TOKEN_MATCHES(res.at(0u), RSquare, "]", 1u, 1u);
}
