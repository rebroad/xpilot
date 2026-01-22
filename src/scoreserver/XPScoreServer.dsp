# Microsoft Developer Studio Project File - Name="XPScoreServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=XPScoreServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "XPScoreServer.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "XPScoreServer.mak" CFG="XPScoreServer - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "XPScoreServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "XPScoreServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XPScoreServer - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\common" /I "..\serverlib" /I "..\common\expat\lib" /I "..\common\web" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_XPILOTSCORESERVER_" /Yu"StdAfx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "XPScoreServer - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\common" /I "..\serverlib" /I "..\common\expat\lib" /I "..\common\web" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_XPILOTSCORESERVER_" /D "_SOCKWINDOWS" /FR /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /incremental:no /nodefaultlib

!ENDIF

# Begin Target

# Name "XPScoreServer - Win32 Release"
# Name "XPScoreServer - Win32 Debug"
# Begin Group "XPScoreServer"

# PROP Default_Filter ""
# Begin Group "engines"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\engines\.cvsignore
# End Source File
# Begin Source File

SOURCE=.\engines\Makefile.in
# End Source File
# Begin Source File

SOURCE=.\engines\ScoreEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\engines\ScoreEngine.h
# End Source File
# Begin Source File

SOURCE=.\engines\ScoreEngineBasic.cpp
# End Source File
# Begin Source File

SOURCE=.\engines\ScoreEngineBasic.h
# End Source File
# Begin Source File

SOURCE=.\engines\ScoreEngineBasicCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\engines\ScoreEngineBasicCfg.h
# End Source File
# Begin Source File

SOURCE=.\engines\ScoreEngineBasicXML.cpp
# End Source File
# Begin Source File

SOURCE=.\engines\ScoreEngineBasicXML.h
# End Source File
# Begin Source File

SOURCE=.\engines\ScoreEngineBasicXMLCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\engines\ScoreEngineBasicXMLCfg.h
# End Source File
# Begin Source File

SOURCE=.\engines\ScoreEngineCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\engines\ScoreEngineCfg.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\.cvsignore
# End Source File
# Begin Source File

SOURCE=.\ConnectionLocalCtl.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnectionLocalCtl.h
# End Source File
# Begin Source File

SOURCE=.\HttpScore.cpp
# End Source File
# Begin Source File

SOURCE=.\HttpScore.h
# End Source File
# Begin Source File

SOURCE=.\Makefile.in
# End Source File
# Begin Source File

SOURCE=.\NetServerScoreServer.cpp
# End Source File
# Begin Source File

SOURCE=.\NetServerScoreServer.h
# End Source File
# Begin Source File

SOURCE=.\ScoreEngineSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\ScoreEngineSelector.h
# End Source File
# Begin Source File

SOURCE=.\ScoreServer.cpp
# End Source File
# Begin Source File

SOURCE=.\ScoreServer.h
# End Source File
# Begin Source File

SOURCE=.\ScoreServerCfg.cpp
# End Source File
# Begin Source File

SOURCE=.\ScoreServerCfg.h
# End Source File
# Begin Source File

SOURCE=.\ScoreServerNetClient.cpp
# End Source File
# Begin Source File

SOURCE=.\ScoreServerNetClient.h
# End Source File
# Begin Source File

SOURCE=.\ScoreServerUnix.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ScoreServerW32.cpp

!IF  "$(CFG)" == "XPScoreServer - Win32 Release"

!ELSEIF  "$(CFG)" == "XPScoreServer - Win32 Debug"

# ADD CPP /Yu

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\ScoreServerW32.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Group "expat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\expat\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\ascii.h
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\asciitab.h
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\expat.h
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\expatconfig.h
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\iasciitab.h
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\latin1tab.h
# End Source File
# Begin Source File

SOURCE=..\common\expat\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\nametab.h
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\utf8tab.h
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\winconfig.h
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\xmlparse.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\xmlrole.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\xmlrole.h
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\xmltok.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\xmltok.h
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\xmltok_impl.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\xmltok_impl.h
# End Source File
# Begin Source File

SOURCE=..\common\expat\lib\xmltok_ns.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "web"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\web\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\common\web\Http.cpp
# End Source File
# Begin Source File

SOURCE=..\common\web\Http.h
# End Source File
# Begin Source File

SOURCE=..\common\web\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\common\web\NetList.cpp
# End Source File
# Begin Source File

SOURCE=..\common\web\NetList.h
# End Source File
# Begin Source File

SOURCE=..\common\web\Network.cpp
# End Source File
# Begin Source File

SOURCE=..\common\web\Network.h
# End Source File
# End Group
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

SOURCE=..\common\bit.h
# End Source File
# Begin Source File

SOURCE=..\common\CfgBase.cpp
# End Source File
# Begin Source File

SOURCE=..\common\CfgBase.h
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

SOURCE=..\common\error.h
# End Source File
# Begin Source File

SOURCE=..\common\FirewallPortList.cpp
# End Source File
# Begin Source File

SOURCE=..\common\FirewallPortList.h
# End Source File
# Begin Source File

SOURCE=..\common\Makefile.in
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

SOURCE=..\common\PacketCtl.h
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

SOURCE=..\common\ServerOptionsNetClient.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ServerOptionsNetClient.h
# End Source File
# Begin Source File

SOURCE=..\common\ServerSite.h
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

SOURCE=..\common\strlcpy.cpp
# End Source File
# Begin Source File

SOURCE=..\common\types.h
# End Source File
# Begin Source File

SOURCE=..\common\WildMap.cpp
# End Source File
# Begin Source File

SOURCE=..\common\WildMap.h
# End Source File
# Begin Source File

SOURCE=..\common\XMLArchive.cpp
# End Source File
# Begin Source File

SOURCE=..\common\XMLArchive.h
# End Source File
# Begin Source File

SOURCE=..\common\xpprintf.cpp
# End Source File
# Begin Source File

SOURCE=..\common\xpprintf.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\XPScoreServer.ico
# End Source File
# Begin Source File

SOURCE=.\res\XPScoreServer.rc
# End Source File
# End Group
# Begin Group "serverlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\serverlib\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\serverlib\Connection.cpp
# End Source File
# Begin Source File

SOURCE=..\serverlib\Connection.h
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

SOURCE=..\serverlib\NetServer.cpp
# End Source File
# Begin Source File

SOURCE=..\serverlib\NetServer.h
# End Source File
# Begin Source File

SOURCE=..\serverlib\NetServer.sched.cpp
# End Source File
# Begin Source File

SOURCE=..\serverlib\serverconst.h
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
# Begin Group "html"

# PROP Default_Filter ""
# Begin Group "g"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\lib\scoreserver\html\g\styles.css
# End Source File
# Begin Source File

SOURCE=..\..\lib\scoreserver\html\g\xpback.png
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\lib\scoreserver\html\index.html
# End Source File
# Begin Source File

SOURCE=..\..\lib\scoreserver\html\playerline.html
# End Source File
# Begin Source File

SOURCE=..\..\lib\scoreserver\html\scorelist.html
# End Source File
# End Group
# End Target
# End Project
