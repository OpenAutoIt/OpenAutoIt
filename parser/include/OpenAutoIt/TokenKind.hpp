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
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(Garbage)                                                       \
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
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(OP_TernaryElse)                                                \
    /* Built-in Functions */                                                                       \
    /* https://www.autoitscript.com/autoit3/docs/functions.htm */                                  \
    /* https://www.autoitscript.com/autoit3/docs/function_notes.htm */                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Abs)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ACos)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_AdlibRegister)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_AdlibUnRegister)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Asc)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_AscW)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ASin)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Assign)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ATan)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_AutoItSetOption)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_AutoItWinGetTitle)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_AutoItWinSetTitle)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Beep)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Binary)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_BinaryLen)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_BinaryMid)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_BinaryToString)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_BitAND)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_BitNOT)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_BitOR)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_BitRotate)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_BitShift)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_BitXOR)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_BlockInput)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Break)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Call)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_CDTray)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Ceiling)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Chr)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ChrW)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ClipGet)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ClipPut)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ConsoleRead)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ConsoleWrite)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ConsoleWriteError)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlClick)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlCommand)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlDisable)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlEnable)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlFocus)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlGetFocus)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlGetHandle)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlGetPos)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlGetText)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlHide)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlListView)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlMove)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlSend)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlSetText)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlShow)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ControlTreeView)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Cos)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Dec)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DirCopy)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DirCreate)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DirGetSize)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DirMove)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DirRemove)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DllCall)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DllCallAddress)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DllCallbackFree)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DllCallbackGetPtr)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DllCallbackRegister)                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DllClose)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DllOpen)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DllStructCreate)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DllStructGetData)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DllStructGetPtr)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DllStructGetSize)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DllStructSetData)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DriveGetDrive)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DriveGetFileSystem)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DriveGetLabel)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DriveGetSerial)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DriveGetType)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DriveMapAdd)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DriveMapDel)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DriveMapGet)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DriveSetLabel)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DriveSpaceFree)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DriveSpaceTotal)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_DriveStatus)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_EnvGet)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_EnvSet)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_EnvUpdate)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Eval)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Execute)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Exp)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileChangeDir)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileClose)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileCopy)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileCreateNTFSLink)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileCreateShortcut)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileDelete)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileExists)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileFindFirstFile)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileFindNextFile)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileFlush)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileGetAttrib)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileGetEncoding)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileGetLongName)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileGetPos)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileGetShortcut)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileGetShortName)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileGetSize)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileGetTime)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileGetVersion)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileInstall)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileMove)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileOpen)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileOpenDialog)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileRead)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileReadLine)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileReadToArray)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileRecycle)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileRecycleEmpty)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileSaveDialog)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileSelectFolder)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileSetAttrib)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileSetEnd)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileSetPos)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileSetTime)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileWrite)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FileWriteLine)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Floor)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FtpSetProxy)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_FuncName)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICreate)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateAvi)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateButton)                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateCheckbox)                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateCombo)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateContextMenu)                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateDate)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateDummy)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateEdit)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateGraphic)                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateGroup)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateIcon)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateInput)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateLabel)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateList)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateListView)                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateListViewItem)                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateMenu)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateMenuItem)                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateMonthCal)                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateObj)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreatePic)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateProgress)                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateRadio)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateSlider)                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateTab)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateTabItem)                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateTreeView)                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateTreeViewItem)                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlCreateUpdown)                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlDelete)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlGetHandle)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlGetState)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlRead)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlRecvMsg)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlRegisterListViewSort)                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSendMsg)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSendToDummy)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetBkColor)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetColor)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetCursor)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetData)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetDefBkColor)                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetDefColor)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetFont)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetGraphic)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetImage)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetLimit)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetOnEvent)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetPos)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetResizing)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetState)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetStyle)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUICtrlSetTip)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUIDelete)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUIGetCursorInfo)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUIGetMsg)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUIGetStyle)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUIRegisterMsg)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUISetAccelerators)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUISetBkColor)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUISetCoord)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUISetCursor)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUISetFont)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUISetHelp)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUISetIcon)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUISetOnEvent)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUISetState)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUISetStyle)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUIStartGroup)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_GUISwitch)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Hex)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_HotKeySet)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_HttpSetProxy)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_HttpSetUserAgent)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_HWnd)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_InetClose)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_InetGet)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_InetGetInfo)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_InetGetSize)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_InetRead)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IniDelete)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IniRead)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IniReadSection)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IniReadSectionNames)                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IniRenameSection)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IniWrite)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IniWriteSection)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_InputBox)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Int)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsAdmin)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsArray)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsBinary)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsBool)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsDeclared)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsDllStruct)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsFloat)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsFunc)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsHWnd)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsInt)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsKeyword)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsMap)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsNumber)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsObj)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsPtr)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_IsString)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Log)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MapAppend)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MapExists)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MapKeys)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MapRemove)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MemGetStats)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Mod)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MouseClick)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MouseClickDrag)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MouseDown)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MouseGetCursor)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MouseGetPos)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MouseMove)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MouseUp)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MouseWheel)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_MsgBox)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Number)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ObjCreate)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ObjCreateInterface)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ObjEvent)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ObjGet)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ObjName)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_OnAutoItExitRegister)                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_OnAutoItExitUnRegister)                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Ping)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_PixelChecksum)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_PixelGetColor)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_PixelSearch)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ProcessClose)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ProcessExists)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ProcessGetStats)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ProcessList)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ProcessSetPriority)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ProcessWait)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ProcessWaitClose)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ProgressOff)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ProgressOn)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ProgressSet)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Ptr)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Random)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_RegDelete)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_RegEnumKey)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_RegEnumVal)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_RegRead)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_RegWrite)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Round)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Run)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_RunAs)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_RunAsWait)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_RunWait)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Send)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_SendKeepActive)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_SetError)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_SetExtended)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ShellExecute)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ShellExecuteWait)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Shutdown)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Sin)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Sleep)                                                      \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_SoundPlay)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_SoundSetWaveVolume)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_SplashImageOn)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_SplashOff)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_SplashTextOn)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Sqrt)                                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_SRandom)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StatusbarGetText)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StderrRead)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StdinWrite)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StdioClose)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StdoutRead)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_String)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringAddCR)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringCompare)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringFormat)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringFromASCIIArray)                                       \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringInStr)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringIsAlNum)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringIsAlpha)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringIsASCII)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringIsDigit)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringIsFloat)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringIsInt)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringIsLower)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringIsSpace)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringIsUpper)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringIsXDigit)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringLeft)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringLen)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringLower)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringMid)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringRegExp)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringRegExpReplace)                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringReplace)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringReverse)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringRight)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringSplit)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringStripCR)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringStripWS)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringToASCIIArray)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringToBinary)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringTrimLeft)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringTrimRight)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_StringUpper)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_Tan)                                                        \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TCPAccept)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TCPCloseSocket)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TCPConnect)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TCPListen)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TCPNameToIP)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TCPRecv)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TCPSend)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TCPShutdown)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TCPStartup)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TimerDiff)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TimerInit)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ToolTip)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TrayCreateItem)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TrayCreateMenu)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TrayGetMsg)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TrayItemDelete)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TrayItemGetHandle)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TrayItemGetState)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TrayItemGetText)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TrayItemSetOnEvent)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TrayItemSetState)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TrayItemSetText)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TraySetClick)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TraySetIcon)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TraySetOnEvent)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TraySetPauseIcon)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TraySetState)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TraySetToolTip)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_TrayTip)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_UBound)                                                     \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_UDPBind)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_UDPCloseSocket)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_UDPOpen)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_UDPRecv)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_UDPSend)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_UDPShutdown)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_UDPStartup)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_VarGetType)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinActivate)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinActive)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinClose)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinExists)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinFlash)                                                   \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinGetCaretPos)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinGetClassList)                                            \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinGetClientSize)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinGetHandle)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinGetPos)                                                  \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinGetProcess)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinGetState)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinGetText)                                                 \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinGetTitle)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinKill)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinList)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinMenuSelectItem)                                          \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinMinimizeAll)                                             \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinMinimizeAllUndo)                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinMove)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinSetOnTop)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinSetState)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinSetTitle)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinSetTrans)                                                \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinWait)                                                    \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinWaitActive)                                              \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinWaitClose)                                               \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_WinWaitNotActive)                                           \
    /* OpenAutoIt built-in Function language extensions */                                         \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ConsoleWriteLine)                                           \
    OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(BI_ConsoleWriteErrorLine)

    enum class TokenKind
    {
#define OPENAUTOIT_ENUM_TOKEN_KIND_IMPL(name) name,

        OPENAUTOIT_ENUM_TOKEN_KIND()

#undef OPENAUTOIT_ENUM_TOKEN_KIND_IMPL

        // NOTE: Always keep last
        COUNT,
    };

    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wunused-const-variable")

    static constexpr const phi::size_t NumberOfTokens = static_cast<phi::size_t>(TokenKind::COUNT);
    static constexpr const phi::size_t BuiltInFirst   = static_cast<phi::size_t>(TokenKind::BI_Abs);
    static constexpr const phi::size_t BuiltInLast =
            static_cast<phi::size_t>(TokenKind::BI_ConsoleWriteErrorLine);

    PHI_GCC_SUPPRESS_WARNING_POP()

    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wreturn-type")
    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4702) // Unreachable code

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

    PHI_MSVC_SUPPRESS_WARNING_POP()
    PHI_GCC_SUPPRESS_WARNING_POP()

} // namespace OpenAutoIt
