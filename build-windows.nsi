; NSIS Installer Script for XPilot NG Windows Client
; This script creates a Windows installer using NSIS (Nullsoft Scriptable Install System)
; To use: Install NSIS from https://nsis.sourceforge.io/, then compile this script

!define PRODUCT_NAME "XPilot NG"
!define PRODUCT_VERSION "4.6.3"
!define PRODUCT_PUBLISHER "XPilot NG Team"
!define PRODUCT_WEB_SITE "http://xpilot.sourceforge.net/"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\xpilot-ng-sdl.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI2.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page (if you have a license file)
; !insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\xpilot-ng-sdl.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "xpilot-ng-setup.exe"
InstallDir "$PROGRAMFILES\XPilot NG"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"

  ; Client executable (adjust path based on your build)
  File "build-windows\src\client\sdl\xpilot-ng-sdl.exe"
  ; Or if using X11 client:
  ; File "build-windows\src\client\xpilot-ng-x11.exe"

  ; Required DLLs (adjust paths as needed)
  File "build-windows\zlib1.dll"
  File "build-windows\libexpat-1.dll"
  File "build-windows\SDL.dll"
  File "build-windows\SDL_ttf.dll"
  File "build-windows\SDL_image.dll"

  ; Data files
  SetOutPath "$INSTDIR\share\xpilot-ng"
  File /r "lib\*.*"

  ; Documentation
  SetOutPath "$INSTDIR\doc"
  File /r "doc\*.*"

  ; Create shortcuts
  CreateDirectory "$SMPROGRAMS\XPilot NG"
  CreateShortCut "$SMPROGRAMS\XPilot NG\XPilot NG.lnk" "$INSTDIR\xpilot-ng-sdl.exe"
  CreateShortCut "$SMPROGRAMS\XPilot NG\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  CreateShortCut "$DESKTOP\XPilot NG.lnk" "$INSTDIR\xpilot-ng-sdl.exe"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\XPilot NG\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\xpilot-ng-sdl.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\xpilot-ng-sdl.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\xpilot-ng-sdl.exe"
  Delete "$INSTDIR\zlib1.dll"
  Delete "$INSTDIR\libexpat-1.dll"
  Delete "$INSTDIR\SDL.dll"
  Delete "$INSTDIR\SDL_ttf.dll"
  Delete "$INSTDIR\SDL_image.dll"

  RMDir /r "$INSTDIR\share"
  RMDir /r "$INSTDIR\doc"
  RMDir /r "$SMPROGRAMS\XPilot NG"
  Delete "$DESKTOP\XPilot NG.lnk"

  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
