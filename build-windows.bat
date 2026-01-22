@echo off
REM Build script for XPilot NG on Windows (using MinGW/MSYS2)
REM This script assumes you're running in MSYS2 MinGW64 shell or have MinGW in PATH

echo ==========================================
echo XPilot NG Build Script for Windows
echo ==========================================
echo.

REM Check for MinGW
where gcc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: gcc not found in PATH
    echo Please install MinGW or MSYS2 and ensure gcc is in PATH
    exit /b 1
)

echo Found gcc compiler
echo.

REM Check for required libraries
echo Checking for required libraries...
if not exist "%MINGW_PREFIX%\include\zlib.h" (
    echo WARNING: zlib not found. Install with: pacman -S mingw-w64-x86_64-zlib
)
if not exist "%MINGW_PREFIX%\include\expat.h" (
    echo WARNING: expat not found. Install with: pacman -S mingw-w64-x86_64-expat
)

REM Configure
echo.
echo Configuring build...
if not exist Makefile (
    configure --enable-sdl-client
    if %ERRORLEVEL% NEQ 0 (
        echo Configuration failed!
        exit /b 1
    )
)

REM Build
echo.
echo Building XPilot NG...
make -j%NUMBER_OF_PROCESSORS%
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b 1
)

echo.
echo ==========================================
echo Build completed successfully!
echo ==========================================
echo.
echo Built binaries:
echo   - Client:  src\client\sdl\xpilot-ng-sdl.exe
echo   - Server:  src\server\xpilot-ng-server.exe
echo   - Replay:  src\replay\xpilot-ng-replay.exe
echo.
echo Note: You may need to copy required DLLs to the executable directory:
echo   - zlib1.dll
echo   - libexpat-1.dll
echo   - SDL.dll, SDL_ttf.dll, SDL_image.dll
echo.
