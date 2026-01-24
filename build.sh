#!/bin/bash
# Build script for XPilot NG on Linux
# This script compiles the server and client for Linux
# Uses out-of-tree build in build-linux/ to avoid conflicts with cross-compilation

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

BUILD_DIR="build-linux"
FORCE_AUTORECONF=false
FORCE_RECONFIGURE=false
CONFIGURE_ARGS=()

SDL_CLIENT_SETTING="auto" # auto|on|off

while [ $# -gt 0 ]; do
    case "$1" in
        --autoreconf)
            FORCE_AUTORECONF=true
            shift
            ;;
        --reconfigure)
            FORCE_RECONFIGURE=true
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [--autoreconf] [--reconfigure] [configure options...]"
            echo ""
            echo "  --autoreconf    Regenerate autotools files (configure/Makefile.in)"
            echo "  --reconfigure   Rerun ./configure in build-linux/"
            echo ""
            echo "Notes:"
            echo "  - By default, this script will NOT regenerate autotools or rerun configure unless needed."
            echo "  - The build enforces warnings-as-errors via CFLAGS+=' -Werror' at make time."
            exit 0
            ;;
        *)
            CONFIGURE_ARGS+=("$1")
            shift
            ;;
    esac
done

# Determine whether the user explicitly requested SDL client on/off.
# Autoconf accepts: --enable-sdl-client[=yes|no] and --disable-sdl-client
for arg in "${CONFIGURE_ARGS[@]}"; do
    case "$arg" in
	--disable-sdl-client)
	    SDL_CLIENT_SETTING="off"
	    ;;
	--enable-sdl-client)
	    SDL_CLIENT_SETTING="on"
	    ;;
    esac
done

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

# SDL client (optional): this codebase uses SDL 1.2 via sdl-config (AM_PATH_SDL).
SDL_CONFIG_CMD="${SDL_CONFIG:-sdl-config}"
HAVE_SDL_CONFIG=false
if command -v "$SDL_CONFIG_CMD" >/dev/null 2>&1; then
    HAVE_SDL_CONFIG=true
fi

if [ "$SDL_CLIENT_SETTING" = "on" ]; then
    if [ "$HAVE_SDL_CONFIG" != true ]; then
	MISSING_DEPS+=("libsdl1.2-dev (provides sdl-config)")
	MISSING_DEPS+=("libsdl-ttf2.0-dev")
	MISSING_DEPS+=("libsdl-image1.2-dev")
	MISSING_DEPS+=("libgl1-mesa-dev")
	MISSING_DEPS+=("libglu1-mesa-dev")
    fi
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

# If SDL client wasn't explicitly requested, automatically disable it if SDL 1.2
# isn't available. This keeps the server/X11/replay build working on modern
# distros that no longer ship SDL 1.2 by default.
if [ "$SDL_CLIENT_SETTING" = "auto" ] && [ "$HAVE_SDL_CONFIG" != true ]; then
    echo "Note: SDL 1.2 (sdl-config) not found; building without SDL client."
    echo "      Install SDL 1.2 dev packages to enable it, or pass --enable-sdl-client."
    echo ""
    CONFIGURE_ARGS+=(--disable-sdl-client)
    SDL_CLIENT_SETTING="off"
fi

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

# Create build directory early (used for stamp files)
mkdir -p "$BUILD_DIR"
AUTOTOOLS_STAMP="$SCRIPT_DIR/$BUILD_DIR/.autotools.stamp"
CONFIGURE_STAMP="$SCRIPT_DIR/$BUILD_DIR/.configure.stamp"
CONFIGURE_ARGS_FILE="$SCRIPT_DIR/$BUILD_DIR/.configure.args"

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
AUTORECONF_NEEDED=false
if [ "$FORCE_AUTORECONF" = true ] || [ ! -f "$SCRIPT_DIR/configure" ]; then
    AUTORECONF_NEEDED=true
elif [ ! -f "$AUTOTOOLS_STAMP" ]; then
    AUTORECONF_NEEDED=true
elif find "$SCRIPT_DIR" \
	\( -path "$SCRIPT_DIR/$BUILD_DIR" -o -path "$SCRIPT_DIR/$BUILD_DIR/*" \
	   -o -path "$SCRIPT_DIR/build-windows" -o -path "$SCRIPT_DIR/build-windows/*" \
	   -o -path "$SCRIPT_DIR/autom4te.cache" -o -path "$SCRIPT_DIR/autom4te.cache/*" \
	   -o -path "$SCRIPT_DIR/.git" -o -path "$SCRIPT_DIR/.git/*" \) -prune -o \
	\( -name "configure.ac" -o -name "Makefile.am" -o \( -name "*.m4" ! -name "aclocal.m4" \) \) \
	-newer "$AUTOTOOLS_STAMP" -print -quit 2>/dev/null | grep -q .; then
    AUTORECONF_NEEDED=true
fi

if [ "$AUTORECONF_NEEDED" = true ]; then
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
        # Use a conservative autotools sequence to work across older distros.
        # Also force-refresh config/missing to avoid the '--is-lightweight' warning.
        aclocal -I .
        autoconf
        autoheader
        automake --add-missing --copy --force-missing
	# Autotools may avoid rewriting files when output is identical.
	# Touch a stamp so we don't loop on mtime comparisons.
	touch "$AUTOTOOLS_STAMP"
    else
        echo "ERROR: No configure.ac found"
        exit 1
    fi
fi

cd "$BUILD_DIR"

# Configure if needed (or explicitly requested)
CONFIGURE_NEEDED=false
if [ "$FORCE_RECONFIGURE" = true ] || [ "$AUTORECONF_NEEDED" = true ] || [ ! -f Makefile ] || [ ! -f config.status ]; then
    CONFIGURE_NEEDED=true
elif [ ! -f "$CONFIGURE_STAMP" ]; then
    CONFIGURE_NEEDED=true
elif [ "$SCRIPT_DIR/configure" -nt "$CONFIGURE_STAMP" ]; then
    CONFIGURE_NEEDED=true
elif find "$SCRIPT_DIR" -name "Makefile.in" -newer "$CONFIGURE_STAMP" 2>/dev/null | head -1 | grep -q .; then
    CONFIGURE_NEEDED=true
elif [ -f "$CONFIGURE_ARGS_FILE" ]; then
    if ! printf '%s\n' "${CONFIGURE_ARGS[@]}" | cmp -s - "$CONFIGURE_ARGS_FILE"; then
	CONFIGURE_NEEDED=true
    fi
else
    CONFIGURE_NEEDED=true
fi

if [ "$CONFIGURE_NEEDED" = true ]; then
    echo "Configuring build..."
    "$SCRIPT_DIR/configure" "${CONFIGURE_ARGS[@]}"
    printf '%s\n' "${CONFIGURE_ARGS[@]}" > "$CONFIGURE_ARGS_FILE"
    touch "$CONFIGURE_STAMP"
    echo ""
fi

# Build
echo "Building XPilot NG..."
echo "Using $(nproc) parallel jobs"
# Enforce warnings-as-errors at build time (without impacting configure tests).
make -j"$(nproc)" CFLAGS="${CFLAGS:-} -Werror"

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
