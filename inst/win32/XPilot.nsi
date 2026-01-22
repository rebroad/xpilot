# XPilot.nsi - the script to NSIS, the NullSoft Install System.
# $Id: XPilot.nsi,v 1.30 2007/02/03 05:37:14 dick Exp $
#              Copyright 2001 Jarno van der Kolk <jarno@j-a-r-n-o.nl>
#              Released under GNU General Public License Version 2
# The NullSoft Install System can be found here http://www.nullsoft.com/free/nsis/
#
#
# $Log: XPilot.nsi,v $
# Revision 1.30  2007/02/03 05:37:14  dick
# Don't overwrite user's existing lib files
#
# Revision 1.29  2007/01/21 07:37:43  dick
# Install robots5.txt instead of robots4a.txt
#
# Revision 1.28  2007/01/17 08:52:29  dick
# Install robots4a.txt
#
# Revision 1.27  2007/01/06 18:37:43  dick
# Client menu link to should point to client
#
# Revision 1.26  2004/08/01 02:10:08  dick
# Update to nsis 2.0
#
# Header stuff
Name "[PRODUCT]"
Icon "XPilot5.ico"
#OutFile XPilot-440NT14.exe
!include outfilename.txt

Page license
Page directory
Page custom MakePassword "" ": Server password"
Page instfiles

# License text seems to be next
LicenseText "[PRODUCT] works under the GPL license"
LicenseData "License.txt"

###############################################################################
Function MakePassword
  IfFileExists "$INSTDIR\lib\ServerPassword.txt" hasPassword noPassword
hasPassword:
  FileOpen $5 "$INSTDIR\lib\ServerPassword.txt" r
  FileSeek $5 9
  FileRead $5 $6
  FileClose $5
  WriteINIStr $7 "Field 2" State $6

noPassword:
  Push $R0
  # CallInstDLL $8 dialog
  InstallOptions::dialog $7
  Pop $R0
FunctionEnd

# Component page configuration commands
InstType "Typical"
#InstType "Compact"
#EnabledBitmap "xp_on.bmp"
#DisabledBitmap "xp_off.bmp"
#CheckBitmap "xp_on.bmp"
#DisabledBitmap "xp_off.bmp"
ComponentText "This will install [RELEASE] on your computer."

# Directory selection page configuration commands
InstallDir C:\[PRODUCT]
DirText "Select the directory to install [RELEASE] in"
#DirShow show
InstallDirRegKey HKEY_LOCAL_MACHINE SOFTWARE\[PRODUCT] "Install_Dir"

# Install page configuration commands
AutoCloseWindow false
ShowInstDetails show
# Installation execution commands
# This seems to be the real stuff. :)

Section "-Essential game files"
SetOutPath "$INSTDIR"
CreateDirectory "$INSTDIR\doc"
CreateDirectory "$INSTDIR\lib"
CreateDirectory "$INSTDIR\lib\maps"
CreateDirectory "$INSTDIR\lib\textures"

IfFileExists "$INSTDIR\XPilotClient.ini" noini
  File "XPilotClient.ini"
  WriteINIStr "$INSTDIR\XPilotClient.ini" "Settings" shipShapeFile "$INSTDIR\lib\XPilot.shp"
noini:

# Check for OpenAL
GetDLLVersion "wrap_oal.dll" $R1 $R2
IfErrors instOpenAL noInstOpenAL
instOpenAL:
SetOutPath $TEMP
File "OpenALwEAX.exe"
ExecWait "$TEMP\OpenALwEAX.exe"
Delete "OpenALwEAX.exe"

noInstOpenAL:

# $  (Root) Directory
SetOutPath "$INSTDIR"
File "License.txt"
File "README.txt"
File "XPilot5.exe"
File "XPilotClient.exe"
File "XPilotServer.exe"
File "XPreplay.exe"
File "XPreplay.reg"
File "XPScoreServer.exe"
#File "msvcr70.dll"

# $/doc
SetOutPath "$INSTDIR\doc"
File "doc\ChangeLog.txt"
File "doc\Credits.txt"
File "doc\Faq.txt"
File "doc\README.MAPS.txt"
File "doc\README.MAPS2.txt"
File "doc\README.SHIPS.txt"
File "doc\README.talkmacros.txt"
File "doc\Todo.txt"
File "doc\The XPilot Page.url"
File "doc\XPilot @ BuckoSoft.url"
File "doc\Newbie Guide.url"

# $/lib
SetOutPath "$INSTDIR\lib"
IfFileExists "$INSTDIR\lib\defaults.txt" nodefaults
	File "lib\defaults.txt"
nodefaults:
IfFileExists "$INSTDIR\lib\robots.txt" norobots
	File "lib\robots.txt"
norobots:
IfFileExists "$INSTDIR\lib\robots5.txt" norobots5
	File "lib\robots5.txt"
norobots5:
IfFileExists "$INSTDIR\lib\sounds.conf" nosoundsconf
	File "lib\sounds.conf"
nosoundsconf:
IfFileExists "$INSTDIR\lib\XPilot.shp" noshp
  File "lib\XPilot.shp"
noshp:

# $/lib/buttons
SetOutPath "$INSTDIR\lib\buttons"
!include xpebuttons-nsi.txt

# $/lib/sounds
SetOutPath "$INSTDIR\lib\sounds"
File "lib\sounds\bfire.wav"
File "lib\sounds\bloop.wav"
File "lib\sounds\buzzer.wav"
File "lib\sounds\confirm.wav"
File "lib\sounds\corkpop.wav"
File "lib\sounds\done.wav"
File "lib\sounds\doorchimes.wav"
File "lib\sounds\Drop01.wav"
File "lib\sounds\explo2.wav"
File "lib\sounds\failure.wav"
File "lib\sounds\finished.wav"
File "lib\sounds\flare.wav"
File "lib\sounds\gundead.wav"
File "lib\sounds\launch1.18.wav"
File "lib\sounds\lboom.wav"
File "lib\sounds\magic.wav"
File "lib\sounds\MusicaDefault.wav"
File "lib\sounds\popclick.wav"
File "lib\sounds\RobotzDefault.wav"
File "lib\sounds\RobotzQuestion.wav"
File "lib\sounds\sboom.wav"
File "lib\sounds\sbounce.wav"
File "lib\sounds\sstart.wav"
File "lib\sounds\success.wav"
File "lib\sounds\UtopiaClose.wav"
File "lib\sounds\waiting.wav"
File "lib\sounds\zap.wav"

# $/lib/textures
SetOutPath "$INSTDIR\lib\textures"
File "lib\textures\allitems.ppm"
File "lib\textures\asteroidconcentrator.ppm"
File "lib\textures\ball.ppm"
File "lib\textures\base_down.ppm"
File "lib\textures\base_left.ppm"
File "lib\textures\base_right.ppm"
File "lib\textures\base_up.ppm"
File "lib\textures\bullet.ppm"
File "lib\textures\bullet_blue.ppm"
File "lib\textures\bullet_green.ppm"
File "lib\textures\bullet2.ppm"
File "lib\textures\cannon_down.ppm"
File "lib\textures\cannon_left.ppm"
File "lib\textures\cannon_right.ppm"
File "lib\textures\cannon_up.ppm"
File "lib\textures\checkpoint.ppm"
File "lib\textures\clouds.ppm"
File "lib\textures\concentrator.ppm"
File "lib\textures\fuel2.ppm"
File "lib\textures\fuelcell.ppm"
File "lib\textures\holder1.ppm"
File "lib\textures\holder2.ppm"
File "lib\textures\logo.ppm"
File "lib\textures\meter.ppm"
File "lib\textures\mine_other.ppm"
File "lib\textures\mine_team.ppm"
File "lib\textures\minus.ppm"
File "lib\textures\paused.ppm"
File "lib\textures\plus.ppm"
File "lib\textures\radar.ppm"
File "lib\textures\radar2.ppm"
File "lib\textures\radar3.ppm"
File "lib\textures\refuel.ppm"
File "lib\textures\ship.ppm"
File "lib\textures\ship_blue.ppm"
File "lib\textures\ship_red.ppm"
File "lib\textures\ship_red2.ppm"
File "lib\textures\ship_red3.ppm"
File "lib\textures\sparks.ppm"
File "lib\textures\wall_bottom.ppm"
File "lib\textures\wall_dl.ppm"
File "lib\textures\wall_dr.ppm"
File "lib\textures\wall_fi.ppm"
File "lib\textures\wall_left.ppm"
File "lib\textures\wall_right.ppm"
File "lib\textures\wall_top.ppm"
File "lib\textures\wall_ul.ppm"
File "lib\textures\wall_ull.ppm"
File "lib\textures\wall_ur.ppm"
File "lib\textures\wall_url.ppm"
File "lib\textures\wormhole.ppm"

# $/lib/scoreserver/html
SetOutPath "$INSTDIR\lib\scoreserver\html"
File "lib\scoreserver\html\index.html"
File "lib\scoreserver\html\playerline.html"
File "lib\scoreserver\html\scorelist.html"

# $/lib/scoreserver/html/g
SetOutPath "$INSTDIR\lib\scoreserver\html\g"
File "lib\scoreserver\html\g\xpback.png"
File "lib\scoreserver\html\g\styles.css"

# Uninstaller
SetOutPath "$INSTDIR"
WriteUninstaller "uninstall.exe"

# Write the installation path into the registry
WriteRegStr HKEY_LOCAL_MACHINE SOFTWARE\[PRODUCT] "Install_Dir" "$INSTDIR"

# Write the uninstall keys for Windows
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\[PRODUCT]" "DisplayName" "[PRODUCT]"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\[PRODUCT]" "UninstallString" '"$INSTDIR\uninstall.exe"'

###############################################################################
# Fetch the Server password
  ReadINIStr $6 $7 "Field 2" State
  DetailPrint "Install Password=$R0"
  FileOpen $5 $INSTDIR\lib\ServerPassword.txt w
  FileWrite $5 "password:$6$\r$\n"
  FileClose $5

SectionEnd

###############################################################################
# Default maps take in half-a-meg!
Section "Mappack"
SectionIn 1 2
SetOutPath "$INSTDIR\lib\maps"
File "lib\maps\blood-music2.xp"
File "lib\maps\CAMD.xp"
File "lib\maps\cloudscape.xp"
File "lib\maps\default.xp"
File "lib\maps\doggy.xp"
File "lib\maps\fireball.xp"
File "lib\maps\fishfight.xp"
File "lib\maps\fuzz.xp"
File "lib\maps\fuzz2.xp"
File "lib\maps\globe.xp"
File "lib\maps\grandprix.xp"
File "lib\maps\newdarkhell.xp"
File "lib\maps\newdarkhell2.xp"
File "lib\maps\pad.xp"
File "lib\maps\pit.xp"
File "lib\maps\planetx.xp"
File "lib\maps\teamball.xp"
File "lib\maps\tourmination.xp"
File "lib\maps\tournament.xp"
File "lib\maps\war.xp"
SectionEnd

###############################################################################
# Now to make some lovely shortcuts in the already oh so crowded Startmenu.
Section "Start Menu Shortcuts"
SectionIn 1
SetOutPath "$INSTDIR"
CreateDirectory "$SMPROGRAMS\[PRODUCT]"
CreateDirectory "$SMPROGRAMS\[PRODUCT]\doc"
CreateShortCut "$SMPROGRAMS\[PRODUCT]\doc\README.lnk" "$INSTDIR\README.txt" ""
CreateShortCut "$SMPROGRAMS\[PRODUCT]\doc\License.lnk" "$INSTDIR\License.txt" ""
CreateShortCut "$SMPROGRAMS\[PRODUCT]\doc\ChangeLog.lnk" "$INSTDIR\doc\ChangeLog.txt" ""
CreateShortCut "$SMPROGRAMS\[PRODUCT]\doc\The XPilot Page.lnk" "$INSTDIR\doc\The XPilot Page.url" ""
CreateShortCut "$SMPROGRAMS\[PRODUCT]\doc\XPilot @ BuckoSoft.lnk" "$INSTDIR\doc\XPilot @ BuckoSoft.url" ""
CreateShortCut "$SMPROGRAMS\[PRODUCT]\XPilot.lnk" "$INSTDIR\XPilot5.exe" "" "$INSTDIR\XPilot5.exe" 0
CreateDirectory "$SMPROGRAMS\[PRODUCT]\programs"
#CreateShortCut "$SMPROGRAMS\[PRODUCT]\programs\XPwhere.lnk" "$INSTDIR\XPwhere.exe" "" "$INSTDIR\XPwhere.exe" 0
#CreateShortCut "$SMPROGRAMS\[PRODUCT]\programs\Keyboard Config Tool.lnk" "$INSTDIR\XPKbConfig.exe" "" "$INSTDIR\XPKbConfig.exe" 0
CreateShortCut "$SMPROGRAMS\[PRODUCT]\programs\XPilotClient.lnk" "$INSTDIR\XPilotClient.exe" "" "$INSTDIR\XPilotClient.exe" 0
CreateShortCut "$SMPROGRAMS\[PRODUCT]\programs\XPilotServer.lnk" "$INSTDIR\XPilotServer.exe" "" "$INSTDIR\XPilotServer.exe" 0
CreateShortCut "$SMPROGRAMS\[PRODUCT]\programs\XPilotClient.ini.lnk" "$INSTDIR\XPilotClient.ini" "" "$INSTDIR\XPilotClient.ini" 0
CreateShortCut "$SMPROGRAMS\[PRODUCT]\programs\XPreplay.lnk" "$INSTDIR\XPreplay.exe" "" "$INSTDIR\XPreplay.exe" 0
CreateShortCut "$SMPROGRAMS\[PRODUCT]\programs\XPScoreServer.lnk" "$INSTDIR\XPScoreServer.exe" "" "$INSTDIR\XPScoreServer.exe" 0
CreateShortCut "$SMPROGRAMS\[PRODUCT]\programs\XPilot.lnk" "$INSTDIR\XPilot5.exe" "" "$INSTDIR\XPilot5.exe" 0

# Doesn't work
#StrCmp "$TEMP" "$QUICKLAUNCH" noQuickLaunch
#CreateShortCut "$QUICKLAUNCH\[PRODUCT]\programs\XPilot.lnk" "$INSTDIR\XPilot5.exe" "" "$INSTDIR\XPilot5.exe" 0
#noQuickLaunch:

#put uninstall at the bottom
CreateShortCut "$SMPROGRAMS\[PRODUCT]\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

#############################################################################
# Uninstall stuff. Sad but true, some people actually uninstall XPilot...

; special uninstall section.
Section "Uninstall"

MessageBox MB_YESNO|MB_ICONEXCLAMATION "Press 'Yes' to remove [RELEASE] from your system" IDYES removeYes IDNO removeNo
Goto removeNo

removeYes:
; remove registry keys
DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\[PRODUCT]"
DeleteRegKey HKEY_LOCAL_MACHINE SOFTWARE\[PRODUCT]
; DeleteRegKey HKEY_CURRENT_USER SOFTWARE\BuckoSoft\[PRODUCT]

; remove files
Delete $INSTDIR\buttons\*.*
Delete $INSTDIR\doc\*.*
Delete $INSTDIR\lib\buttons\*.*
Delete $INSTDIR\lib\maps\*.*
Delete $INSTDIR\lib\scoreserver\html\g\*.*
Delete $INSTDIR\lib\scoreserver\html\*.*
Delete $INSTDIR\lib\scoreserver\*.*
Delete $INSTDIR\lib\sounds\*.*
Delete $INSTDIR\lib\textures\*.*
Delete $INSTDIR\lib\var\*.*
Delete $INSTDIR\lib\*.*
Delete $INSTDIR\*.*

; remove shortcuts, if any.
Delete "$SMPROGRAMS\[PRODUCT]\programs\*.*"
Delete "$SMPROGRAMS\[PRODUCT]\doc\*.*"
Delete "$SMPROGRAMS\[PRODUCT]\*.*"
; remove directories used.
RMDir "$SMPROGRAMS\[PRODUCT]\programs"
RMDir "$SMPROGRAMS\[PRODUCT]\doc"
RMDir "$SMPROGRAMS\[PRODUCT]"
RMDir "$INSTDIR\lib\buttons"
RMDir "$INSTDIR\lib\maps"
RMDir "$INSTDIR\lib\scoreserver\html\g"
RMDir "$INSTDIR\lib\scoreserver\html"
RMDir "$INSTDIR\lib\scoreserver"
RMDir "$INSTDIR\lib\sounds"
RMDir "$INSTDIR\lib\textures"
RMDir "$INSTDIR\lib\var"
RMDir "$INSTDIR\lib"
RMDir "$INSTDIR\doc"
RMDir "$INSTDIR"

removeNo:
SectionEnd

###############################################################################
Function .onInit
  StrCpy $9 0
  GetTempFileName $8
  GetTempFileName $7
  File /oname=$8 InstallOptions.dll
  File /oname=$7 "getpw.ini"
	InitPluginsDir
	#File /oname=$8 InstallOptions.dll
	#File /oname=$PLUGINSDIR\getpw.ini getpw.ini
FunctionEnd

###############################################################################
Function .onInstSuccess
MessageBox MB_YESNO|MB_ICONINFORMATION "[RELEASE] installed successfully.  Would you like to start XPilot5?" IDNO NoXPstartNow
Exec $INSTDIR\XPilot5.exe
NoXPstartNow:
Call Cleanup
FunctionEnd

###############################################################################
Function .onInstFailed
Call Cleanup
FunctionEnd

###############################################################################
Function .onUserAbort
Call Cleanup
FunctionEnd

###############################################################################
Function Cleanup
#	Delete $8
	Delete $7
FunctionEnd
; eof
