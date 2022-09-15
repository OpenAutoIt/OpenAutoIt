#pragma once

#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/size_t.hpp>
#include <string_view>

namespace OpenAutoIt
{
#define OPENAUTOIT_ENUM_TOKEN_KIND()                                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(NotAToken)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(EndOfFile)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(NewLine)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(Comment)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(VariableIdentifier)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(FunctionIdentifier)                                            \
    /* Literals */                                                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(IntegerLiteral)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(FloatLiteral)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(StringLiteral)                                                 \
    /* Punctuation */                                                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(Comma)                                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(LParen)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(RParen)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(Dot)                                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(LSquare)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(RSquare)                                                       \
    /* Macros */                                                                                   \
    /* https://www.autoitscript.com/autoit3/docs/macros.htm */                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_AppDataCommonDir)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_AppDataDir)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_AutoItExe)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_AutoItPID)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_AutoItVersion)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_AutoItX64)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_COM_EventObj)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_CommonFilesDir)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_Compiled)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_ComputerName)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_ComSpec)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_CPUArch)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_CR)                                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_CRLF)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_DesktopCommonDir)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_DesktopDepth)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_DesktopDir)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_DesktopHeight)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_DesktopRefresh)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_DesktopWidth)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_DocumentsCommonDir)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_error)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_exitCode)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_exitMethod)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_extended)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_FavoritesCommonDir)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_FavoritesDir)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_GUI_CtrlHandle)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_GUI_CtrlId)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_GUI_DragFile)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_GUI_DragId)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_GUI_DropId)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_GUI_WinHandle)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_HomeDrive)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_HomePath)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_HomeShare)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_HotKeyPressed)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_HOUR)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_IPAddress1)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_IPAddress2)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_IPAddress3)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_IPAddress4)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_KBLayout)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_LF)                                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_LocalAppDataDir)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_LogonDNSDomain)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_LogonDomain)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_LogonServer)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_MDAY)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_MIN)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_MON)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_MSEC)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_MUILang)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_MyDocumentsDir)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_NumParams)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_OSArch)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_OSBuild)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_OSLang)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_OSServicePack)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_OSType)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_OSVersion)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_ProgramFilesDir)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_ProgramsCommonDir)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_ProgramsDir)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_ScriptDir)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_ScriptFullPath)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_ScriptLineNumber)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_ScriptName)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SEC)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_StartMenuCommonDir)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_StartMenuDir)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_StartupCommonDir)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_StartupDir)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_DISABLE)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_ENABLE)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_HIDE)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_LOCK)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_MAXIMIZE)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_MINIMIZE)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_RESTORE)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_SHOW)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_SHOWDEFAULT)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_SHOWMAXIMIZED)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_SHOWMINIMIZED)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_SHOWMINNOACTIVE)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_SHOWNA)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_SHOWNOACTIVATE)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_SHOWNORMAL)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SW_UNLOCK)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_SystemDir)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_TAB)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_TempDir)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_TRAY_ID)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_TrayIconFlashing)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_TrayIconVisible)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_UserName)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_UserProfileDir)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_WDAY)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_WindowsDir)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_WorkingDir)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_YDAY)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(MK_YEAR)                                                       \
    /* Preprocessor identifiers */                                                                 \
    /* https://www.autoitscript.com/autoit3/docs/intro/lang_directives.htm */                      \
    /* https://www.autoitscript.com/autoit3/docs/keywords/comments-start.htm */                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(PP_CommentsStart)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(PP_CommentsEnd)                                                \
    /* https://www.autoitscript.com/autoit3/docs/keywords/include.htm */                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(PP_Include)                                                    \
    /* https://www.autoitscript.com/autoit3/docs/keywords/include-once.htm */                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(PP_IncludeOnce)                                                \
    /* https://www.autoitscript.com/autoit3/docs/keywords/NoTrayIcon.htm */                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(PP_NoTrayIcon)                                                 \
    /* https://www.autoitscript.com/autoit3/docs/keywords/OnAutoItStartRegister.htm */             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(PP_OnAutoItStartRegister)                                      \
    /* https://www.autoitscript.com/autoit3/docs/keywords/pragma.htm */                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(PP_Pragma)                                                     \
    /* https://www.autoitscript.com/autoit3/docs/keywords/RequireAdmin.htm */                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(PP_RequireAdmin)                                               \
    /* Keywords */                                                                                 \
    /* https://www.autoitscript.com/autoit3/docs/keywords.htm */                                   \
    /* https://www.autoitscript.com/autoit3/docs/intro/lang_operators.htm */                       \
    /* https://www.autoitscript.com/autoit3/docs/keywords/Booleans.htm */                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_False)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_True)                                                       \
    /* https://www.autoitscript.com/autoit3/docs/keywords/ContinueCase.htm */                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_ContinueCase)                                               \
    /* https://www.autoitscript.com/autoit3/docs/keywords/ContinueLoop.htm */                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_ContinueLoop)                                               \
    /* https://www.autoitscript.com/autoit3/docs/keywords/Default.htm */                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Default)                                                    \
    /* https://www.autoitscript.com/autoit3/docs/keywords/Dim.htm */                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Dim)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Local)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Global)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Const)                                                      \
    /* https://www.autoitscript.com/autoit3/docs/keywords/Do.htm */                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Do)                                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Until)                                                      \
    /* https://www.autoitscript.com/autoit3/docs/keywords/Enum.htm */                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Enum)                                                       \
    /* https://www.autoitscript.com/autoit3/docs/keywords/Exit.htm */                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Exit)                                                       \
    /* https://www.autoitscript.com/autoit3/docs/keywords/ExitLoop.htm */                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_ExitLoop)                                                   \
    /* https://www.autoitscript.com/autoit3/docs/keywords/For.htm */                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_For)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_To)                                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Step)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Next)                                                       \
    /* https://www.autoitscript.com/autoit3/docs/keywords/ForInNext.htm */                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_In)                                                         \
    /* https://www.autoitscript.com/autoit3/docs/keywords/Func.htm */                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Func)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_ByRef)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Return)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_EndFunc)                                                    \
    /* https://www.autoitscript.com/autoit3/docs/keywords/If.htm */                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_If)                                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Then)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_EndIf)                                                      \
    /* https://www.autoitscript.com/autoit3/docs/keywords/IfElseEndIf.htm */                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_ElseIf)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Else)                                                       \
    /* https://www.autoitscript.com/autoit3/docs/keywords/Null.htm */                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Null)                                                       \
    /* https://www.autoitscript.com/autoit3/docs/keywords/ReDim.htm */                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_ReDim)                                                      \
    /* https://www.autoitscript.com/autoit3/docs/keywords/Select.htm */                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Select)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Case)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_EndSelect)                                                  \
    /* https://www.autoitscript.com/autoit3/docs/keywords/Static.htm */                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Static)                                                     \
    /* https://www.autoitscript.com/autoit3/docs/keywords/Switch.htm */                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Switch)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_EndSwitch)                                                  \
    /* https://www.autoitscript.com/autoit3/docs/keywords/Volatile.htm */                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Volatile)                                                   \
    /* https://www.autoitscript.com/autoit3/docs/keywords/While.htm */                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_While)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_WEnd)                                                       \
    /* https://www.autoitscript.com/autoit3/docs/keywords/With.htm */                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_With)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_EndWith)                                                    \
    /* https://www.autoitscript.com/autoit3/docs/intro/lang_operators.htm */                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_And)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Or)                                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(KW_Not)                                                        \
    /* Operator */                                                                                 \
    /* https://www.autoitscript.com/autoit3/docs/intro/lang_operators.htm */                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_Equals)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_PlusEquals)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_MinusEquals)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_MultiplyEquals)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_DivideEquals)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_Concatenate)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_ConcatenateEquals)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_Plus)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_Minus)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_Multiply)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_Divide)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_Raise)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_EqualsEquals)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_NotEqual)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_GreaterThan)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_GreaterThanEqual)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_LessThan)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_LessThanEqual)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_TernaryIf)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_TernaryElse)

    enum class TokenKind
    {
#define OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(name) name,

        OPENAUTOIT_ENUM_TOKEN_KIND()

#undef OPENAUTOIT_ENUM_TOKEN_KIND_IMPL

        // NOTE: Always keep last
        NumberOfTokens,
    };

    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wunused-const-variable")

    static constexpr const phi::size_t NumberOfTokens =
            static_cast<phi::size_t>(TokenKind::NumberOfTokens);

    PHI_GCC_SUPPRESS_WARNING_POP()

    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wreturn-type")

    [[nodiscard]] PHI_ATTRIBUTE_PURE constexpr const char* enum_name(TokenKind token_kind) noexcept
    {
        switch (token_kind)
        {
#define OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(name)                                                      \
    case TokenKind::name:                                                                          \
        return #name;

            OPENAUTOIT_ENUM_TOKEN_KIND()

#undef OPENAUTOIT_ENUM_TOKEN_KIND_IMPL

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }

    PHI_GCC_SUPPRESS_WARNING_POP()

} // namespace OpenAutoIt
