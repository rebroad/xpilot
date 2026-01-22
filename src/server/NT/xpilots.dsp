# Microsoft Developer Studio Project File - Name="XPilotServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=XPilotServer - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "xpilots.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "xpilots.mak" CFG="XPilotServer - Win32 Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "XPilotServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "XPilotServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XPilotServer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /I "..\..\common" /I "..\..\serverlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SOCKWINDOWS" /D "SOUND" /Fr /Yu"StdAfx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Ws2_32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /machine:I386 /out:".\Release\XPilotServer.exe"
# SUBTRACT LINK32 /debug /nodefaultlib

!ELSEIF  "$(CFG)" == "XPilotServer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\..\serverlib" /D "_DEBUG" /D "NOTRANDOM" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SOCKWINDOWS" /D "SOUND" /FR /Yu"StdAfx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 Ws2_32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386 /out:".\Debug\XPilotServer.exe"

!ENDIF

# Begin Target

# Name "XPilotServer - Win32 Release"
# Name "XPilotServer - Win32 Debug"
# Begin Group "server"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\Alliance.cpp
# End Source File
# Begin Source File

SOURCE=..\asteroid.cpp
# End Source File
# Begin Source File

SOURCE=..\asteroid.h
# End Source File
# Begin Source File

SOURCE=..\Cannon.cpp
# End Source File
# Begin Source File

SOURCE=..\Cannon.h
# End Source File
# Begin Source File

SOURCE=..\click.h
# End Source File
# Begin Source File

SOURCE=..\command.cpp
# End Source File
# Begin Source File

SOURCE=..\ConnectionControl.cpp
# End Source File
# Begin Source File

SOURCE=..\ConnectionControl.h
# End Source File
# Begin Source File

SOURCE=..\ConnectionControlLocalCtl.cpp
# End Source File
# Begin Source File

SOURCE=..\ConnectionControlLocalCtl.h
# End Source File
# Begin Source File

SOURCE=..\ConnectionControlScoreServer.cpp
# End Source File
# Begin Source File

SOURCE=..\ConnectionControlScoreServer.h
# End Source File
# Begin Source File

SOURCE=..\ConnectionPlayer.cpp
# End Source File
# Begin Source File

SOURCE=..\ConnectionPlayer.h
# End Source File
# Begin Source File

SOURCE=..\defaults.h
# End Source File
# Begin Source File

SOURCE=..\event.cpp
# End Source File
# Begin Source File

SOURCE=..\fileparser.cpp
# End Source File
# Begin Source File

SOURCE=..\Frame.cpp
# End Source File
# Begin Source File

SOURCE=..\Frame.h
# End Source File
# Begin Source File

SOURCE=..\global.h
# End Source File
# Begin Source File

SOURCE=..\id.cpp
# End Source File
# Begin Source File

SOURCE=..\item.cpp
# End Source File
# Begin Source File

SOURCE=..\laser.cpp
# End Source File
# Begin Source File

SOURCE=..\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\metaserver.cpp
# End Source File
# Begin Source File

SOURCE=..\metaserver.h
# End Source File
# Begin Source File

SOURCE=..\NetServerServer.h
# End Source File
# Begin Source File

SOURCE=..\Object.cpp
# End Source File
# Begin Source File

SOURCE=..\Object.h
# End Source File
# Begin Source File

SOURCE=..\objpos.cpp
# End Source File
# Begin Source File

SOURCE=..\objpos.h
# End Source File
# Begin Source File

SOURCE=..\option.cpp
# End Source File
# Begin Source File

SOURCE=..\Player.cpp
# End Source File
# Begin Source File

SOURCE=..\Player.h
# End Source File
# Begin Source File

SOURCE=..\proto.h
# End Source File
# Begin Source File

SOURCE=..\Robot.cpp
# End Source File
# Begin Source File

SOURCE=..\Robot.h
# End Source File
# Begin Source File

SOURCE=..\robotdef.cpp
# End Source File
# Begin Source File

SOURCE=..\saudio.cpp
# End Source File
# Begin Source File

SOURCE=..\saudio.h
# End Source File
# Begin Source File

SOURCE=..\score.cpp
# End Source File
# Begin Source File

SOURCE=..\score.h
# End Source File
# Begin Source File

SOURCE=..\server.h
# End Source File
# Begin Source File

SOURCE=..\ServerOptionWorld.cpp
# End Source File
# Begin Source File

SOURCE=..\ServerOptionWorld.h
# End Source File
# Begin Source File

SOURCE=..\ship.cpp
# End Source File
# Begin Source File

SOURCE=..\shot.cpp
# End Source File
# Begin Source File

SOURCE=..\update.cpp
# End Source File
# Begin Source File

SOURCE=..\walls.h
# End Source File
# Begin Source File

SOURCE=..\World.cell.cpp
# End Source File
# Begin Source File

SOURCE=..\World.collision.cpp
# End Source File
# Begin Source File

SOURCE=..\World.contact.cpp
# End Source File
# Begin Source File

SOURCE=..\World.cpp
# End Source File
# Begin Source File

SOURCE=..\World.h
# End Source File
# Begin Source File

SOURCE=..\World.main.cpp
# End Source File
# Begin Source File

SOURCE=..\World.netserver.cpp
# End Source File
# Begin Source File

SOURCE=..\World.parser.cpp
# End Source File
# Begin Source File

SOURCE=..\World.play.cpp
# End Source File
# Begin Source File

SOURCE=..\World.rules.cpp
# End Source File
# Begin Source File

SOURCE=..\World.ScoreServer.cpp
# End Source File
# Begin Source File

SOURCE=..\World.tag.cpp
# End Source File
# Begin Source File

SOURCE=..\World.walls.cpp
# End Source File
# End Group
# Begin Group "serverNT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\resource.h

!IF  "$(CFG)" == "XPilotServer - Win32 Release"

!ELSEIF  "$(CFG)" == "XPilotServer - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\ServerThreadW32.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerThreadW32.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h

!IF  "$(CFG)" == "XPilotServer - Win32 Release"

!ELSEIF  "$(CFG)" == "XPilotServer - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\xpilots.rc
# End Source File
# Begin Source File

SOURCE=.\XPilotServerW32.cpp
# End Source File
# Begin Source File

SOURCE=.\XPilotServerW32.h
# End Source File
# End Group
# Begin Group "serverlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\serverlib\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\Connection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\Connection.h
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\File.cpp
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\File.h
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\NetServer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\NetServer.h
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\NetServer.sched.cpp
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\serverconst.h
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\ServerOption.cpp
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\ServerOption.h
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\ServerOptions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\ServerOptions.h
# End Source File
# Begin Source File

SOURCE=..\..\serverlib\ServerOptions.init.cpp.h
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\..\common\audioDefs.h
# End Source File
# Begin Source File

SOURCE=..\..\common\bit.h
# End Source File
# Begin Source File

SOURCE=..\..\common\checknames.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\checknames.h
# End Source File
# Begin Source File

SOURCE=..\..\common\commonproto.h
# End Source File
# Begin Source File

SOURCE=..\..\common\config.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\config.h
# End Source File
# Begin Source File

SOURCE=..\..\common\const.h
# End Source File
# Begin Source File

SOURCE=..\..\common\cstring.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\cstring.h
# End Source File
# Begin Source File

SOURCE=..\..\common\draw.h
# End Source File
# Begin Source File

SOURCE=..\..\common\error.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\error.h
# End Source File
# Begin Source File

SOURCE=..\..\common\FirewallPortList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\FirewallPortList.h
# End Source File
# Begin Source File

SOURCE=..\..\common\Ini.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Ini.Save.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Ini.Win.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\IniServer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\IniServer.Defaults.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\item.h
# End Source File
# Begin Source File

SOURCE=..\..\common\keys.h
# End Source File
# Begin Source File

SOURCE=..\..\common\list.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\..\common\map.h
# End Source File
# Begin Source File

SOURCE=..\..\common\math.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Obj.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Obj.h
# End Source File
# Begin Source File

SOURCE=..\..\common\pack.h
# End Source File
# Begin Source File

SOURCE=..\..\common\packet.h
# End Source File
# Begin Source File

SOURCE=..\..\common\PacketCtl.h
# End Source File
# Begin Source File

SOURCE=..\..\common\portability.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\portability.h
# End Source File
# Begin Source File

SOURCE=..\..\common\randommt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\rules.h
# End Source File
# Begin Source File

SOURCE=..\..\common\ScoreTable.h
# End Source File
# Begin Source File

SOURCE=..\..\common\setup.h
# End Source File
# Begin Source File

SOURCE=..\..\common\shipshape.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\showtime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\showtime.h
# End Source File
# Begin Source File

SOURCE=..\..\common\Sock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Sock.h
# End Source File
# Begin Source File

SOURCE=..\..\common\Sockbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Sockbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\common\sockerrs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\sockerrs.h
# End Source File
# Begin Source File

SOURCE=..\..\common\soundNames.cpp.h
# End Source File
# Begin Source File

SOURCE=..\..\common\strdup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\strlcpy.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\version.h
# End Source File
# Begin Source File

SOURCE=..\..\common\WildMap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\WildMap.h
# End Source File
# Begin Source File

SOURCE=..\..\common\xpmemory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\xpprintf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\xpprintf.h
# End Source File
# End Group
# Begin Group "commonNT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\NT\winNet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\NT\winNet.h
# End Source File
# Begin Source File

SOURCE=..\..\common\NT\winX.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\xpilots.ico
# End Source File
# Begin Source File

SOURCE=.\res\xpilots.rc2
# End Source File
# End Group
# End Target
# End Project
