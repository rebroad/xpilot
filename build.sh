#!/bin/bash
# Build script for XPilot NG on Linux
# This script compiles the server and client for Linux

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

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

# Configure if needed
if [ ! -f Makefile ]; then
    echo "Running configure..."
    ./configure "$@"
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
echo "Built binaries:"
echo "  - Server:  src/server/xpilot-ng-server"
echo "  - Client:  src/client/xpilot-ng-x11"
echo "  - Replay:  src/replay/xpilot-ng-replay"
echo ""
echo "To install system-wide, run:"
echo "  sudo make install"
echo ""
echo "Or install to a custom location:"
echo "  ./configure --prefix=/path/to/install"
echo "  make install"
echo ""
