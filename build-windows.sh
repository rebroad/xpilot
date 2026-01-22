#!/bin/bash
# Build script for XPilot NG Windows client using MinGW cross-compiler
# This script cross-compiles the Windows client

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=========================================="
echo "XPilot NG Windows Cross-Compilation Script"
echo "=========================================="
echo ""

# Detect MinGW cross-compiler
MINGW_CC=""
MINGW_CXX=""
MINGW_PREFIX=""

if command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1; then
    MINGW_CC="x86_64-w64-mingw32-gcc"
    MINGW_CXX="x86_64-w64-mingw32-g++"
    MINGW_PREFIX="x86_64-w64-mingw32"
    TARGET_ARCH="x86_64"
elif command -v i686-w64-mingw32-gcc >/dev/null 2>&1; then
    MINGW_CC="i686-w64-mingw32-gcc"
    MINGW_CXX="i686-w64-mingw32-g++"
    MINGW_PREFIX="i686-w64-mingw32"
    TARGET_ARCH="i686"
else
    echo "ERROR: MinGW cross-compiler not found!"
    echo ""
    echo "Please install MinGW-w64:"
    echo "  Ubuntu/Debian: sudo apt-get install mingw-w64"
    echo "  Fedora:        sudo dnf install mingw64-gcc"
    echo "  Arch:          sudo pacman -S mingw-w64-gcc"
    exit 1
fi

echo "Found MinGW cross-compiler: $MINGW_CC"
echo "Target architecture: $TARGET_ARCH"
echo ""

# Check for Windows libraries
# Note: For a complete Windows build, you'd need Windows versions of:
# - zlib
# - expat
# - X11 (or use SDL client instead)
# - SDL (if building SDL client)

echo "WARNING: Windows cross-compilation requires Windows versions of dependencies."
echo "This script will attempt to configure for Windows, but you may need to:"
echo "  1. Install Windows libraries (zlib, expat) for MinGW"
echo "  2. Use --enable-sdl-client for SDL-based client (recommended for Windows)"
echo ""

# Create a separate build directory for Windows
BUILD_DIR="build-windows"
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning previous Windows build..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$SCRIPT_DIR"

# Configure for Windows
echo "Configuring for Windows ($TARGET_ARCH)..."
echo ""

# Basic configure for Windows - you may need to adjust paths
# Note: This is a simplified version. Full Windows build may require:
# - Windows-specific library paths
# - SDL client (--enable-sdl-client) instead of X11 client
# - Additional configure options

CONFIGURE_OPTS=(
    --host="${MINGW_PREFIX}"
    CC="${MINGW_CC}"
    CXX="${MINGW_CXX}"
)

# Try to enable SDL client if available (better for Windows)
if pkg-config --exists sdl 2>/dev/null || \
   [ -f "/usr/${MINGW_PREFIX}/include/SDL/SDL.h" ] || \
   [ -f "/usr/${MINGW_PREFIX}/sys-root/mingw/include/SDL/SDL.h" ]; then
    echo "SDL found - enabling SDL client (recommended for Windows)"
    CONFIGURE_OPTS+=(--enable-sdl-client)
else
    echo "SDL not found - will try X11 client (may not work on Windows)"
fi

# Run configure from parent directory
cd ..
"$SCRIPT_DIR/configure" "${CONFIGURE_OPTS[@]}" "$@" || {
    echo ""
    echo "=========================================="
    echo "Configuration failed!"
    echo "=========================================="
    echo ""
    echo "For Windows builds, you typically need:"
    echo "  1. MinGW-w64 cross-compiler (installed)"
    echo "  2. Windows libraries in /usr/${MINGW_PREFIX}/"
    echo "  3. SDL libraries for SDL client (recommended)"
    echo ""
    echo "Alternative: Use the Visual Studio project files (.dsp) on Windows"
    echo "or build natively on Windows with MinGW/MSYS2."
    exit 1
}

echo ""
cd "$BUILD_DIR"
echo "Building Windows client..."
make -j$(nproc) || {
    echo ""
    echo "Build failed. You may need to install Windows libraries."
    exit 1
}

echo ""
echo "=========================================="
echo "Windows build completed!"
echo "=========================================="
echo ""
echo "Built binaries are in: $BUILD_DIR/src/client/"
echo ""
echo "Note: You may need to copy DLL dependencies:"
echo "  - zlib1.dll"
echo "  - libexpat-1.dll"
echo "  - SDL.dll, SDL_ttf.dll, SDL_image.dll (if using SDL client)"
echo ""
