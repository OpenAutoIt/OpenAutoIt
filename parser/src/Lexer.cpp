#include "OpenAutoIt/Lexer.hpp"

#include "OpenAutoIt/SourceFile.hpp"
#include "OpenAutoIt/Token.hpp"
#include "OpenAutoIt/TokenKind.hpp"
#include "OpenAutoIt/TokenStream.hpp"
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/container/array.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/types.hpp>
#include <phi/text/is_alpha_numeric.hpp>
#include <phi/text/is_digit.hpp>
#include <phi/text/is_hex_digit.hpp>
#include <phi/text/to_lower_case.hpp>
#include <algorithm>
#include <array>
#include <map>
#include <string>

template <typename KeyT, typename ValueT, std::size_t SizeT>
class LookUpMap
{
public:
    constexpr LookUpMap(const std::array<std::pair<KeyT, ValueT>, SizeT>& data,
                        ValueT                                            default_value)
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

[[nodiscard]] OpenAutoIt::TokenKind lookup_macro(phi::string_view token)
{
    static constexpr auto map =
            LookUpMap<phi::string_view, OpenAutoIt::TokenKind, MacroValues.size()>(
                    MacroValues, OpenAutoIt::TokenKind::NotAToken);

    std::string str{token.begin(), token.end()};
    std::transform(str.begin(), str.end(), str.begin(), phi::to_lower_case);

    return map.at(phi::string_view(str.c_str(), str.size()));
}

static constexpr std::array<std::pair<phi::string_view, OpenAutoIt::TokenKind>, 10u>
        PreProcessorValues{{
                {"#comments-start", OpenAutoIt::TokenKind::PP_CommentsStart},
                {"#comments-end", OpenAutoIt::TokenKind::PP_CommentsEnd},
                {"#cs", OpenAutoIt::TokenKind::PP_CommentsStart},
                {"#ce", OpenAutoIt::TokenKind::PP_CommentsEnd},
                {"#include", OpenAutoIt::TokenKind::PP_Include},
                {"#include-once", OpenAutoIt::TokenKind::PP_IncludeOnce},
                {"#notrayicon", OpenAutoIt::TokenKind::PP_NoTrayIcon},
                {"#onautoitstartregister", OpenAutoIt::TokenKind::PP_OnAutoItStartRegister},
                {"#pragma", OpenAutoIt::TokenKind::PP_Pragma},
                {"#requireadmin", OpenAutoIt::TokenKind::PP_RequireAdmin},
        }};

[[nodiscard]] OpenAutoIt::TokenKind lookup_pre_processor(phi::string_view token)
{
    static constexpr auto map =
            LookUpMap<phi::string_view, OpenAutoIt::TokenKind, PreProcessorValues.size()>(
                    PreProcessorValues, OpenAutoIt::TokenKind::NotAToken);

    std::string str{token.begin(), token.end()};
    std::transform(str.begin(), str.end(), str.begin(), phi::to_lower_case);

    return map.at(phi::string_view(str.c_str(), str.size()));
}

static constexpr std::array<std::pair<phi::string_view, OpenAutoIt::TokenKind>, 404u> BuiltInValues{
        {
                {"abs", OpenAutoIt::TokenKind::BI_Abs},
                {"acos", OpenAutoIt::TokenKind::BI_ACos},
                {"adlibregister", OpenAutoIt::TokenKind::BI_AdlibRegister},
                {"adlibunregister", OpenAutoIt::TokenKind::BI_AdlibUnRegister},
                {"asc", OpenAutoIt::TokenKind::BI_Asc},
                {"ascw", OpenAutoIt::TokenKind::BI_AscW},
                {"asin", OpenAutoIt::TokenKind::BI_ASin},
                {"assign", OpenAutoIt::TokenKind::BI_Assign},
                {"atan", OpenAutoIt::TokenKind::BI_ATan},
                {"autoitsetoption", OpenAutoIt::TokenKind::BI_AutoItSetOption},
                {"autoitwingettitle", OpenAutoIt::TokenKind::BI_AutoItWinGetTitle},
                {"autoitwinsettitle", OpenAutoIt::TokenKind::BI_AutoItWinSetTitle},
                {"beep", OpenAutoIt::TokenKind::BI_Beep},
                {"binary", OpenAutoIt::TokenKind::BI_Binary},
                {"binarylen", OpenAutoIt::TokenKind::BI_BinaryLen},
                {"binarymid", OpenAutoIt::TokenKind::BI_BinaryMid},
                {"binarytostring", OpenAutoIt::TokenKind::BI_BinaryToString},
                {"bitand", OpenAutoIt::TokenKind::BI_BitAND},
                {"bitnot", OpenAutoIt::TokenKind::BI_BitNOT},
                {"bitor", OpenAutoIt::TokenKind::BI_BitOR},
                {"bitrotate", OpenAutoIt::TokenKind::BI_BitRotate},
                {"bitshift", OpenAutoIt::TokenKind::BI_BitShift},
                {"bitxor", OpenAutoIt::TokenKind::BI_BitXOR},
                {"blockinput", OpenAutoIt::TokenKind::BI_BlockInput},
                {"break", OpenAutoIt::TokenKind::BI_Break},
                {"call", OpenAutoIt::TokenKind::BI_Call},
                {"cdtray", OpenAutoIt::TokenKind::BI_CDTray},
                {"ceiling", OpenAutoIt::TokenKind::BI_Ceiling},
                {"chr", OpenAutoIt::TokenKind::BI_Chr},
                {"chrw", OpenAutoIt::TokenKind::BI_ChrW},
                {"clipget", OpenAutoIt::TokenKind::BI_ClipGet},
                {"clipput", OpenAutoIt::TokenKind::BI_ClipPut},
                {"consoleread", OpenAutoIt::TokenKind::BI_ConsoleRead},
                {"consolewrite", OpenAutoIt::TokenKind::BI_ConsoleWrite},
                {"consolewriteerror", OpenAutoIt::TokenKind::BI_ConsoleWriteError},
                {"controlclick", OpenAutoIt::TokenKind::BI_ControlClick},
                {"controlcommand", OpenAutoIt::TokenKind::BI_ControlCommand},
                {"controldisable", OpenAutoIt::TokenKind::BI_ControlDisable},
                {"controlenable", OpenAutoIt::TokenKind::BI_ControlEnable},
                {"controlfocus", OpenAutoIt::TokenKind::BI_ControlFocus},
                {"controlgetfocus", OpenAutoIt::TokenKind::BI_ControlGetFocus},
                {"controlgethandle", OpenAutoIt::TokenKind::BI_ControlGetHandle},
                {"controlgetpos", OpenAutoIt::TokenKind::BI_ControlGetPos},
                {"controlgettext", OpenAutoIt::TokenKind::BI_ControlGetText},
                {"controlhide", OpenAutoIt::TokenKind::BI_ControlHide},
                {"controllistview", OpenAutoIt::TokenKind::BI_ControlListView},
                {"controlmove", OpenAutoIt::TokenKind::BI_ControlMove},
                {"controlsend", OpenAutoIt::TokenKind::BI_ControlSend},
                {"controlsettext", OpenAutoIt::TokenKind::BI_ControlSetText},
                {"controlshow", OpenAutoIt::TokenKind::BI_ControlShow},
                {"controltreeview", OpenAutoIt::TokenKind::BI_ControlTreeView},
                {"cos", OpenAutoIt::TokenKind::BI_Cos},
                {"dec", OpenAutoIt::TokenKind::BI_Dec},
                {"dircopy", OpenAutoIt::TokenKind::BI_DirCopy},
                {"dircreate", OpenAutoIt::TokenKind::BI_DirCreate},
                {"dirgetsize", OpenAutoIt::TokenKind::BI_DirGetSize},
                {"dirmove", OpenAutoIt::TokenKind::BI_DirMove},
                {"dirremove", OpenAutoIt::TokenKind::BI_DirRemove},
                {"dllcall", OpenAutoIt::TokenKind::BI_DllCall},
                {"dllcalladdress", OpenAutoIt::TokenKind::BI_DllCallAddress},
                {"dllcallbackfree", OpenAutoIt::TokenKind::BI_DllCallbackFree},
                {"dllcallbackgetptr", OpenAutoIt::TokenKind::BI_DllCallbackGetPtr},
                {"dllcallbackregister", OpenAutoIt::TokenKind::BI_DllCallbackRegister},
                {"dllclose", OpenAutoIt::TokenKind::BI_DllClose},
                {"dllopen", OpenAutoIt::TokenKind::BI_DllOpen},
                {"dllstructcreate", OpenAutoIt::TokenKind::BI_DllStructCreate},
                {"dllstructgetdata", OpenAutoIt::TokenKind::BI_DllStructGetData},
                {"dllstructgetptr", OpenAutoIt::TokenKind::BI_DllStructGetPtr},
                {"dllstructgetsize", OpenAutoIt::TokenKind::BI_DllStructGetSize},
                {"dllstructsetdata", OpenAutoIt::TokenKind::BI_DllStructSetData},
                {"drivegetdrive", OpenAutoIt::TokenKind::BI_DriveGetDrive},
                {"drivegetfilesystem", OpenAutoIt::TokenKind::BI_DriveGetFileSystem},
                {"drivegetlabel", OpenAutoIt::TokenKind::BI_DriveGetLabel},
                {"drivegetserial", OpenAutoIt::TokenKind::BI_DriveGetSerial},
                {"drivegettype", OpenAutoIt::TokenKind::BI_DriveGetType},
                {"drivemapadd", OpenAutoIt::TokenKind::BI_DriveMapAdd},
                {"drivemapdel", OpenAutoIt::TokenKind::BI_DriveMapDel},
                {"drivemapget", OpenAutoIt::TokenKind::BI_DriveMapGet},
                {"drivesetlabel", OpenAutoIt::TokenKind::BI_DriveSetLabel},
                {"drivespacefree", OpenAutoIt::TokenKind::BI_DriveSpaceFree},
                {"drivespacetotal", OpenAutoIt::TokenKind::BI_DriveSpaceTotal},
                {"drivestatus", OpenAutoIt::TokenKind::BI_DriveStatus},
                {"envget", OpenAutoIt::TokenKind::BI_EnvGet},
                {"envset", OpenAutoIt::TokenKind::BI_EnvSet},
                {"envupdate", OpenAutoIt::TokenKind::BI_EnvUpdate},
                {"eval", OpenAutoIt::TokenKind::BI_Eval},
                {"execute", OpenAutoIt::TokenKind::BI_Execute},
                {"exp", OpenAutoIt::TokenKind::BI_Exp},
                {"filechangedir", OpenAutoIt::TokenKind::BI_FileChangeDir},
                {"fileclose", OpenAutoIt::TokenKind::BI_FileClose},
                {"filecopy", OpenAutoIt::TokenKind::BI_FileCopy},
                {"filecreatentfslink", OpenAutoIt::TokenKind::BI_FileCreateNTFSLink},
                {"filecreateshortcut", OpenAutoIt::TokenKind::BI_FileCreateShortcut},
                {"filedelete", OpenAutoIt::TokenKind::BI_FileDelete},
                {"fileexists", OpenAutoIt::TokenKind::BI_FileExists},
                {"filefindfirstfile", OpenAutoIt::TokenKind::BI_FileFindFirstFile},
                {"filefindnextfile", OpenAutoIt::TokenKind::BI_FileFindNextFile},
                {"fileflush", OpenAutoIt::TokenKind::BI_FileFlush},
                {"filegetattrib", OpenAutoIt::TokenKind::BI_FileGetAttrib},
                {"filegetencoding", OpenAutoIt::TokenKind::BI_FileGetEncoding},
                {"filegetlongname", OpenAutoIt::TokenKind::BI_FileGetLongName},
                {"filegetpos", OpenAutoIt::TokenKind::BI_FileGetPos},
                {"filegetshortcut", OpenAutoIt::TokenKind::BI_FileGetShortcut},
                {"filegetshortname", OpenAutoIt::TokenKind::BI_FileGetShortName},
                {"filegetsize", OpenAutoIt::TokenKind::BI_FileGetSize},
                {"filegettime", OpenAutoIt::TokenKind::BI_FileGetTime},
                {"filegetversion", OpenAutoIt::TokenKind::BI_FileGetVersion},
                {"fileinstall", OpenAutoIt::TokenKind::BI_FileInstall},
                {"filemove", OpenAutoIt::TokenKind::BI_FileMove},
                {"fileopen", OpenAutoIt::TokenKind::BI_FileOpen},
                {"fileopendialog", OpenAutoIt::TokenKind::BI_FileOpenDialog},
                {"fileread", OpenAutoIt::TokenKind::BI_FileRead},
                {"filereadline", OpenAutoIt::TokenKind::BI_FileReadLine},
                {"filereadtoarray", OpenAutoIt::TokenKind::BI_FileReadToArray},
                {"filerecycle", OpenAutoIt::TokenKind::BI_FileRecycle},
                {"filerecycleempty", OpenAutoIt::TokenKind::BI_FileRecycleEmpty},
                {"filesavedialog", OpenAutoIt::TokenKind::BI_FileSaveDialog},
                {"fileselectfolder", OpenAutoIt::TokenKind::BI_FileSelectFolder},
                {"filesetattrib", OpenAutoIt::TokenKind::BI_FileSetAttrib},
                {"filesetend", OpenAutoIt::TokenKind::BI_FileSetEnd},
                {"filesetpos", OpenAutoIt::TokenKind::BI_FileSetPos},
                {"filesettime", OpenAutoIt::TokenKind::BI_FileSetTime},
                {"filewrite", OpenAutoIt::TokenKind::BI_FileWrite},
                {"filewriteline", OpenAutoIt::TokenKind::BI_FileWriteLine},
                {"floor", OpenAutoIt::TokenKind::BI_Floor},
                {"ftpsetproxy", OpenAutoIt::TokenKind::BI_FtpSetProxy},
                {"funcname", OpenAutoIt::TokenKind::BI_FuncName},
                {"guicreate", OpenAutoIt::TokenKind::BI_GUICreate},
                {"guictrlcreateavi", OpenAutoIt::TokenKind::BI_GUICtrlCreateAvi},
                {"guictrlcreatebutton", OpenAutoIt::TokenKind::BI_GUICtrlCreateButton},
                {"guictrlcreatecheckbox", OpenAutoIt::TokenKind::BI_GUICtrlCreateCheckbox},
                {"guictrlcreatecombo", OpenAutoIt::TokenKind::BI_GUICtrlCreateCombo},
                {"guictrlcreatecontextmenu", OpenAutoIt::TokenKind::BI_GUICtrlCreateContextMenu},
                {"guictrlcreatedate", OpenAutoIt::TokenKind::BI_GUICtrlCreateDate},
                {"guictrlcreatedummy", OpenAutoIt::TokenKind::BI_GUICtrlCreateDummy},
                {"guictrlcreateedit", OpenAutoIt::TokenKind::BI_GUICtrlCreateEdit},
                {"guictrlcreategraphic", OpenAutoIt::TokenKind::BI_GUICtrlCreateGraphic},
                {"guictrlcreategroup", OpenAutoIt::TokenKind::BI_GUICtrlCreateGroup},
                {"guictrlcreateicon", OpenAutoIt::TokenKind::BI_GUICtrlCreateIcon},
                {"guictrlcreateinput", OpenAutoIt::TokenKind::BI_GUICtrlCreateInput},
                {"guictrlcreatelabel", OpenAutoIt::TokenKind::BI_GUICtrlCreateLabel},
                {"guictrlcreatelist", OpenAutoIt::TokenKind::BI_GUICtrlCreateList},
                {"guictrlcreatelistview", OpenAutoIt::TokenKind::BI_GUICtrlCreateListView},
                {"guictrlcreatelistviewitem", OpenAutoIt::TokenKind::BI_GUICtrlCreateListViewItem},
                {"guictrlcreatemenu", OpenAutoIt::TokenKind::BI_GUICtrlCreateMenu},
                {"guictrlcreatemenuitem", OpenAutoIt::TokenKind::BI_GUICtrlCreateMenuItem},
                {"guictrlcreatemonthcal", OpenAutoIt::TokenKind::BI_GUICtrlCreateMonthCal},
                {"guictrlcreateobj", OpenAutoIt::TokenKind::BI_GUICtrlCreateObj},
                {"guictrlcreatepic", OpenAutoIt::TokenKind::BI_GUICtrlCreatePic},
                {"guictrlcreateprogress", OpenAutoIt::TokenKind::BI_GUICtrlCreateProgress},
                {"guictrlcreateradio", OpenAutoIt::TokenKind::BI_GUICtrlCreateRadio},
                {"guictrlcreateslider", OpenAutoIt::TokenKind::BI_GUICtrlCreateSlider},
                {"guictrlcreatetab", OpenAutoIt::TokenKind::BI_GUICtrlCreateTab},
                {"guictrlcreatetabitem", OpenAutoIt::TokenKind::BI_GUICtrlCreateTabItem},
                {"guictrlcreatetreeview", OpenAutoIt::TokenKind::BI_GUICtrlCreateTreeView},
                {"guictrlcreatetreeviewitem", OpenAutoIt::TokenKind::BI_GUICtrlCreateTreeViewItem},
                {"guictrlcreateupdown", OpenAutoIt::TokenKind::BI_GUICtrlCreateUpdown},
                {"guictrldelete", OpenAutoIt::TokenKind::BI_GUICtrlDelete},
                {"guictrlgethandle", OpenAutoIt::TokenKind::BI_GUICtrlGetHandle},
                {"guictrlgetstate", OpenAutoIt::TokenKind::BI_GUICtrlGetState},
                {"guictrlread", OpenAutoIt::TokenKind::BI_GUICtrlRead},
                {"guictrlrecvmsg", OpenAutoIt::TokenKind::BI_GUICtrlRecvMsg},
                {"guictrlregisterlistviewsort",
                 OpenAutoIt::TokenKind::BI_GUICtrlRegisterListViewSort},
                {"guictrlsendmsg", OpenAutoIt::TokenKind::BI_GUICtrlSendMsg},
                {"guictrlsendtodummy", OpenAutoIt::TokenKind::BI_GUICtrlSendToDummy},
                {"guictrlsetbkcolor", OpenAutoIt::TokenKind::BI_GUICtrlSetBkColor},
                {"guictrlsetcolor", OpenAutoIt::TokenKind::BI_GUICtrlSetColor},
                {"guictrlsetcursor", OpenAutoIt::TokenKind::BI_GUICtrlSetCursor},
                {"guictrlsetdata", OpenAutoIt::TokenKind::BI_GUICtrlSetData},
                {"guictrlsetdefbkcolor", OpenAutoIt::TokenKind::BI_GUICtrlSetDefBkColor},
                {"guictrlsetdefcolor", OpenAutoIt::TokenKind::BI_GUICtrlSetDefColor},
                {"guictrlsetfont", OpenAutoIt::TokenKind::BI_GUICtrlSetFont},
                {"guictrlsetgraphic", OpenAutoIt::TokenKind::BI_GUICtrlSetGraphic},
                {"guictrlsetimage", OpenAutoIt::TokenKind::BI_GUICtrlSetImage},
                {"guictrlsetlimit", OpenAutoIt::TokenKind::BI_GUICtrlSetLimit},
                {"guictrlsetonevent", OpenAutoIt::TokenKind::BI_GUICtrlSetOnEvent},
                {"guictrlsetpos", OpenAutoIt::TokenKind::BI_GUICtrlSetPos},
                {"guictrlsetresizing", OpenAutoIt::TokenKind::BI_GUICtrlSetResizing},
                {"guictrlsetstate", OpenAutoIt::TokenKind::BI_GUICtrlSetState},
                {"guictrlsetstyle", OpenAutoIt::TokenKind::BI_GUICtrlSetStyle},
                {"guictrlsettip", OpenAutoIt::TokenKind::BI_GUICtrlSetTip},
                {"guidelete", OpenAutoIt::TokenKind::BI_GUIDelete},
                {"guigetcursorinfo", OpenAutoIt::TokenKind::BI_GUIGetCursorInfo},
                {"guigetmsg", OpenAutoIt::TokenKind::BI_GUIGetMsg},
                {"guigetstyle", OpenAutoIt::TokenKind::BI_GUIGetStyle},
                {"guiregistermsg", OpenAutoIt::TokenKind::BI_GUIRegisterMsg},
                {"guisetaccelerators", OpenAutoIt::TokenKind::BI_GUISetAccelerators},
                {"guisetbkcolor", OpenAutoIt::TokenKind::BI_GUISetBkColor},
                {"guisetcoord", OpenAutoIt::TokenKind::BI_GUISetCoord},
                {"guisetcursor", OpenAutoIt::TokenKind::BI_GUISetCursor},
                {"guisetfont", OpenAutoIt::TokenKind::BI_GUISetFont},
                {"guisethelp", OpenAutoIt::TokenKind::BI_GUISetHelp},
                {"guiseticon", OpenAutoIt::TokenKind::BI_GUISetIcon},
                {"guisetonevent", OpenAutoIt::TokenKind::BI_GUISetOnEvent},
                {"guisetstate", OpenAutoIt::TokenKind::BI_GUISetState},
                {"guisetstyle", OpenAutoIt::TokenKind::BI_GUISetStyle},
                {"guistartgroup", OpenAutoIt::TokenKind::BI_GUIStartGroup},
                {"guiswitch", OpenAutoIt::TokenKind::BI_GUISwitch},
                {"hex", OpenAutoIt::TokenKind::BI_Hex},
                {"hotkeyset", OpenAutoIt::TokenKind::BI_HotKeySet},
                {"httpsetproxy", OpenAutoIt::TokenKind::BI_HttpSetProxy},
                {"httpsetuseragent", OpenAutoIt::TokenKind::BI_HttpSetUserAgent},
                {"hwnd", OpenAutoIt::TokenKind::BI_HWnd},
                {"inetclose", OpenAutoIt::TokenKind::BI_InetClose},
                {"inetget", OpenAutoIt::TokenKind::BI_InetGet},
                {"inetgetinfo", OpenAutoIt::TokenKind::BI_InetGetInfo},
                {"inetgetsize", OpenAutoIt::TokenKind::BI_InetGetSize},
                {"inetread", OpenAutoIt::TokenKind::BI_InetRead},
                {"inidelete", OpenAutoIt::TokenKind::BI_IniDelete},
                {"iniread", OpenAutoIt::TokenKind::BI_IniRead},
                {"inireadsection", OpenAutoIt::TokenKind::BI_IniReadSection},
                {"inireadsectionnames", OpenAutoIt::TokenKind::BI_IniReadSectionNames},
                {"inirenamesection", OpenAutoIt::TokenKind::BI_IniRenameSection},
                {"iniwrite", OpenAutoIt::TokenKind::BI_IniWrite},
                {"iniwritesection", OpenAutoIt::TokenKind::BI_IniWriteSection},
                {"inputbox", OpenAutoIt::TokenKind::BI_InputBox},
                {"int", OpenAutoIt::TokenKind::BI_Int},
                {"isadmin", OpenAutoIt::TokenKind::BI_IsAdmin},
                {"isarray", OpenAutoIt::TokenKind::BI_IsArray},
                {"isbinary", OpenAutoIt::TokenKind::BI_IsBinary},
                {"isbool", OpenAutoIt::TokenKind::BI_IsBool},
                {"isdeclared", OpenAutoIt::TokenKind::BI_IsDeclared},
                {"isdllstruct", OpenAutoIt::TokenKind::BI_IsDllStruct},
                {"isfloat", OpenAutoIt::TokenKind::BI_IsFloat},
                {"isfunc", OpenAutoIt::TokenKind::BI_IsFunc},
                {"ishwnd", OpenAutoIt::TokenKind::BI_IsHWnd},
                {"isint", OpenAutoIt::TokenKind::BI_IsInt},
                {"iskeyword", OpenAutoIt::TokenKind::BI_IsKeyword},
                {"ismap", OpenAutoIt::TokenKind::BI_IsMap},
                {"isnumber", OpenAutoIt::TokenKind::BI_IsNumber},
                {"isobj", OpenAutoIt::TokenKind::BI_IsObj},
                {"isptr", OpenAutoIt::TokenKind::BI_IsPtr},
                {"isstring", OpenAutoIt::TokenKind::BI_IsString},
                {"log", OpenAutoIt::TokenKind::BI_Log},
                {"mapappend", OpenAutoIt::TokenKind::BI_MapAppend},
                {"mapexists", OpenAutoIt::TokenKind::BI_MapExists},
                {"mapkeys", OpenAutoIt::TokenKind::BI_MapKeys},
                {"mapremove", OpenAutoIt::TokenKind::BI_MapRemove},
                {"memgetstats", OpenAutoIt::TokenKind::BI_MemGetStats},
                {"mod", OpenAutoIt::TokenKind::BI_Mod},
                {"mouseclick", OpenAutoIt::TokenKind::BI_MouseClick},
                {"mouseclickdrag", OpenAutoIt::TokenKind::BI_MouseClickDrag},
                {"mousedown", OpenAutoIt::TokenKind::BI_MouseDown},
                {"mousegetcursor", OpenAutoIt::TokenKind::BI_MouseGetCursor},
                {"mousegetpos", OpenAutoIt::TokenKind::BI_MouseGetPos},
                {"mousemove", OpenAutoIt::TokenKind::BI_MouseMove},
                {"mouseup", OpenAutoIt::TokenKind::BI_MouseUp},
                {"mousewheel", OpenAutoIt::TokenKind::BI_MouseWheel},
                {"msgbox", OpenAutoIt::TokenKind::BI_MsgBox},
                {"number", OpenAutoIt::TokenKind::BI_Number},
                {"objcreate", OpenAutoIt::TokenKind::BI_ObjCreate},
                {"objcreateinterface", OpenAutoIt::TokenKind::BI_ObjCreateInterface},
                {"objevent", OpenAutoIt::TokenKind::BI_ObjEvent},
                {"objget", OpenAutoIt::TokenKind::BI_ObjGet},
                {"objname", OpenAutoIt::TokenKind::BI_ObjName},
                {"onautoitexitregister", OpenAutoIt::TokenKind::BI_OnAutoItExitRegister},
                {"onautoitexitunregister", OpenAutoIt::TokenKind::BI_OnAutoItExitUnRegister},
                {"ping", OpenAutoIt::TokenKind::BI_Ping},
                {"pixelchecksum", OpenAutoIt::TokenKind::BI_PixelChecksum},
                {"pixelgetcolor", OpenAutoIt::TokenKind::BI_PixelGetColor},
                {"pixelsearch", OpenAutoIt::TokenKind::BI_PixelSearch},
                {"processclose", OpenAutoIt::TokenKind::BI_ProcessClose},
                {"processexists", OpenAutoIt::TokenKind::BI_ProcessExists},
                {"processgetstats", OpenAutoIt::TokenKind::BI_ProcessGetStats},
                {"processlist", OpenAutoIt::TokenKind::BI_ProcessList},
                {"processsetpriority", OpenAutoIt::TokenKind::BI_ProcessSetPriority},
                {"processwait", OpenAutoIt::TokenKind::BI_ProcessWait},
                {"processwaitclose", OpenAutoIt::TokenKind::BI_ProcessWaitClose},
                {"progressoff", OpenAutoIt::TokenKind::BI_ProgressOff},
                {"progresson", OpenAutoIt::TokenKind::BI_ProgressOn},
                {"progressset", OpenAutoIt::TokenKind::BI_ProgressSet},
                {"ptr", OpenAutoIt::TokenKind::BI_Ptr},
                {"random", OpenAutoIt::TokenKind::BI_Random},
                {"regdelete", OpenAutoIt::TokenKind::BI_RegDelete},
                {"regenumkey", OpenAutoIt::TokenKind::BI_RegEnumKey},
                {"regenumval", OpenAutoIt::TokenKind::BI_RegEnumVal},
                {"regread", OpenAutoIt::TokenKind::BI_RegRead},
                {"regwrite", OpenAutoIt::TokenKind::BI_RegWrite},
                {"round", OpenAutoIt::TokenKind::BI_Round},
                {"run", OpenAutoIt::TokenKind::BI_Run},
                {"runas", OpenAutoIt::TokenKind::BI_RunAs},
                {"runaswait", OpenAutoIt::TokenKind::BI_RunAsWait},
                {"runwait", OpenAutoIt::TokenKind::BI_RunWait},
                {"send", OpenAutoIt::TokenKind::BI_Send},
                {"sendkeepactive", OpenAutoIt::TokenKind::BI_SendKeepActive},
                {"seterror", OpenAutoIt::TokenKind::BI_SetError},
                {"setextended", OpenAutoIt::TokenKind::BI_SetExtended},
                {"shellexecute", OpenAutoIt::TokenKind::BI_ShellExecute},
                {"shellexecutewait", OpenAutoIt::TokenKind::BI_ShellExecuteWait},
                {"shutdown", OpenAutoIt::TokenKind::BI_Shutdown},
                {"sin", OpenAutoIt::TokenKind::BI_Sin},
                {"sleep", OpenAutoIt::TokenKind::BI_Sleep},
                {"soundplay", OpenAutoIt::TokenKind::BI_SoundPlay},
                {"soundsetwavevolume", OpenAutoIt::TokenKind::BI_SoundSetWaveVolume},
                {"splashimageon", OpenAutoIt::TokenKind::BI_SplashImageOn},
                {"splashoff", OpenAutoIt::TokenKind::BI_SplashOff},
                {"splashtexton", OpenAutoIt::TokenKind::BI_SplashTextOn},
                {"sqrt", OpenAutoIt::TokenKind::BI_Sqrt},
                {"srandom", OpenAutoIt::TokenKind::BI_SRandom},
                {"statusbargettext", OpenAutoIt::TokenKind::BI_StatusbarGetText},
                {"stderrread", OpenAutoIt::TokenKind::BI_StderrRead},
                {"stdinwrite", OpenAutoIt::TokenKind::BI_StdinWrite},
                {"stdioclose", OpenAutoIt::TokenKind::BI_StdioClose},
                {"stdoutread", OpenAutoIt::TokenKind::BI_StdoutRead},
                {"string", OpenAutoIt::TokenKind::BI_String},
                {"stringaddcr", OpenAutoIt::TokenKind::BI_StringAddCR},
                {"stringcompare", OpenAutoIt::TokenKind::BI_StringCompare},
                {"stringformat", OpenAutoIt::TokenKind::BI_StringFormat},
                {"stringfromasciiarray", OpenAutoIt::TokenKind::BI_StringFromASCIIArray},
                {"stringinstr", OpenAutoIt::TokenKind::BI_StringInStr},
                {"stringisalnum", OpenAutoIt::TokenKind::BI_StringIsAlNum},
                {"stringisalpha", OpenAutoIt::TokenKind::BI_StringIsAlpha},
                {"stringisascii", OpenAutoIt::TokenKind::BI_StringIsASCII},
                {"stringisdigit", OpenAutoIt::TokenKind::BI_StringIsDigit},
                {"stringisfloat", OpenAutoIt::TokenKind::BI_StringIsFloat},
                {"stringisint", OpenAutoIt::TokenKind::BI_StringIsInt},
                {"stringislower", OpenAutoIt::TokenKind::BI_StringIsLower},
                {"stringisspace", OpenAutoIt::TokenKind::BI_StringIsSpace},
                {"stringisupper", OpenAutoIt::TokenKind::BI_StringIsUpper},
                {"stringisxdigit", OpenAutoIt::TokenKind::BI_StringIsXDigit},
                {"stringleft", OpenAutoIt::TokenKind::BI_StringLeft},
                {"stringlen", OpenAutoIt::TokenKind::BI_StringLen},
                {"stringlower", OpenAutoIt::TokenKind::BI_StringLower},
                {"stringmid", OpenAutoIt::TokenKind::BI_StringMid},
                {"stringregexp", OpenAutoIt::TokenKind::BI_StringRegExp},
                {"stringregexpreplace", OpenAutoIt::TokenKind::BI_StringRegExpReplace},
                {"stringreplace", OpenAutoIt::TokenKind::BI_StringReplace},
                {"stringreverse", OpenAutoIt::TokenKind::BI_StringReverse},
                {"stringright", OpenAutoIt::TokenKind::BI_StringRight},
                {"stringsplit", OpenAutoIt::TokenKind::BI_StringSplit},
                {"stringstripcr", OpenAutoIt::TokenKind::BI_StringStripCR},
                {"stringstripws", OpenAutoIt::TokenKind::BI_StringStripWS},
                {"stringtoasciiarray", OpenAutoIt::TokenKind::BI_StringToASCIIArray},
                {"stringtobinary", OpenAutoIt::TokenKind::BI_StringToBinary},
                {"stringtrimleft", OpenAutoIt::TokenKind::BI_StringTrimLeft},
                {"stringtrimright", OpenAutoIt::TokenKind::BI_StringTrimRight},
                {"stringupper", OpenAutoIt::TokenKind::BI_StringUpper},
                {"tan", OpenAutoIt::TokenKind::BI_Tan},
                {"tcpaccept", OpenAutoIt::TokenKind::BI_TCPAccept},
                {"tcpclosesocket", OpenAutoIt::TokenKind::BI_TCPCloseSocket},
                {"tcpconnect", OpenAutoIt::TokenKind::BI_TCPConnect},
                {"tcplisten", OpenAutoIt::TokenKind::BI_TCPListen},
                {"tcpnametoip", OpenAutoIt::TokenKind::BI_TCPNameToIP},
                {"tcprecv", OpenAutoIt::TokenKind::BI_TCPRecv},
                {"tcpsend", OpenAutoIt::TokenKind::BI_TCPSend},
                {"tcpshutdown", OpenAutoIt::TokenKind::BI_TCPShutdown},
                {"tcpstartup", OpenAutoIt::TokenKind::BI_UDPShutdown},
                {"timerdiff", OpenAutoIt::TokenKind::BI_TCPStartup},
                {"timerinit", OpenAutoIt::TokenKind::BI_UDPStartup},
                {"tooltip", OpenAutoIt::TokenKind::BI_TimerDiff},
                {"traycreateitem", OpenAutoIt::TokenKind::BI_TimerInit},
                {"traycreatemenu", OpenAutoIt::TokenKind::BI_ToolTip},
                {"traygetmsg", OpenAutoIt::TokenKind::BI_TrayCreateItem},
                {"trayitemdelete", OpenAutoIt::TokenKind::BI_TrayCreateMenu},
                {"trayitemgethandle", OpenAutoIt::TokenKind::BI_TrayGetMsg},
                {"trayitemgetstate", OpenAutoIt::TokenKind::BI_TrayItemDelete},
                {"trayitemgettext", OpenAutoIt::TokenKind::BI_TrayItemGetHandle},
                {"trayitemsetonevent", OpenAutoIt::TokenKind::BI_TrayItemGetState},
                {"trayitemsetstate", OpenAutoIt::TokenKind::BI_TrayItemGetText},
                {"trayitemsettext", OpenAutoIt::TokenKind::BI_TrayItemSetOnEvent},
                {"traysetclick", OpenAutoIt::TokenKind::BI_TrayItemSetState},
                {"trayseticon", OpenAutoIt::TokenKind::BI_TrayItemSetText},
                {"traysetonevent", OpenAutoIt::TokenKind::BI_TraySetClick},
                {"traysetpauseicon", OpenAutoIt::TokenKind::BI_TraySetIcon},
                {"traysetstate", OpenAutoIt::TokenKind::BI_TraySetOnEvent},
                {"traysettooltip", OpenAutoIt::TokenKind::BI_TraySetPauseIcon},
                {"traytip", OpenAutoIt::TokenKind::BI_TraySetState},
                {"ubound", OpenAutoIt::TokenKind::BI_TraySetToolTip},
                {"udpbind", OpenAutoIt::TokenKind::BI_TrayTip},
                {"udpclosesocket", OpenAutoIt::TokenKind::BI_UBound},
                {"udpopen", OpenAutoIt::TokenKind::BI_UDPBind},
                {"udprecv", OpenAutoIt::TokenKind::BI_UDPCloseSocket},
                {"udpsend", OpenAutoIt::TokenKind::BI_UDPOpen},
                {"udpshutdown", OpenAutoIt::TokenKind::BI_UDPRecv},
                {"udpstartup", OpenAutoIt::TokenKind::BI_UDPSend},
                {"vargettype", OpenAutoIt::TokenKind::BI_VarGetType},
                {"winactivate", OpenAutoIt::TokenKind::BI_WinActivate},
                {"winactive", OpenAutoIt::TokenKind::BI_WinActive},
                {"winclose", OpenAutoIt::TokenKind::BI_WinClose},
                {"winexists", OpenAutoIt::TokenKind::BI_WinExists},
                {"winflash", OpenAutoIt::TokenKind::BI_WinFlash},
                {"wingetcaretpos", OpenAutoIt::TokenKind::BI_WinGetCaretPos},
                {"wingetclasslist", OpenAutoIt::TokenKind::BI_WinGetClassList},
                {"wingetclientsize", OpenAutoIt::TokenKind::BI_WinGetClientSize},
                {"wingethandle", OpenAutoIt::TokenKind::BI_WinGetHandle},
                {"wingetpos", OpenAutoIt::TokenKind::BI_WinGetPos},
                {"wingetprocess", OpenAutoIt::TokenKind::BI_WinGetProcess},
                {"wingetstate", OpenAutoIt::TokenKind::BI_WinGetState},
                {"wingettext", OpenAutoIt::TokenKind::BI_WinGetText},
                {"wingettitle", OpenAutoIt::TokenKind::BI_WinGetTitle},
                {"winkill", OpenAutoIt::TokenKind::BI_WinKill},
                {"winlist", OpenAutoIt::TokenKind::BI_WinList},
                {"winmenuselectitem", OpenAutoIt::TokenKind::BI_WinMenuSelectItem},
                {"winminimizeall", OpenAutoIt::TokenKind::BI_WinMinimizeAll},
                {"winminimizeallundo", OpenAutoIt::TokenKind::BI_WinMinimizeAllUndo},
                {"winmove", OpenAutoIt::TokenKind::BI_WinMove},
                {"winsetontop", OpenAutoIt::TokenKind::BI_WinSetOnTop},
                {"winsetstate", OpenAutoIt::TokenKind::BI_WinSetState},
                {"winsettitle", OpenAutoIt::TokenKind::BI_WinSetTitle},
                {"winsettrans", OpenAutoIt::TokenKind::BI_WinSetTrans},
                {"winwait", OpenAutoIt::TokenKind::BI_WinWait},
                {"winwaitactive", OpenAutoIt::TokenKind::BI_WinWaitClose},
                {"winwaitclose", OpenAutoIt::TokenKind::BI_WinWaitActive},
                {"winwaitnotactive", OpenAutoIt::TokenKind::BI_WinWaitNotActive},
        }};

[[nodiscard]] PHI_ATTRIBUTE_PURE OpenAutoIt::TokenKind lookup_builtin(phi::string_view token)
{
    static constexpr auto map =
            LookUpMap<phi::string_view, OpenAutoIt::TokenKind, BuiltInValues.size()>(
                    BuiltInValues, OpenAutoIt::TokenKind::NotAToken);

    return map.at(token);
}

static constexpr std::array<std::pair<phi::string_view, OpenAutoIt::TokenKind>, 44u> KeyWordsValues{
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
         {"byref", OpenAutoIt::TokenKind::KW_ByRef},
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

[[nodiscard]] OpenAutoIt::TokenKind lookup_identifier(phi::string_view token)
{
    static constexpr auto keyword_map =
            LookUpMap<phi::string_view, OpenAutoIt::TokenKind, KeyWordsValues.size()>(
                    KeyWordsValues, OpenAutoIt::TokenKind::FunctionIdentifier);
    static constexpr auto builtin_map =
            LookUpMap<phi::string_view, OpenAutoIt::TokenKind, BuiltInValues.size()>(
                    BuiltInValues, OpenAutoIt::TokenKind::FunctionIdentifier);

    std::string str{token.begin(), token.end()};
    std::transform(str.begin(), str.end(), str.begin(), phi::to_lower_case);

    phi::string_view string = phi::string_view(str.c_str(), str.size());

    const OpenAutoIt::TokenKind keyword = keyword_map.at(string);
    if (keyword == OpenAutoIt::TokenKind::FunctionIdentifier)
    {
        return builtin_map.at(string);
    }

    return keyword;
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

[[nodiscard]] PHI_ATTRIBUTE_PURE OpenAutoIt::TokenKind lookup_operator(phi::string_view token)
{
    static constexpr auto map =
            LookUpMap<phi::string_view, OpenAutoIt::TokenKind, OperatorValues.size()>(
                    OperatorValues, OpenAutoIt::TokenKind::NotAToken);

    return map.at(token);
}

[[nodiscard]] constexpr phi::boolean is_skip_character(const char c)
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

[[nodiscard]] constexpr phi::boolean is_valid_identifier_char(const char c)
{
    return phi::is_alpha_numeric(c) || c == '_';
}

[[nodiscard]] constexpr phi::boolean is_valid_pp_char(const char c)
{
    return phi::is_alpha_numeric(c) || c == '-';
}

[[nodiscard]] constexpr phi::boolean is_two_part_operator(const char c)
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

[[nodiscard]] constexpr phi::boolean is_single_operator(const char c)
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

Lexer::Lexer()
{}

void Lexer::Reset()
{
    m_Iterator = m_Source.begin();

    m_InsideMultiLineComment = false;

    m_LineNumber = 1u;
    m_Column     = 1u;
}

PHI_ATTRIBUTE_PURE phi::boolean Lexer::IsFinished() const
{
    return m_Iterator == m_Source.end();
}

phi::optional<Token> Lexer::GetNextToken()
{
    while (!IsFinished())
    {
        char current_character = *m_Iterator;

        /* Embedded null character */

        if (current_character == '\0')
        {
            // TODO: Embedded null character warning

            SkipCurrentCharacter();
        }

        /* Multiline comments */

        else if (m_InsideMultiLineComment)
        {
            iterator       begin_of_multiline_comment            = m_Iterator;
            const phi::u64 beginning_line_of_multiline_comment   = m_LineNumber;
            const phi::u64 beginning_column_of_multiline_comment = m_Column;

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

                    const TokenKind pre_processor_token_kind =
                            lookup_pre_processor(TokenText(begin_of_token));

                    if (pre_processor_token_kind == TokenKind::PP_CommentsEnd)
                    {
                        m_InsideMultiLineComment = false;

                        // Go back the size of the parsed end token so we can reparse it in the normal pre processor parser
                        m_Iterator -= TokenText(begin_of_token).length().unsafe();

                        Token token{TokenKind::Comment, TokenText(begin_of_multiline_comment),
                                    BuildSourceLocation(beginning_line_of_multiline_comment,
                                                        beginning_column_of_multiline_comment)};

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
            iterator     begin_of_token = m_Iterator;
            phi::boolean parsed_something{false};
            ConsumeCurrentCharacter();

            while (!IsFinished())
            {
                current_character = *m_Iterator;

                if (is_valid_identifier_char(current_character))
                {
                    ConsumeCurrentCharacter();
                    parsed_something = true;
                    continue;
                }

                break;
            }

            // Ensure that '$' is not a valid variable identifier
            if (!parsed_something)
            {
                return ConstructToken(TokenKind::Garbage, begin_of_token);
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
            const TokenKind pre_processor_token_kind =
                    lookup_pre_processor(TokenText(begin_of_token));

            if (pre_processor_token_kind == TokenKind::PP_CommentsStart)
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

        /* Number Literals - IntegerLiteral/FloatLiteral */

        else if (phi::is_digit(current_character) || current_character == '.')
        {
            const phi::boolean start_with_zero{current_character == '0'};
            phi::boolean       parsing_hex{false};
            phi::boolean       parsing_float{current_character == '.'};

            iterator begin_of_token = m_Iterator;
            ConsumeCurrentCharacter();

            while (!IsFinished())
            {
                current_character = *m_Iterator;

                // Is the second character
                if (m_Iterator - begin_of_token == 1u && start_with_zero)
                {
                    // Hex character
                    if (current_character == 'x' || current_character == 'X')
                    {
                        parsing_hex = true;
                        ConsumeCurrentCharacter();
                        continue;
                    }
                }

                // Actually parsing
                if (parsing_hex)
                {
                    if (parsing_float)
                    {
                        // TODO: Error hexliteral not allowed for floats
                        return ConstructToken(TokenKind::Garbage, begin_of_token);
                    }

                    if (phi::is_hex_digit(current_character))
                    {
                        ConsumeCurrentCharacter();
                        continue;
                    }
                }
                else if (phi::is_digit(current_character))
                {
                    ConsumeCurrentCharacter();
                    continue;
                }
                // Literal dot
                else if (current_character == '.')
                {
                    if (parsing_float)
                    {
                        // TODO: Error more than one dot in float literal
                        return ConstructToken(TokenKind::Garbage, begin_of_token);
                    }

                    parsing_float = true;
                    ConsumeCurrentCharacter();
                    continue;
                }

                break;
            }

            if (parsing_float)
            {
                // Were not allowed to end with a dot
                // TODO: This is very hacky and looks nasty
                if (*(m_Iterator - 1) == '.')
                {
                    // TODO: Prober error
                    return ConstructToken(TokenKind::Garbage, begin_of_token);
                }

                return ConstructToken(TokenKind::FloatLiteral, begin_of_token);
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
            Token token = ConstructToken(lookup_operator({m_Iterator, 1u}));

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

TokenStream Lexer::ProcessString(phi::string_view file_name, phi::string_view source)
{
    SourceFile fake_source{SourceFile::Type::Basic, file_name.data(), source};

    return ProcessFile(&fake_source);
}

TokenStream Lexer::ProcessFile(phi::observer_ptr<const SourceFile> source_file)
{
    TokenStream stream;

    m_SourceFile = source_file;
    m_Source     = m_SourceFile->m_Content;
    Reset();

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

void Lexer::ConsumeCurrentCharacter()
{
    ++m_Iterator;
}

void Lexer::AdvanceToNextLine()
{
    ++m_LineNumber;
    m_Column = 1u;
}

void Lexer::SkipCurrentCharacter()
{
    ConsumeCurrentCharacter();
    ++m_Column;
}
} // namespace OpenAutoIt
