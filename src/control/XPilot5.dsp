# Microsoft Developer Studio Project File - Name="XPilot5" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=XPilot5 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "XPilot5.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "XPilot5.mak" CFG="XPilot5 - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "XPilot5 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "XPilot5 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XPilot5 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\common" /I "..\common\fltk-widgets" /I "..\common\xp-flwidgets" /I "..\serverlib" /I "..\common\xp-flwidgets\kbconfig" /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_XPILOTEDIT_" /D "FLTK" /D "SOUND" /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 alut.lib openal32.lib fltk.lib fltkimages.lib fltkpng.lib fltkz.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "XPilot5 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\common" /I "..\common\fltk-widgets" /I "..\common\xp-flwidgets" /I "..\serverlib" /I "..\common\xp-flwidgets\kbconfig" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_XPILOTEDIT_" /D "FLTK" /D "SOUND" /FR /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 alut.lib openal32.lib fltkd.lib fltkimagesd.lib fltkpngd.lib fltkzd.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF

# Begin Target

# Name "XPilot5 - Win32 Release"
# Name "XPilot5 - Win32 Debug"
# Begin Group "control"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\.cvsignore
# End Source File
# Begin Source File

SOURCE=.\clipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\controlconst.h
# End Source File
# Begin Source File

SOURCE=.\default_colors.h
# End Source File
# Begin Source File

SOURCE=.\EditorsPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\EditorsPanel.h
# End Source File
# Begin Source File

SOURCE=.\errors.cpp
# End Source File
# Begin Source File

SOURCE=.\gui_toolkit.cpp
# End Source File
# Begin Source File

SOURCE=.\InetServerConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\InetServerConfig.h
# End Source File
# Begin Source File

SOURCE=.\InetServerList.cpp
# End Source File
# Begin Source File

SOURCE=.\InetServerList.h
# End Source File
# Begin Source File

SOURCE=.\InetServerPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\InetServerPanel.h
# End Source File
# Begin Source File

SOURCE=.\IniXPilot.cpp
# End Source File
# Begin Source File

SOURCE=.\IniXPilot.Defaults.cpp
# End Source File
# Begin Source File

SOURCE=.\IniXPilot.h
# End Source File
# Begin Source File

SOURCE=.\LocalServerControlClient.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalServerControlClient.h
# End Source File
# Begin Source File

SOURCE=.\LocalServerPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalServerPanel.h
# End Source File
# Begin Source File

SOURCE=.\LocalServerPanelAdvanced.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalServerPanelAdvanced.h
# End Source File
# Begin Source File

SOURCE=.\LocalServerPanelSimple.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalServerPanelSimple.h
# End Source File
# Begin Source File

SOURCE=.\LocalServerPanelWorld.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalServerPanelWorld.h
# End Source File
# Begin Source File

SOURCE=.\MainButtonsPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\MainButtonsPanel.h
# End Source File
# Begin Source File

SOURCE=.\Makefile.in
# End Source File
# Begin Source File

SOURCE=.\MapBlockChange.cpp
# End Source File
# Begin Source File

SOURCE=.\MapBlockChange.h
# End Source File
# Begin Source File

SOURCE=.\MapDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\MapDocument.h
# End Source File
# Begin Source File

SOURCE=.\MapEditWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MapEditWindow.h
# End Source File
# Begin Source File

SOURCE=.\MapUndoableCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\MapUndoableCommand.h
# End Source File
# Begin Source File

SOURCE=.\MapWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MapWindow.h
# End Source File
# Begin Source File

SOURCE=.\PrivatePanelEditButton.cpp
# End Source File
# Begin Source File

SOURCE=.\PrivatePanelEditButton.h
# End Source File
# Begin Source File

SOURCE=.\PrivatePanelEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\PrivatePanelEditor.h
# End Source File
# Begin Source File

SOURCE=.\PrivateServerConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\PrivateServerConfig.h
# End Source File
# Begin Source File

SOURCE=.\PrivateServerList.cpp
# End Source File
# Begin Source File

SOURCE=.\PrivateServerList.h
# End Source File
# Begin Source File

SOURCE=.\PrivateServerPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\PrivateServerPanel.h
# End Source File
# Begin Source File

SOURCE=.\proto.h
# End Source File
# Begin Source File

SOURCE=.\ScoreServerControlClient.cpp
# End Source File
# Begin Source File

SOURCE=.\ScoreServerControlClient.h
# End Source File
# Begin Source File

SOURCE=.\ServerButtonsGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerButtonsGroup.h
# End Source File
# Begin Source File

SOURCE=.\ServerOptionsToImage.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerOptionsToImage.h
# End Source File
# Begin Source File

SOURCE=.\ShipEditWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ShipEditWindow.h
# End Source File
# Begin Source File

SOURCE=.\ShipUndoableCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\ShipUndoableCommand.h
# End Source File
# Begin Source File

SOURCE=.\ShipWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ShipWindow.h
# End Source File
# Begin Source File

SOURCE=.\SplashPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\SplashPanel.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\XPClientConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\XPClientConfig.h
# End Source File
# Begin Source File

SOURCE=.\XPFileChooser.cpp
# End Source File
# Begin Source File

SOURCE=.\XPFileChooser.fl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\XPFileChooser.h
# End Source File
# Begin Source File

SOURCE=.\XPFileChooser2.cpp
# End Source File
# Begin Source File

SOURCE=.\XPilotControl.cpp
# End Source File
# Begin Source File

SOURCE=.\XPilotControl.h
# End Source File
# Begin Source File

SOURCE=.\XPilotControlWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\XPilotControlWindow.h
# End Source File
# Begin Source File

SOURCE=.\XPInput.cpp
# End Source File
# Begin Source File

SOURCE=.\XPInput.h
# End Source File
# Begin Source File

SOURCE=.\XPPresetConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\XPPresetConfig.h
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\common\Archive.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Archive.h
# End Source File
# Begin Source File

SOURCE=..\common\AudioMan.cpp
# End Source File
# Begin Source File

SOURCE=..\common\AudioMan.h
# End Source File
# Begin Source File

SOURCE=..\common\bit.h
# End Source File
# Begin Source File

SOURCE=..\common\checknames.cpp
# End Source File
# Begin Source File

SOURCE=..\common\checknames.h
# End Source File
# Begin Source File

SOURCE=..\common\commonproto.h
# End Source File
# Begin Source File

SOURCE=..\common\config.cpp
# End Source File
# Begin Source File

SOURCE=..\common\config.h
# End Source File
# Begin Source File

SOURCE=..\common\Connectparam.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Connectparam.h
# End Source File
# Begin Source File

SOURCE=..\common\const.h
# End Source File
# Begin Source File

SOURCE=..\common\ControlClient.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ControlClient.h
# End Source File
# Begin Source File

SOURCE=..\common\cstring.cpp
# End Source File
# Begin Source File

SOURCE=..\common\cstring.h
# End Source File
# Begin Source File

SOURCE=..\common\error.cpp
# End Source File
# Begin Source File

SOURCE=..\common\fileUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\common\fileUtils.h
# End Source File
# Begin Source File

SOURCE=..\common\FirewallPortList.cpp
# End Source File
# Begin Source File

SOURCE=..\common\FirewallPortList.h
# End Source File
# Begin Source File

SOURCE=..\common\FlEventNames.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Ini.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Ini.h
# End Source File
# Begin Source File

SOURCE=..\common\Ini.Save.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Ini.Win.cpp
# End Source File
# Begin Source File

SOURCE=..\common\IniClient.cpp
# End Source File
# Begin Source File

SOURCE=..\common\IniClient.Defaults.cpp
# End Source File
# Begin Source File

SOURCE=..\common\IniClient.h
# End Source File
# Begin Source File

SOURCE=..\common\IniServer.cpp
# End Source File
# Begin Source File

SOURCE=..\common\IniServer.Defaults.cpp
# End Source File
# Begin Source File

SOURCE=..\common\IniServer.h
# End Source File
# Begin Source File

SOURCE=..\common\keys.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\common\map.h
# End Source File
# Begin Source File

SOURCE=..\common\microdelay.cpp
# End Source File
# Begin Source File

SOURCE=..\common\microdelay.h
# End Source File
# Begin Source File

SOURCE=..\common\Obj.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Obj.h
# End Source File
# Begin Source File

SOURCE=..\common\pack.h
# End Source File
# Begin Source File

SOURCE=..\common\packet.h
# End Source File
# Begin Source File

SOURCE=..\common\PacketCtl.h
# End Source File
# Begin Source File

SOURCE=..\common\password.cpp
# End Source File
# Begin Source File

SOURCE=..\common\password.h
# End Source File
# Begin Source File

SOURCE=..\common\portability.cpp
# End Source File
# Begin Source File

SOURCE=..\common\portability.h
# End Source File
# Begin Source File

SOURCE=..\common\randommt.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ServerList.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ServerList.h
# End Source File
# Begin Source File

SOURCE=..\common\ServerListCfg.h
# End Source File
# Begin Source File

SOURCE=..\common\ServerOptionsNetClient.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ServerOptionsNetClient.h
# End Source File
# Begin Source File

SOURCE=..\common\ServerOptionsNetClientFl.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ServerOptionsNetClientFl.h
# End Source File
# Begin Source File

SOURCE=..\common\ServerSite.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ServerSite.h
# End Source File
# Begin Source File

SOURCE=..\common\ShipDocument.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ShipDocument.h
# End Source File
# Begin Source File

SOURCE=..\common\ShipList.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ShipList.h
# End Source File
# Begin Source File

SOURCE=..\common\showtime.cpp
# End Source File
# Begin Source File

SOURCE=..\common\showtime.h
# End Source File
# Begin Source File

SOURCE=..\common\Sock.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Sock.h
# End Source File
# Begin Source File

SOURCE=..\common\Sockbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Sockbuf.h
# End Source File
# Begin Source File

SOURCE=..\common\sockerrs.cpp
# End Source File
# Begin Source File

SOURCE=..\common\sockerrs.h
# End Source File
# Begin Source File

SOURCE=..\common\Sound.cpp
# End Source File
# Begin Source File

SOURCE=..\common\Sound.h
# End Source File
# Begin Source File

SOURCE=..\common\SoundEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\common\SoundEvent.h
# End Source File
# Begin Source File

SOURCE=..\common\soundNames.cpp.h
# End Source File
# Begin Source File

SOURCE=..\common\strdup.cpp
# End Source File
# Begin Source File

SOURCE=..\common\strlcpy.cpp
# End Source File
# Begin Source File

SOURCE=..\common\types.h
# End Source File
# Begin Source File

SOURCE=..\common\UndoableCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\common\UndoableCommand.h
# End Source File
# Begin Source File

SOURCE=..\common\UndoMan.cpp
# End Source File
# Begin Source File

SOURCE=..\common\UndoMan.h
# End Source File
# Begin Source File

SOURCE=..\common\WildMap.cpp
# End Source File
# Begin Source File

SOURCE=..\common\WildMap.h
# End Source File
# Begin Source File

SOURCE=..\common\XpPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\common\XpPoint.h
# End Source File
# Begin Source File

SOURCE=..\common\XpPointList.cpp
# End Source File
# Begin Source File

SOURCE=..\common\XpPointList.h
# End Source File
# Begin Source File

SOURCE=..\common\xpprintf.cpp
# End Source File
# End Group
# Begin Group "fltk-widgets"

# PROP Default_Filter ""
# Begin Group "fltk.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Enumerations.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Bitmap.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Box.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Browser.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Browser_.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Button.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Check_Button.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Choice.H
# End Source File
# Begin Source File

SOURCE=".\fltk-widgets\FL\Fl_Dir.H"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Double_Window.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\fl_draw.H
# End Source File
# Begin Source File

SOURCE=".\fltk-widgets\FL\Fl_File_Dialog.H"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Group.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Input.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Input_.H
# End Source File
# Begin Source File

SOURCE=".\fltk-widgets\FL\Fl_Input_File.H"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Light_Button.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Menu_.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Menu_Bar.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Menu_Button.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Menu_Item.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Multi_Browser.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Multiline_Input.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Overlay_Window.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Pixmap.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Repeat_Button.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Return_Button.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Scrollbar.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Slider.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Tabs.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Toggle_Button.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Valuator.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Widget.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\Fl_Window.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\win32.H
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\..\sourceforge\fltk\FL\x.H
# End Source File
# End Group
# Begin Source File

SOURCE="..\common\fltk-widgets\.cvsignore"
# End Source File
# Begin Source File

SOURCE="..\common\fltk-widgets\src\Flv_List.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\fltk-widgets\FL\Flv_List.H"
# End Source File
# Begin Source File

SOURCE="..\common\fltk-widgets\src\Flv_Style.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\fltk-widgets\FL\Flv_Style.H"
# End Source File
# Begin Source File

SOURCE="..\common\fltk-widgets\src\Flv_Table.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\fltk-widgets\FL\Flv_Table.H"
# End Source File
# Begin Source File

SOURCE="..\common\fltk-widgets\Makefile.in"
# End Source File
# Begin Source File

SOURCE="..\common\fltk-widgets\readme.txt"
# End Source File
# End Group
# Begin Group "xp-flwidgets"

# PROP Default_Filter ""
# Begin Group "audio"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\common\xp-flwidgets\audio\Fl_EventSoundConnector.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\audio\Fl_EventSoundConnector.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\audio\Fl_MainGainSlider.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\audio\Fl_MainGainSlider.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\audio\Fl_SoundEventList.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\audio\Fl_SoundEventList.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\audio\Fl_SoundList.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\audio\Fl_SoundList.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\audio\SoundUndoableCommand.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\audio\SoundUndoableCommand.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\audio\SoundWindow.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\audio\SoundWindow.h"
# End Source File
# End Group
# Begin Source File

SOURCE="..\common\xp-flwidgets\.cvsignore"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\Fl_PrefsColorKey.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\Fl_PrefsColorKey.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\Fl_ServerList.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\Fl_ServerList.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\Fl_Status.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\Fl_Status.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\Fl_Value_Input_Counter.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\Fl_Value_Input_Counter.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\Fl_XPColors.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\Fl_XPColors.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\flUtil.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\flUtil.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\guiconst.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\Makefile.in"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\NameAndShipButton.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\NameAndShipButton.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\ServerPrefsWindow.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\ServerPrefsWindow.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\ShipDocument.FL.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\ShipListBox.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\ShipListBox.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\ShipListViewWindow.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\ShipListViewWindow.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\XPCheckButton.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\XPCheckButton.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\XPChoice.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\XPChoice.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\xpcTypes.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\XPPreset_Group.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\XPPreset_Group.h"
# End Source File
# End Group
# Begin Group "serverlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\serverlib\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\serverlib\File.cpp
# End Source File
# Begin Source File

SOURCE=..\serverlib\File.h
# End Source File
# Begin Source File

SOURCE=..\serverlib\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\serverlib\ServerOption.cpp
# End Source File
# Begin Source File

SOURCE=..\serverlib\ServerOption.h
# End Source File
# Begin Source File

SOURCE=..\serverlib\ServerOptions.cpp
# End Source File
# Begin Source File

SOURCE=..\serverlib\ServerOptions.h
# End Source File
# Begin Source File

SOURCE=..\serverlib\ServerOptions.init.cpp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\mainbuttonsFromPng.sh
# End Source File
# Begin Source File

SOURCE=.\res\res\xpilot.ico
# End Source File
# Begin Source File

SOURCE=.\res\XPilotControl.ico
# End Source File
# Begin Source File

SOURCE=.\res\XPilotControl.rc
# End Source File
# Begin Source File

SOURCE=.\res\xpwhere.ico
# End Source File
# End Group
# Begin Group "kbconfig"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\.cvsignore"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbActionButton.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbActionButton.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbCButton.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbCButton.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbConfig.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbConfig.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbConfigBuiltIn.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbConfigKb.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbConfigKb.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbDragger.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbDragger.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbKbSelector.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbKbSelector.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbMultiComm.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Fl_KbMultiComm.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\KbUndoableCommand.cpp"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\KbUndoableCommand.h"
# End Source File
# Begin Source File

SOURCE="..\common\xp-flwidgets\kbconfig\Makefile.in"
# End Source File
# End Group
# End Target
# End Project
