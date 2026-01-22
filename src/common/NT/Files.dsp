# Microsoft Developer Studio Project File - Name="Files" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Files - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "Files.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "Files.mak" CFG="Files - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Files - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Files - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "Files - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f Files.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Files.exe"
# PROP BASE Bsc_Name "Files.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /f Files.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Files.exe"
# PROP Bsc_Name "Files.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Files - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f Files.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Files.exe"
# PROP BASE Bsc_Name "Files.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "NMAKE /f Files.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "Files.exe"
# PROP Bsc_Name "Files.bsc"
# PROP Target_Dir ""

!ENDIF

# Begin Target

# Name "Files - Win32 Release"
# Name "Files - Win32 Debug"

!IF  "$(CFG)" == "Files - Win32 Release"

!ELSEIF  "$(CFG)" == "Files - Win32 Debug"

!ENDIF

# Begin Group "./"

# PROP Default_Filter ""
# Begin Group "./doc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\doc\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\..\..\doc\ChangeLog
# End Source File
# Begin Source File

SOURCE=..\..\..\doc\CREDITS
# End Source File
# Begin Source File

SOURCE=..\..\..\doc\FAQ
# End Source File
# Begin Source File

SOURCE=..\..\..\doc\FIXED
# End Source File
# Begin Source File

SOURCE=..\..\..\doc\README.MAPS
# End Source File
# Begin Source File

SOURCE=..\..\..\doc\README.MAPS2
# End Source File
# Begin Source File

SOURCE=..\..\..\doc\README.SHIPS
# End Source File
# Begin Source File

SOURCE=..\..\..\doc\README.sounds
# End Source File
# Begin Source File

SOURCE=..\..\..\doc\README.talkmacros
# End Source File
# Begin Source File

SOURCE=..\..\..\doc\TODO
# End Source File
# Begin Source File

SOURCE="..\..\..\doc\xpilot-linux.dif"
# End Source File
# Begin Source File

SOURCE=..\..\..\doc\xpilot.spec
# End Source File
# End Group
# Begin Group "./inst"

# PROP Default_Filter ""
# Begin Group "kde"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\inst\kde\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\kde\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\kde\xpilot.desktop
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\kde\xpilot16x16.png
# End Source File
# End Group
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\inst\win32\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\creditsUpdate.pl
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\getpw.ini
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\makeDistribution
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\README.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\README.txt.msub
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\READMEbin.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\READMEbin.txt.msub
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\ServerMOTD.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\ServerMOTD.txt.msub
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\xp_off.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\xp_on.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\win32\XPilot.nsi
# End Source File
# End Group
# Begin Group "unix"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\inst\unix\fetchBuildXPilot5
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\unix\README
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\inst\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\..\..\inst\Makefile.in
# End Source File
# End Group
# Begin Group "./lib"

# PROP Default_Filter ""
# Begin Group "scoreserver"

# PROP Default_Filter ""
# Begin Group "html"

# PROP Default_Filter ""
# Begin Group "g"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib\scoreserver\html\g\xpback.png
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\lib\scoreserver\html\index.html
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\scoreserver\html\playerline.html
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\scoreserver\html\scorelist.html
# End Source File
# End Group
# End Group
# Begin Group "sounds"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib\sounds\bfire.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\bloop.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\buzzer.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\confirm.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\corkpop.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\done.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\doorchimes.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\Drop01.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\explo2.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\failure.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\finished.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\flare.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\gundead.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\launch1.18.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\lboom.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\magic.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\MusicaDefault.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\question.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\sboom.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\sbounce.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\sengine.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\siren.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\sstart.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\sstop.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\start.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\success.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\UtopiaClose.wav
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds\waiting.wav
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\lib\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\defaults
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\Makefile.in
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\robots
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\sounds.conf
# End Source File
# End Group
# Begin Group "./src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\.cvsignore
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\.cvsignore
# End Source File
# Begin Source File

SOURCE=..\..\..\INSTALL.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\INSTALL_FLTK.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\LICENSE
# End Source File
# Begin Source File

SOURCE=..\..\..\README.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\README.txt.msub
# End Source File
# Begin Source File

SOURCE=..\..\..\xpilot.version
# End Source File
# End Group
# End Target
# End Project
