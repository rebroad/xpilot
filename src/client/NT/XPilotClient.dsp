# Microsoft Developer Studio Project File - Name="XPilotClient" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=XPilotClient - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "XPilotClient.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "XPilotClient.mak" CFG="XPilotClient - Win32 Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "XPilotClient - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "XPilotClient - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "XPilotClient - Win32 ReleasePentium" (based on "Win32 (x86) Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XPilotClient - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /I "..\..\common" /I "..\..\common\NT" /I "..\\" /D "NDEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D PAINT_FREE=0 /D "_SOCKWINDOWS" /D "SOUND" /Yu"StdAfx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 alut.lib openal32.lib opengl32.lib glu32.lib Ws2_32.lib Kernel32.lib Advapi32.lib User32.lib GDI32.lib ComDlg32.lib WinSpool.lib Shell32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "XPilotClient - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\..\common\NT" /I ".." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D PAINT_FREE=0 /D "_SOCKWINDOWS" /D "SOUND" /FR /Yu"StdAfx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 alut.lib openal32.lib opengl32.lib glu32.lib Ws2_32.lib Kernel32.lib Advapi32.lib User32.lib GDI32.lib ComDlg32.lib WinSpool.lib Shell32.lib /nologo /subsystem:windows /map /debug /machine:I386

!ELSEIF  "$(CFG)" == "XPilotClient - Win32 ReleasePentium"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\XPilot__"
# PROP BASE Intermediate_Dir ".\XPilot__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\XPilot__"
# PROP Intermediate_Dir ".\XPilot__"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /Zd /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "x_BETAEXPIRE" /FR /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /G5 /MD /W3 /GX /Zd /O2 /I "..\..\common" /I "..\..\common\NT" /D "NDEBUG" /D "_MBCS" /D "x_BETAEXPIRE" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "X_SOUND" /D "WINDOWSCALING" /D PAINT_FREE=0 /D "_SOCKWINDOWS" /FR /Yu"StdAfx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /map /machine:I386 /out:"Release/XPilotNT.exe"
# ADD LINK32 alut.lib openal32.lib opengl32.lib glu32.lib Ws2_32.lib Kernel32.lib Advapi32.lib User32.lib GDI32.lib ComDlg32.lib WinSpool.lib Shell32.lib /nologo /subsystem:windows /map /machine:I386 /out:".\Release\XPilotClient.exe"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\XPilotClient.exe c:\XPilot
# End Special Build Tool

!ENDIF

# Begin Target

# Name "XPilotClient - Win32 Release"
# Name "XPilotClient - Win32 Debug"
# Name "XPilotClient - Win32 ReleasePentium"
# Begin Group "client"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\about.cpp
# End Source File
# Begin Source File

SOURCE=..\blockbitmaps.cpp
# End Source File
# Begin Source File

SOURCE=..\blockbitmaps.h
# End Source File
# Begin Source File

SOURCE=..\client.cpp
# End Source File
# Begin Source File

SOURCE=..\client.h
# End Source File
# Begin Source File

SOURCE=..\ClientWorld.h
# End Source File
# Begin Source File

SOURCE=..\colors.cpp
# End Source File
# Begin Source File

SOURCE=..\configure.cpp
# End Source File
# Begin Source File

SOURCE=..\configure.h
# End Source File
# Begin Source File

SOURCE=..\datagram.cpp
# End Source File
# Begin Source File

SOURCE=..\datagram.h
# End Source File
# Begin Source File

SOURCE=..\dbuff.h
# End Source File
# Begin Source File

SOURCE=..\gfx2d.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx2d.h
# End Source File
# Begin Source File

SOURCE=..\gfx3d.cpp
# End Source File
# Begin Source File

SOURCE=..\gfx3d.h
# End Source File
# Begin Source File

SOURCE=..\guimap.cpp
# End Source File
# Begin Source File

SOURCE=..\guimap.h
# End Source File
# Begin Source File

SOURCE=..\guiobjects.cpp
# End Source File
# Begin Source File

SOURCE=..\guiobjects.h
# End Source File
# Begin Source File

SOURCE=..\icon.h
# End Source File
# Begin Source File

SOURCE=..\join.cpp
# End Source File
# Begin Source File

SOURCE=..\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\netclient.cpp
# End Source File
# Begin Source File

SOURCE=..\netclient.h
# End Source File
# Begin Source File

SOURCE=..\Other.cpp
# End Source File
# Begin Source File

SOURCE=..\Other.h
# End Source File
# Begin Source File

SOURCE=..\paint.cpp
# End Source File
# Begin Source File

SOURCE=..\paint.h
# End Source File
# Begin Source File

SOURCE=..\paintdata.cpp
# End Source File
# Begin Source File

SOURCE=..\paintdata.h
# End Source File
# Begin Source File

SOURCE=..\painthud.cpp
# End Source File
# Begin Source File

SOURCE=..\paintmacros.h
# End Source File
# Begin Source File

SOURCE=..\paintmap.cpp
# End Source File
# Begin Source File

SOURCE=..\paintobjects.cpp
# End Source File
# Begin Source File

SOURCE=..\paintradar.cpp
# End Source File
# Begin Source File

SOURCE=..\protoclient.h
# End Source File
# Begin Source File

SOURCE=..\query.cpp
# End Source File
# Begin Source File

SOURCE=..\record.cpp
# End Source File
# Begin Source File

SOURCE=..\record.h
# End Source File
# Begin Source File

SOURCE=..\RobotWatchMan.cpp
# End Source File
# Begin Source File

SOURCE=..\RobotWatchMan.h
# End Source File
# Begin Source File

SOURCE=..\sim.cpp
# End Source File
# Begin Source File

SOURCE=..\syslimit.cpp
# End Source File
# Begin Source File

SOURCE=..\talk.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\talkmacros.cpp
# End Source File
# Begin Source File

SOURCE=..\textinterface.cpp
# End Source File
# Begin Source File

SOURCE=..\texture.cpp
# End Source File
# Begin Source File

SOURCE=..\texture.h
# End Source File
# Begin Source File

SOURCE=..\welcome.cpp
# End Source File
# Begin Source File

SOURCE=..\widget.cpp
# End Source File
# Begin Source File

SOURCE=..\widget.h
# End Source File
# Begin Source File

SOURCE=..\xevent.cpp
# End Source File
# Begin Source File

SOURCE=..\xevent.h
# End Source File
# Begin Source File

SOURCE=..\xeventhandlers.cpp
# End Source File
# Begin Source File

SOURCE=..\xeventhandlers.h
# End Source File
# Begin Source File

SOURCE=..\xinit.cpp
# End Source File
# Begin Source File

SOURCE=..\xinit.h
# End Source File
# Begin Source File

SOURCE=..\xpilot.cpp
# End Source File
# Begin Source File

SOURCE=..\xpmread.h
# End Source File
# End Group
# Begin Group "clientNT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\.cvsignore
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TalkW32.cpp
# End Source File
# Begin Source File

SOURCE=.\TalkW32.h
# End Source File
# Begin Source File

SOURCE=.\winBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\winBitmap.h
# End Source File
# Begin Source File

SOURCE=.\winClient.h
# End Source File
# Begin Source File

SOURCE=.\winXThread.cpp
# End Source File
# Begin Source File

SOURCE=.\winXThread.h
# End Source File
# Begin Source File

SOURCE=.\winXXPilot.h
# End Source File
# Begin Source File

SOURCE=.\XPilotClient.rc
# End Source File
# Begin Source File

SOURCE=.\XPilotClientW32.cpp
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\..\common\Archive.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Archive.h
# End Source File
# Begin Source File

SOURCE=..\..\common\astershape.h
# End Source File
# Begin Source File

SOURCE=..\..\common\audioDefs.h
# End Source File
# Begin Source File

SOURCE=..\..\common\AudioMan.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\AudioMan.h
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

SOURCE=..\..\common\Connectparam.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Connectparam.h
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

SOURCE=..\..\common\fileUtils.cpp
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

SOURCE=..\..\common\Ini.h
# End Source File
# Begin Source File

SOURCE=..\..\common\Ini.Save.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Ini.Win.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\IniClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\IniClient.Defaults.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\IniClient.h
# End Source File
# Begin Source File

SOURCE=..\..\common\item.h
# End Source File
# Begin Source File

SOURCE=..\..\common\keys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\keys.h
# End Source File
# Begin Source File

SOURCE=..\..\common\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\..\common\math.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\microdelay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\microdelay.h
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

SOURCE=..\..\common\password.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\password.h
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

SOURCE=..\..\common\randommt.h
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

SOURCE=..\..\common\Sound.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Sound.h
# End Source File
# Begin Source File

SOURCE=..\..\common\SoundEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\SoundEvent.h
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

SOURCE=..\..\common\talk.h
# End Source File
# Begin Source File

SOURCE=..\..\common\wreckshape.h
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

SOURCE=..\..\common\NT\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\..\common\NT\winNet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\NT\winNet.h
# End Source File
# Begin Source File

SOURCE=..\..\common\NT\winX.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\NT\winX.h
# End Source File
# Begin Source File

SOURCE=..\..\common\NT\winX11.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\NT\winX_.h
# End Source File
# Begin Source File

SOURCE=..\..\common\NT\winXKey.h
# End Source File
# End Group
# Begin Group "items"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\common\items\itemAfterburner.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemArmor.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemAutopilot.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemCloakingDevice.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemDeflector.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemEcm.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemEmergencyShield.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemEmergencyThrust.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemEnergyPack.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemHyperJump.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemLaser.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemMinePack.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemMirror.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemPhasingDevice.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemRearShot.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemRocketPack.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemSensorPack.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemTank.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemTractorBeam.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemTransporter.xbm
# End Source File
# Begin Source File

SOURCE=..\..\common\items\itemWideangleShot.xbm
# End Source File
# Begin Source File

SOURCE=..\recordfmt.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\..\lib\textures\ball.xpm
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\textures\rock4.xpm
# End Source File
# Begin Source File

SOURCE=.\res\xpilot.ico
# End Source File
# Begin Source File

SOURCE=.\res\xpilot.rc2
# End Source File
# Begin Source File

SOURCE=..\..\server\NT\res\xpilots.ico
# End Source File
# End Group
# End Target
# End Project
# Section XPilotClient : {0FEA6ED2-A2FE-11CF-80DA-00A02423A46C}
# 	0:8:Splash.h:D:\users\dick\buckosof\xpilot\contrib\NT\xpilot\Splash.h
# 	0:10:Splash.cpp:D:\users\dick\buckosof\xpilot\contrib\NT\xpilot\Splash.cpp
# 	1:10:IDB_SPLASH:102
# 	2:10:ResHdrName:resource.h
# 	2:11:ProjHdrName:stdafx.h
# 	2:10:WrapperDef:_SPLASH_SCRN_
# 	2:12:SplClassName:CSplashWnd
# 	2:21:SplashScreenInsertKey:4.0
# 	2:10:HeaderName:Splash.h
# 	2:10:ImplemName:Splash.cpp
# 	2:7:BmpID16:IDB_SPLASH
# End Section
