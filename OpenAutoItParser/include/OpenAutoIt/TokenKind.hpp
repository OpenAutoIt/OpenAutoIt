#pragma once

#include <magic_enum.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/size_t.hpp>
#include <string_view>

namespace OpenAutoIt
{
    enum class TokenKind
    {
        NotAToken,

        EndOfFile, // '\0'
        NewLine,   // '\n'
        Comment,   // ; Comment

        VariableIdentifier, // $var
        FunctionIdentifier, // my_func

        /* Literals */
        IntegerLiteral, // 123
        FloatLiteral,   // 0.123
        StringLiteral,  // "String"

        /* Punctioation */
        Comma,   // ,
        LParen,  // (
        RParen,  // )
        Dot,     // .
        LSquare, // [
        RSquare, // ]

        /* Macros */
        // https://www.autoitscript.com/autoit3/docs/macros.htm
        MK_AppDataCommonDir,   // @AppDataCommonDir
        MK_AppDataDir,         // @AppDataDir
        MK_AutoItExe,          // @AutoItExe
        MK_AutoItPID,          // @AutoItPID
        MK_AutoItVersion,      // @AutoItVersion
        MK_AutoItX64,          // @AutoItX64
        MK_COM_EventObj,       // @COM_EventObj
        MK_CommonFilesDir,     // @CommonFilesDir
        MK_Compiled,           // @Compiled
        MK_ComputerName,       // @ComputerName
        MK_ComSpec,            // @ComSpec
        MK_CPUArch,            // @CPUArch
        MK_CR,                 // @CR
        MK_CRLF,               // @CRLF
        MK_DesktopCommonDir,   // @DesktopCommonDir
        MK_DesktopDepth,       // @DesktopDepth
        MK_DesktopDir,         // @DesktopDir
        MK_DesktopHeight,      // @DesktopHeight
        MK_DesktopRefresh,     // @DesktopRefresh
        MK_DesktopWidth,       // @DesktopWidth
        MK_DocumentsCommonDir, // @DocumentsCommonDir
        MK_error,              // @error
        MK_exitCode,           // @exitCode
        MK_exitMethod,         // @exitMethod
        MK_extended,           // @extended
        MK_FavoritesCommonDir, // @FavoritesCommonDir
        MK_FavoritesDir,       // @FavoritesDir
        MK_GUI_CtrlHandle,     // @GUI_CtrlHandle
        MK_GUI_CtrlId,         // @GUI_CtrlId
        MK_GUI_DragFile,       // @GUI_DragFile
        MK_GUI_DragId,         // @GUI_DragId
        MK_GUI_DropId,         // @GUI_DropId
        MK_GUI_WinHandle,      // @GUI_WinHandle
        MK_HomeDrive,          // @HomeDrive
        MK_HomePath,           // @HomePath
        MK_HomeShare,          // @HomeShare
        MK_HotKeyPressed,      // @HotKeyPressed
        MK_HOUR,               // @HOUR
        MK_IPAddress1,         // @IPAddress1
        MK_IPAddress2,         // @IPAddress2
        MK_IPAddress3,         // @IPAddress3
        MK_IPAddress4,         // @IPAddress4
        MK_KBLayout,           // @KBLayout
        MK_LF,                 // @LF
        MK_LocalAppDataDir,    // @LocalAppDataDir
        MK_LogonDNSDomain,     // @LogonDNSDomain
        MK_LogonDomain,        // @LogonDomain
        MK_LogonServer,        // @LogonServer
        MK_MDAY,               // @MDAY
        MK_MIN,                // @MIN
        MK_MON,                // @MON
        MK_MSEC,               // @MSEC
        MK_MUILang,            // @MUILang
        MK_MyDocumentsDir,     // @MyDocumentsDir
        MK_NumParams,          // @NumParams
        MK_OSArch,             // @OSArch
        MK_OSBuild,            // @OSBuild
        MK_OSLang,             // @OSLang
        MK_OSServicePack,      // @OSServicePack
        MK_OSType,             // @OSType
        MK_OSVersion,          // @OSVersion
        MK_ProgramFilesDir,    // @ProgramFilesDir
        MK_ProgramsCommonDir,  // @ProgramsCommonDir
        MK_ProgramsDir,        // @ProgramsDir
        MK_ScriptDir,          // @ScriptDir
        MK_ScriptFullPath,     // @ScriptFullPath
        MK_ScriptLineNumber,   // @ScriptLineNumber
        MK_ScriptName,         // @ScriptName
        MK_SEC,                // @SEC
        MK_StartMenuCommonDir, // @StartMenuCommonDir
        MK_StartMenuDir,       // @StartMenuDir
        MK_StartupCommonDir,   // @StartupCommonDir
        MK_StartupDir,         // @StartupDir
        MK_SW_DISABLE,         // @SW_DISABLE
        MK_SW_ENABLE,          // @SW_ENABLE
        MK_SW_HIDE,            // @SW_HIDE
        MK_SW_LOCK,            // @SW_LOCK
        MK_SW_MAXIMIZE,        // @SW_MAXIMIZE
        MK_SW_MINIMIZE,        // @SW_MINIMIZE
        MK_SW_RESTORE,         // @SW_RESTORE
        MK_SW_SHOW,            // @SW_SHOW
        MK_SW_SHOWDEFAULT,     // @SW_SHOWDEFAULT
        MK_SW_SHOWMAXIMIZED,   // @SW_SHOWMAXIMIZED
        MK_SW_SHOWMINIMIZED,   // @SW_SHOWMINIMIZED
        MK_SW_SHOWMINNOACTIVE, // @SW_SHOWMINNOACTIVE
        MK_SW_SHOWNA,          // @SW_SHOWNA
        MK_SW_SHOWNOACTIVATE,  // @SW_SHOWNOACTIVATE
        MK_SW_SHOWNORMAL,      // @SW_SHOWNORMAL
        MK_SW_UNLOCK,          // @SW_UNLOCK
        MK_SystemDir,          // @SystemDir
        MK_TAB,                // @TAB
        MK_TempDir,            // @TempDir
        MK_TRAY_ID,            // @TRAY_ID
        MK_TrayIconFlashing,   // @TrayIconFlashing
        MK_TrayIconVisible,    // @TrayIconVisible
        MK_UserName,           // @UserName
        MK_UserProfileDir,     // @UserProfileDir
        MK_WDAY,               // @WDAY
        MK_WindowsDir,         // @WindowsDir
        MK_WorkingDir,         // @WorkingDir
        MK_YDAY,               // @YDAY
        MK_YEAR,               // @YEAR

        /* Preprocessor identifiers */
        // https://www.autoitscript.com/autoit3/docs/intro/lang_directives.htm

        // https://www.autoitscript.com/autoit3/docs/keywords/comments-start.htm
        PP_CommentsStart, // #Comments-Start, #cs
        PP_CommentsEnd,   // #Comments-End, #ce
        // https://www.autoitscript.com/autoit3/docs/keywords/include.htm
        PP_Include, // #include
        // https://www.autoitscript.com/autoit3/docs/keywords/include-once.htm
        PP_IncludeOnce, // #include-once
        // https://www.autoitscript.com/autoit3/docs/keywords/NoTrayIcon.htm
        PP_NoTrayIcon, // #NoTrayIcon
        // https://www.autoitscript.com/autoit3/docs/keywords/OnAutoItStartRegister.htm
        PP_OnAutoItStartRegister, // #OnAutoitStartRegister
        // https://www.autoitscript.com/autoit3/docs/keywords/pragma.htm
        PP_Pragma, // #pragma
        // https://www.autoitscript.com/autoit3/docs/keywords/RequireAdmin.htm
        PP_RequireAdmin, // #RequireAdmin

        /* Keywords */
        // https://www.autoitscript.com/autoit3/docs/keywords.htm
        // https://www.autoitscript.com/autoit3/docs/intro/lang_operators.htm

        // https://www.autoitscript.com/autoit3/docs/keywords/Booleans.htm
        KW_False, // False
        KW_True,  // True
        // https://www.autoitscript.com/autoit3/docs/keywords/ContinueCase.htm
        KW_ContinueCase, // ContinueCase
        // https://www.autoitscript.com/autoit3/docs/keywords/ContinueLoop.htm
        KW_ContinueLoop, // ContinueLoop
        // https://www.autoitscript.com/autoit3/docs/keywords/Default.htm
        KW_Default, // Default
        // https://www.autoitscript.com/autoit3/docs/keywords/Dim.htm
        KW_Dim,    // Dim
        KW_Local,  // Local
        KW_Global, // Global
        KW_Const,  // Const
        // https://www.autoitscript.com/autoit3/docs/keywords/Do.htm
        KW_Do,    // Do
        KW_Until, // Until
        // https://www.autoitscript.com/autoit3/docs/keywords/Enum.htm
        KW_Enum, // Enum
        // https://www.autoitscript.com/autoit3/docs/keywords/Exit.htm
        KW_Exit, // Exit
        // https://www.autoitscript.com/autoit3/docs/keywords/ExitLoop.htm
        KW_ExitLoop, // ExitLoop
        // https://www.autoitscript.com/autoit3/docs/keywords/For.htm
        KW_For,  // For
        KW_To,   // To
        KW_Step, // Step
        KW_Next, // Next
        // https://www.autoitscript.com/autoit3/docs/keywords/ForInNext.htm
        KW_In, // In
        // https://www.autoitscript.com/autoit3/docs/keywords/Func.htm
        KW_Func,    // Func
        KW_ByRef,   // ByRef
        KW_Return,  // Return
        KW_EndFunc, // EndFunc
        // https://www.autoitscript.com/autoit3/docs/keywords/If.htm
        KW_If,    // If
        KW_Then,  // Then
        KW_EndIf, // EndIf
        // https://www.autoitscript.com/autoit3/docs/keywords/IfElseEndIf.htm
        KW_ElseIf, // ElseIf
        KW_Else,   // Else
        // https://www.autoitscript.com/autoit3/docs/keywords/Null.htm
        KW_Null, // Null
        // https://www.autoitscript.com/autoit3/docs/keywords/ReDim.htm
        KW_ReDim, // ReDim
        // https://www.autoitscript.com/autoit3/docs/keywords/Select.htm
        KW_Select,    // Select
        KW_Case,      // Case
        KW_EndSelect, // EndSelect
        // https://www.autoitscript.com/autoit3/docs/keywords/Static.htm
        KW_Static, // Static
        // https://www.autoitscript.com/autoit3/docs/keywords/Switch.htm
        KW_Switch,    // Switch
        KW_EndSwitch, // EndSwitch
        // https://www.autoitscript.com/autoit3/docs/keywords/Volatile.htm
        KW_Volatile, // Volatile
        // https://www.autoitscript.com/autoit3/docs/keywords/While.htm
        KW_While, // While
        KW_WEnd,  // WEnd
        // https://www.autoitscript.com/autoit3/docs/keywords/With.htm
        KW_With,    // With
        KW_EndWith, // EndWith
        // https://www.autoitscript.com/autoit3/docs/intro/lang_operators.htm
        KW_And, // And
        KW_Or,  // Or
        KW_Not, // Not

        /* Operator */
        // https://www.autoitscript.com/autoit3/docs/intro/lang_operators.htm

        OP_Equals,            // = (Note includes assignment and case insensitive comparision)
        OP_PlusEquals,        // +=
        OP_MinusEquals,       // -=
        OP_MultiplyEquals,    // *=
        OP_DivideEquals,      // /=
        OP_Concatenate,       // &
        OP_ConcatenateEquals, // &=
        OP_Plus,              // +
        OP_Minus,             // -
        OP_Multiply,          // *
        OP_Divide,            // /
        OP_Raise,             // ^
        OP_EqualsEquals,      // ==
        OP_NotEqual,          // <>
        OP_GreaterThan,       // >
        OP_GreaterThanEqual,  // >=
        OP_LessThan,          // <
        OP_LessThanEqual,     // <=
        OP_TernaryIf,         // ?
        OP_TernaryElse,       // :

        // NOTE: Always keep last
        NumberOfTokens,
    };

    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wunused-const-variable")

    static constexpr const phi::size_t NumberOfTokens =
            static_cast<phi::size_t>(TokenKind::NumberOfTokens);

    PHI_GCC_SUPPRESS_WARNING_POP()

    [[nodiscard]] constexpr const char* get_token_name(TokenKind token_kind) noexcept
    {
        switch (token_kind)
        {
            case TokenKind::OP_Plus:
                return "OP_Plus";
            default:
                return magic_enum::enum_name<TokenKind>(token_kind).data();
        }
    }
} // namespace OpenAutoIt
