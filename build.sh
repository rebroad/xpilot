#!/bin/bash
# Build script for XPilot NG on Linux
# This script compiles the server and client for Linux
# Uses out-of-tree build in build-linux/ to avoid conflicts with cross-compilation

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

BUILD_DIR="build-linux"

echo "=========================================="
echo "XPilot NG Build Script for Linux"
echo "=========================================="
echo ""

# Check for required dependencies
echo "Checking dependencies..."
MISSING_DEPS=()

if ! pkg-config --exists x11 2>/dev/null; then
    MISSING_DEPS+=("libx11-dev")
fi

if ! pkg-config --exists zlib 2>/dev/null; then
    MISSING_DEPS+=("zlib1g-dev")
fi

if ! pkg-config --exists expat 2>/dev/null; then
    MISSING_DEPS+=("libexpat1-dev")
fi

if [ ${#MISSING_DEPS[@]} -ne 0 ]; then
    echo "ERROR: Missing required dependencies:"
    for dep in "${MISSING_DEPS[@]}"; do
        echo "  - $dep"
    done
    echo ""
    echo "Please install them with:"
    echo "  sudo apt-get install ${MISSING_DEPS[*]}"
    echo "  (or equivalent for your distribution)"
    exit 1
fi

echo "All dependencies found."
echo ""

# Check for build tools
if ! command -v autoconf >/dev/null 2>&1; then
    echo "ERROR: autoconf not found. Please install it."
    exit 1
fi

if ! command -v automake >/dev/null 2>&1; then
    echo "ERROR: automake not found. Please install it."
    exit 1
fi

if ! command -v make >/dev/null 2>&1; then
    echo "ERROR: make not found. Please install it."
    exit 1
fi

if ! command -v gcc >/dev/null 2>&1; then
    echo "ERROR: gcc not found. Please install it."
    exit 1
fi

# Clean any old in-tree build artifacts that would conflict with out-of-tree builds
if [ -f "$SCRIPT_DIR/src/client/libxpclient.a" ] || \
   find "$SCRIPT_DIR/src" -name "*.o" -type f 2>/dev/null | head -1 | grep -q .; then
    echo "Cleaning old in-tree build artifacts..."
    find "$SCRIPT_DIR/src" -name "*.o" -type f -delete 2>/dev/null || true
    find "$SCRIPT_DIR/src" -name "*.a" -type f -delete 2>/dev/null || true
    find "$SCRIPT_DIR/src" -name "*.lo" -type f -delete 2>/dev/null || true
    find "$SCRIPT_DIR/src" -name ".libs" -type d -exec rm -rf {} + 2>/dev/null || true
    find "$SCRIPT_DIR/src" -name ".deps" -type d -exec rm -rf {} + 2>/dev/null || true
    # Clean config files from source directory
    rm -f "$SCRIPT_DIR/config.status" "$SCRIPT_DIR/config.log" "$SCRIPT_DIR/config.h" \
          "$SCRIPT_DIR/Makefile" "$SCRIPT_DIR/stamp-h1" 2>/dev/null || true
    find "$SCRIPT_DIR" -name "Makefile" -type f ! -path "*/${BUILD_DIR}/*" ! -path "*/build-windows/*" -delete 2>/dev/null || true
    echo "  Done."
    echo ""
fi

# Ensure autotools outputs exist/up-to-date (configure/Makefile.in).
# This repo does not keep generated files in version control.
NEED_AUTOTOOLS=false
if [ ! -f "$SCRIPT_DIR/configure" ]; then
    NEED_AUTOTOOLS=true
fi
# If any Makefile.am is newer than its corresponding Makefile.in, regen.
if [ -f "$SCRIPT_DIR/configure" ] && command -v find >/dev/null 2>&1; then
    if find "$SCRIPT_DIR" -name "Makefile.am" -type f -newer "$SCRIPT_DIR/configure" 2>/dev/null | head -1 | grep -q .; then
        NEED_AUTOTOOLS=true
    fi
fi
if [ "$NEED_AUTOTOOLS" = true ]; then
    echo "Generating autotools files (configure/Makefile.in)..."
    if [ -f "$SCRIPT_DIR/configure.ac" ]; then
        # Copy SDL m4 macro if not present (needed for AM_PATH_SDL)
        if [ ! -f "$SCRIPT_DIR/sdl.m4" ]; then
            for SDL_M4_PATH in \
                /usr/share/aclocal/sdl.m4 \
                /usr/local/share/aclocal/sdl.m4 \
                /usr/x86_64-w64-mingw32/share/aclocal/sdl.m4; do
                if [ -f "$SDL_M4_PATH" ]; then
                    echo "  Copying sdl.m4 from $SDL_M4_PATH..."
                    cp "$SDL_M4_PATH" "$SCRIPT_DIR/"
                    break
                fi
            done
        fi
        aclocal -I . 2>/dev/null || aclocal
        autoconf
        automake --add-missing 2>/dev/null || true
    else
        echo "ERROR: No configure.ac found"
        exit 1
    fi
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure if needed
if [ ! -f Makefile ]; then
    echo "Configuring build..."
    "$SCRIPT_DIR/configure" "$@"
    echo ""
fi

# Build
echo "Building XPilot NG..."
echo "Using $(nproc) parallel jobs"
make -j$(nproc)

echo ""
echo "=========================================="
echo "Build completed successfully!"
echo "=========================================="
echo ""
echo "Built binaries (in $BUILD_DIR/):"
echo "  - Server:  $BUILD_DIR/src/server/xpilot-ng-server"
echo "  - Client:  $BUILD_DIR/src/client/x11/xpilot-ng-x11"
echo "  - Replay:  $BUILD_DIR/src/replay/xpilot-ng-replay"
echo ""
echo "To install system-wide, run:"
echo "  cd $BUILD_DIR && sudo make install"
echo ""
echo "Or reconfigure with a custom prefix:"
echo "  ./build.sh --prefix=/path/to/install"
echo ""
