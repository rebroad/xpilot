#!/bin/bash
# Build script for XPilot NG Windows client using MinGW cross-compiler
# This script automatically handles all dependencies and cross-compiles the Windows client

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=========================================="
echo "XPilot NG Windows Cross-Compilation"
echo "=========================================="
echo ""

# Cache sudo password for unattended operation
if command -v sudo >/dev/null 2>&1; then
    echo "Caching sudo credentials..."
    sudo -v
    # Keep sudo alive for the duration of the script
    while true; do
        sudo -n true
        sleep 60
        kill -0 "$$" || exit
    done 2>/dev/null &
    SUDO_PID=$!
fi

# Cleanup function for exit
cleanup_on_exit() {
    # Kill sudo keepalive
    if [ -n "$SUDO_PID" ]; then
        kill "$SUDO_PID" 2>/dev/null || true
    fi
}
trap cleanup_on_exit EXIT

# Helper function to install packages across different package managers
# Usage: install_package <apt_pkg> [dnf_pkg] [pacman_pkg]
# If dnf_pkg or pacman_pkg not specified, uses apt_pkg name
install_package() {
    local apt_pkg="$1"
    local dnf_pkg="${2:-$apt_pkg}"
    local pacman_pkg="${3:-$apt_pkg}"

    if command -v apt-get >/dev/null 2>&1; then
        sudo apt-get install -y -qq "$apt_pkg" >/dev/null 2>&1 || return 1
    elif command -v dnf >/dev/null 2>&1; then
        sudo dnf install -y -q "$dnf_pkg" >/dev/null 2>&1 || return 1
    elif command -v pacman >/dev/null 2>&1; then
        sudo pacman -S --noconfirm "$pacman_pkg" >/dev/null 2>&1 || return 1
    else
        return 1
    fi
    return 0
}

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
    echo "MinGW cross-compiler not found. Installing..."
    # Update package lists first for apt
    if command -v apt-get >/dev/null 2>&1; then
        sudo apt-get update -qq
    fi
    if ! install_package "mingw-w64" "mingw64-gcc" "mingw-w64-gcc"; then
        echo "ERROR: Cannot auto-install MinGW. Package manager not found or installation failed."
        exit 1
    fi

    # Try again after installation
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
        echo "ERROR: MinGW installation failed or compiler not in PATH"
        exit 1
    fi
fi

# Set up paths
MINGW_SYSROOT="/usr/${MINGW_PREFIX}/sys-root/mingw"
MINGW_PREFIX_DIR="/usr/${MINGW_PREFIX}"
MINGW_INCLUDE="${MINGW_PREFIX_DIR}/include"
MINGW_LIB="${MINGW_PREFIX_DIR}/lib"

# Function to check if a library exists
check_library() {
    local header=$1
    if [ -f "${MINGW_INCLUDE}/${header}" ] || [ -f "${MINGW_SYSROOT}/include/${header}" ]; then
        return 0
    fi
    return 1
}

# Shared build directory in parent (reusable by other projects)
SHARED_BUILD_DIR="$SCRIPT_DIR/../mingw-build-${MINGW_PREFIX}"
mkdir -p "$SHARED_BUILD_DIR"

# Function to build zlib for Windows
build_zlib() {
    echo "Building zlib for Windows..."
    cd "$SHARED_BUILD_DIR"

    ZLIB_TAR="zlib-1.3.1.tar.gz"
    ZLIB_DIR="zlib-1.3.1"

    if [ ! -f "$ZLIB_TAR" ]; then
        echo "  Downloading zlib..."
        wget -q https://zlib.net/zlib-1.3.1.tar.gz || {
            echo "  ERROR: Failed to download zlib"
            return 1
        }
    fi

    if [ ! -d "$ZLIB_DIR" ]; then
        tar xzf "$ZLIB_TAR"
    fi

    cd "$ZLIB_DIR"

    echo "  Compiling zlib..."
    make -f win32/Makefile.gcc \
        PREFIX="${MINGW_PREFIX}-" \
        BINARY_PATH="${MINGW_PREFIX_DIR}/bin" \
        INCLUDE_PATH="${MINGW_INCLUDE}" \
        LIBRARY_PATH="${MINGW_LIB}" \
        SHARED_MODE=1 \
        clean all >/dev/null 2>&1

    echo "  Installing zlib..."
    sudo make -f win32/Makefile.gcc \
        PREFIX="${MINGW_PREFIX}-" \
        BINARY_PATH="${MINGW_PREFIX_DIR}/bin" \
        INCLUDE_PATH="${MINGW_INCLUDE}" \
        LIBRARY_PATH="${MINGW_LIB}" \
        SHARED_MODE=1 \
        install >/dev/null 2>&1

    echo "  ✓ zlib installed"
}

# Function to build libpng for Windows
build_libpng() {
    echo "Building libpng for Windows..."
    cd "$SHARED_BUILD_DIR"

    LIBPNG_TAR="libpng-1.6.43.tar.xz"
    LIBPNG_DIR="libpng-1.6.43"

    if [ ! -f "$LIBPNG_TAR" ]; then
        echo "  Downloading libpng..."
        wget -q "https://download.sourceforge.net/libpng/$LIBPNG_TAR" || {
            echo "  ERROR: Failed to download libpng"
            return 1
        }
    fi

    if [ ! -d "$LIBPNG_DIR" ]; then
        echo "  Extracting libpng..."
        tar xJf "$LIBPNG_TAR" || {
            echo "  ERROR: Failed to extract libpng"
            return 1
        }
    fi

    cd "$LIBPNG_DIR"

    # Check if already built
    if [ -f ".libs/libpng16.a" ] || [ -f ".libs/libpng.a" ]; then
        echo "  libpng already built, reinstalling..."
        sudo make install >"$SHARED_BUILD_DIR/libpng-install.log" 2>&1 || true
        if [ -f "${MINGW_LIB}/libpng.a" ] || [ -f "${MINGW_LIB}/libpng16.a" ]; then
            echo "  ✓ libpng installed"
            cd "$SCRIPT_DIR"
            return 0
        fi
    fi

    echo "  Configuring libpng..."
    # libpng needs to find zlib
    export CPPFLAGS="-I${MINGW_INCLUDE}"
    export LDFLAGS="-L${MINGW_LIB}"
    export CFLAGS="-I${MINGW_INCLUDE}"

    ./configure \
        --host="${MINGW_PREFIX}" \
        --prefix="${MINGW_PREFIX_DIR}" \
        --disable-shared \
        --enable-static \
        --quiet >"$SHARED_BUILD_DIR/libpng-configure.log" 2>&1 || {
        echo "  ERROR: libpng configuration failed"
        cat "$SHARED_BUILD_DIR/libpng-configure.log" | tail -20
        return 1
    }

    echo "  Compiling libpng..."
    make -j$(nproc) >"$SHARED_BUILD_DIR/libpng-build.log" 2>&1 || {
        echo "  ERROR: libpng compilation failed"
        cat "$SHARED_BUILD_DIR/libpng-build.log" | tail -20
        return 1
    }

    echo "  Installing libpng..."
    sudo make install >"$SHARED_BUILD_DIR/libpng-install.log" 2>&1 || {
        echo "  ERROR: libpng installation failed"
        return 1
    }

    # Create symlinks for compatibility (some code expects libpng.a, others libpng16.a)
    if [ -f "${MINGW_LIB}/libpng16.a" ] && [ ! -f "${MINGW_LIB}/libpng.a" ]; then
        sudo ln -sf libpng16.a "${MINGW_LIB}/libpng.a"
    fi
    if [ -f "${MINGW_INCLUDE}/libpng16/png.h" ] && [ ! -f "${MINGW_INCLUDE}/png.h" ]; then
        sudo ln -sf libpng16/png.h "${MINGW_INCLUDE}/png.h"
        sudo ln -sf libpng16/pngconf.h "${MINGW_INCLUDE}/pngconf.h"
        sudo ln -sf libpng16/pnglibconf.h "${MINGW_INCLUDE}/pnglibconf.h"
    fi

    echo "  ✓ libpng installed"
    cd "$SCRIPT_DIR"
}

# Function to build expat for Windows
build_expat() {
    echo "Building expat for Windows..."
    BUILD_TMP_DIR="$SCRIPT_DIR/.mingw-build"
    mkdir -p "$BUILD_TMP_DIR"
    cd "$BUILD_TMP_DIR"

    if [ ! -f expat-2.6.3.tar.gz ]; then
        echo "  Downloading expat..."
        wget -q https://github.com/libexpat/libexpat/releases/download/R_2_6_3/expat-2.6.3.tar.gz || {
            echo "  ERROR: Failed to download expat"
            return 1
        }
    fi

    if [ ! -d expat-2.6.3 ]; then
        tar xzf expat-2.6.3.tar.gz || {
            echo "  ERROR: Failed to extract expat"
            return 1
        }
    fi

    cd expat-2.6.3 || {
        echo "  ERROR: Failed to enter expat directory"
        return 1
    }

    # Make sure configure exists and is executable
    if [ ! -f configure ]; then
        if [ -f buildconf.sh ]; then
            echo "  Running buildconf..."
            bash buildconf.sh >/dev/null 2>&1 || {
                echo "  ERROR: buildconf failed"
                return 1
            }
        else
            echo "  ERROR: configure script not found"
            return 1
        fi
    fi

    chmod +x configure

    echo "  Configuring expat..."
    ./configure \
        --host="${MINGW_PREFIX}" \
        --prefix="${MINGW_PREFIX_DIR}" \
        --disable-shared \
        --enable-static \
        --quiet >"$BUILD_TMP_DIR/expat-configure.log" 2>&1 || {
        echo "  ERROR: expat configuration failed. Check $BUILD_TMP_DIR/expat-configure.log"
        cat "$BUILD_TMP_DIR/expat-configure.log" | tail -20
        return 1
    }

    echo "  Compiling expat..."
    make -j$(nproc) >"$BUILD_TMP_DIR/expat-build.log" 2>&1 || {
        echo "  ERROR: expat compilation failed. Check $BUILD_TMP_DIR/expat-build.log"
        cat "$BUILD_TMP_DIR/expat-build.log" | tail -20
        return 1
    }

    echo "  Installing expat..."
    sudo make install >"$BUILD_TMP_DIR/expat-install.log" 2>&1 || {
        echo "  ERROR: expat installation failed. Check $BUILD_TMP_DIR/expat-install.log"
        return 1
    }

    echo "  ✓ expat installed"
}

# Check and build dependencies automatically
if ! check_library "zlib.h"; then
    build_zlib || {
        echo "ERROR: Failed to build zlib"
        exit 1
    }
fi

# Build libpng (depends on zlib) - needed for SDL_image PNG support
if ! check_library "png.h"; then
    build_libpng || {
        echo "ERROR: Failed to build libpng"
        exit 1
    }
fi

if ! check_library "expat.h"; then
    build_expat || {
        echo "ERROR: Failed to build expat"
        exit 1
    }
fi

# Check for SDL in various locations
SDL_AVAILABLE=false
SDL_PREFIX=""

# Check MinGW system location first
if check_library "SDL/SDL.h"; then
    SDL_AVAILABLE=true
    SDL_PREFIX="${MINGW_PREFIX_DIR}"
fi

# Check parent directory for existing SDL build
if [ "$SDL_AVAILABLE" = false ] && [ -d "$SCRIPT_DIR/../SDL" ]; then
    SDL_SRC_DIR="$SCRIPT_DIR/../SDL"
    # Check if it's configured for MinGW
    if [ -f "$SDL_SRC_DIR/config.status" ]; then
        # Check if it was configured for our target
        if grep -q "host=${MINGW_PREFIX}" "$SDL_SRC_DIR/config.status" 2>/dev/null; then
            SDL_AVAILABLE=true
            SDL_PREFIX="$SDL_SRC_DIR"
            echo "Found existing SDL build in ../SDL"
        fi
    fi
fi

# Determine if SDL client should be enabled
ENABLE_SDL=false
for arg in "$@"; do
    if [ "$arg" = "--enable-sdl-client" ]; then
        ENABLE_SDL=true
        break
    fi
done

# Auto-enable SDL if available and not explicitly disabled
if [ "$ENABLE_SDL" = false ] && [ "$SDL_AVAILABLE" = true ]; then
    # Check if --disable-sdl-client was explicitly passed
    DISABLE_SDL=false
    for arg in "$@"; do
        if [ "$arg" = "--disable-sdl-client" ]; then
            DISABLE_SDL=true
            break
        fi
    done
    if [ "$DISABLE_SDL" = false ]; then
        ENABLE_SDL=true
    fi
fi

# Create build directory
BUILD_DIR="build-windows"

# Clean any leftover config in source directory FIRST (autotools complains if source is configured)
if [ -f "$SCRIPT_DIR/config.status" ] || [ -f "$SCRIPT_DIR/Makefile" ]; then
    echo "Cleaning leftover config in source directory..."
    # Don't use make distclean as it might trigger configure - just remove files directly
    rm -f "$SCRIPT_DIR/config.status" "$SCRIPT_DIR/config.log" "$SCRIPT_DIR/config.h" \
          "$SCRIPT_DIR/Makefile" "$SCRIPT_DIR/stamp-h1" 2>/dev/null
    # Remove generated Makefiles in subdirectories (but not in build-windows if it exists)
    find "$SCRIPT_DIR" -name "Makefile" -type f ! -path "*/${BUILD_DIR}/*" -delete 2>/dev/null || true
fi

# Check for old in-tree build artifacts - VPATH will find these
# Both Linux and Windows builds should use out-of-tree directories now
if find "$SCRIPT_DIR/src" -name "*.o" -type f 2>/dev/null | head -1 | grep -q .; then
    echo "Cleaning old in-tree build artifacts (use ./build.sh for Linux builds)..."
    find "$SCRIPT_DIR/src" -name "*.o" -type f -delete 2>/dev/null || true
    find "$SCRIPT_DIR/src" -name "*.a" -type f -delete 2>/dev/null || true
    find "$SCRIPT_DIR/src" -name "*.lo" -type f -delete 2>/dev/null || true
    find "$SCRIPT_DIR/src" -name ".libs" -type d -exec rm -rf {} + 2>/dev/null || true
    find "$SCRIPT_DIR/src" -name ".deps" -type d -exec rm -rf {} + 2>/dev/null || true
fi


# Now clean/create build directory
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning previous Windows build..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure for Windows
# Set library paths so configure can find the libraries we built
# Note: CPPFLAGS must include the parent include directory (without /SDL)
# because configure checks for "SDL/SDL_ttf.h"
export LDFLAGS="-L${MINGW_LIB}"
# -D_WINDOWS enables Windows-specific code paths in XPilot source
# Also add build directory's src/common for generated headers like version.h
# -DHAVE_GETTIMEOFDAY tells XPilot code that MinGW provides gettimeofday
BUILD_DIR_ABS="${SCRIPT_DIR}/${BUILD_DIR}"
export CPPFLAGS="-I${MINGW_INCLUDE} -I${MINGW_INCLUDE}/SDL -D_WINDOWS -DWIN32 -DHAVE_GETTIMEOFDAY -I${BUILD_DIR_ABS}/src/common"
export CFLAGS="-I${MINGW_INCLUDE} -I${MINGW_INCLUDE}/SDL -D_WINDOWS -DWIN32 -DHAVE_GETTIMEOFDAY -I${BUILD_DIR_ABS}/src/common"
# -Werror will be added during make, not configure (configure tests may have warnings)
export LIBS="-lSDL_ttf -lSDL_image -lfreetype -lpng16 -lz"
export PKG_CONFIG_PATH="${MINGW_LIB}/pkgconfig:${PKG_CONFIG_PATH:-}"

# Get SDL_CFLAGS but remove -Dmain=SDL_main which breaks configure tests
# (configure's test programs have main() with no args, but SDL_main requires argc/argv)
SDL_CFLAGS_RAW="$(/usr/x86_64-w64-mingw32/bin/sdl-config --cflags 2>/dev/null || echo '')"
export SDL_CFLAGS="${SDL_CFLAGS_RAW//-Dmain=SDL_main/}"

CONFIGURE_OPTS=(
    --host="${MINGW_PREFIX}"
    CC="${MINGW_CC}"
    CXX="${MINGW_CXX}"
    CPPFLAGS="${CPPFLAGS}"
    CFLAGS="${CFLAGS}"
    LDFLAGS="${LDFLAGS}"
    LIBS="${LIBS}"
    SDL_CFLAGS="${SDL_CFLAGS}"
    # Tell configure that socklen_t is available (MinGW provides it)
    # This prevents config.h from defining socklen_t which conflicts with xpcommon.h
    ac_cv_type_socklen_t=yes
)

# Add SDL paths if using local SDL installation
if [ -n "$SDL_PREFIX" ] && [ "$SDL_PREFIX" != "${MINGW_PREFIX_DIR}" ]; then
    export LDFLAGS="-L${SDL_PREFIX}/lib $LDFLAGS"
    export CPPFLAGS="-I${SDL_PREFIX}/include $CPPFLAGS"
    export PKG_CONFIG_PATH="${SDL_PREFIX}/lib/pkgconfig:${PKG_CONFIG_PATH}"
    # Find sdl-config
    if [ -f "${SDL_PREFIX}/bin/sdl-config" ]; then
        export SDL_CONFIG="${SDL_PREFIX}/bin/sdl-config"
    elif [ -f "${SDL_PREFIX}/sdl-config" ]; then
        export SDL_CONFIG="${SDL_PREFIX}/sdl-config"
    fi
    export PATH="${SDL_PREFIX}/bin:${PATH}"
fi

# Also check if SDL was installed to MinGW system location
if [ -f "${MINGW_PREFIX_DIR}/bin/sdl-config" ]; then
    export SDL_CONFIG="${MINGW_PREFIX_DIR}/bin/sdl-config"
    export PATH="${MINGW_PREFIX_DIR}/bin:${PATH}"
fi

# For Windows, we must use SDL client (X11 doesn't work on Windows)
# Force enable SDL client for Windows builds
if [ "$ENABLE_SDL" = false ]; then
    echo "SDL client required for Windows. Enabling SDL client..."
    ENABLE_SDL=true
fi

# Build SDL for Windows if not available
if [ "$SDL_AVAILABLE" = false ]; then
    echo "Building SDL for Windows..."
    cd "$SHARED_BUILD_DIR"

    SDL_TAR="SDL-1.2.15.tar.gz"
    SDL_DIR="SDL-1.2.15"

    if [ ! -f "$SDL_TAR" ]; then
        echo "  Downloading SDL..."
        wget -q https://www.libsdl.org/release/SDL-1.2.15.tar.gz || {
            echo "  ERROR: Failed to download SDL"
            exit 1
        }
    fi

    if [ ! -d "$SDL_DIR" ]; then
        tar xzf "$SDL_TAR"
    fi

    cd "$SDL_DIR"

    # Check if already built
    if [ -f "config.status" ] && [ -f ".libs/libSDL.a" ] || [ -f "libSDL.a" ]; then
        echo "  SDL already built, reinstalling..."
        sudo make install >"$SHARED_BUILD_DIR/sdl-install.log" 2>&1 || {
            if check_library "SDL/SDL.h"; then
                echo "  SDL already installed"
                SDL_AVAILABLE=true
                cd "$SCRIPT_DIR"
            fi
        }
    fi

    echo "  Configuring SDL..."
    ./configure \
        --host="${MINGW_PREFIX}" \
        --prefix="${MINGW_PREFIX_DIR}" \
        --disable-shared \
        --enable-static \
        --quiet >"$SHARED_BUILD_DIR/sdl-configure.log" 2>&1 || {
        echo "  ERROR: SDL configuration failed"
        cat "$SHARED_BUILD_DIR/sdl-configure.log" | tail -20
        exit 1
    }

    echo "  Compiling SDL..."
    make -j$(nproc) >"$SHARED_BUILD_DIR/sdl-build.log" 2>&1 || {
        echo "  ERROR: SDL compilation failed"
        cat "$SHARED_BUILD_DIR/sdl-build.log" | tail -20
        exit 1
    }

    echo "  Installing SDL..."
    sudo make install >"$SHARED_BUILD_DIR/sdl-install.log" 2>&1 || {
        echo "  ERROR: SDL installation failed"
        exit 1
    }

    echo "  ✓ SDL installed"
    SDL_AVAILABLE=true
    cd "$SCRIPT_DIR"
fi

# Check for FreeType2 - it can be in include/ or include/freetype2/
FREETYPE_AVAILABLE=false
FREETYPE_HEADER=""
FREETYPE_LIB=""

# Check various locations
for loc in "${MINGW_INCLUDE}" "${MINGW_SYSROOT}/include"; do
    if [ -f "${loc}/ft2build.h" ]; then
        FREETYPE_AVAILABLE=true
        FREETYPE_HEADER="${loc}"
        break
    elif [ -f "${loc}/freetype2/ft2build.h" ]; then
        FREETYPE_AVAILABLE=true
        FREETYPE_HEADER="${loc}/freetype2"
        break
    fi
done

# Check for library
if [ -f "${MINGW_LIB}/libfreetype.a" ] || [ -f "${MINGW_LIB}/libfreetype.dll.a" ]; then
    FREETYPE_LIB="${MINGW_LIB}"
fi

# Build FreeType2 for Windows if not available (required by SDL_ttf)
if [ "$FREETYPE_AVAILABLE" = false ] || [ -z "$FREETYPE_LIB" ]; then
    echo "Building FreeType2 for Windows..."
    cd "$SHARED_BUILD_DIR"

    FREETYPE_TAR="freetype-2.13.2.tar.xz"
    FREETYPE_DIR="freetype-2.13.2"

    if [ ! -f "$FREETYPE_TAR" ]; then
        echo "  Downloading FreeType2..."
        wget -q https://download.savannah.gnu.org/releases/freetype/freetype-2.13.2.tar.xz || {
            echo "  ERROR: Failed to download FreeType2"
            exit 1
        }
    fi

    if [ ! -d "$FREETYPE_DIR" ]; then
        tar xJf "$FREETYPE_TAR"
    fi

    cd "$FREETYPE_DIR"

    # Check if already configured and built (check for either static or shared lib)
    if [ -f "config.status" ] && { [ -f "objs/.libs/libfreetype.a" ] || [ -f "objs/.libs/libfreetype.dll.a" ]; }; then
        echo "  FreeType2 already built, reinstalling..."
        sudo make install >"$SHARED_BUILD_DIR/freetype-install.log" 2>&1 || {
            echo "  Reinstalling failed, checking if already installed..."
            # Check if it's actually installed
            if [ -f "${MINGW_INCLUDE}/ft2build.h" ] || [ -f "${MINGW_INCLUDE}/freetype2/ft2build.h" ]; then
                echo "  FreeType2 already installed"
                FREETYPE_AVAILABLE=true
                cd "$SCRIPT_DIR"
                return 0
            fi
        }
    fi

    # Use meson for FreeType - it handles cross-compilation properly
    MESON_BUILD_DIR="build-mingw"
    if [ ! -f "$MESON_BUILD_DIR/libfreetype.a" ]; then
        echo "  Configuring FreeType2 with meson..."
        rm -rf "$MESON_BUILD_DIR"

        # Create meson cross-file for MinGW
        CROSS_FILE="$SHARED_BUILD_DIR/mingw-cross.txt"
        cat > "$CROSS_FILE" << CROSSEOF
[binaries]
c = '${MINGW_PREFIX}-gcc'
cpp = '${MINGW_PREFIX}-g++'
ar = '${MINGW_PREFIX}-ar'
strip = '${MINGW_PREFIX}-strip'
windres = '${MINGW_PREFIX}-windres'

[host_machine]
system = 'windows'
cpu_family = 'x86_64'
cpu = 'x86_64'
endian = 'little'
CROSSEOF

        # Run meson with clean environment to avoid XPilot's exported vars
        (
            unset CFLAGS CPPFLAGS LDFLAGS LIBS
            meson setup "$MESON_BUILD_DIR" \
                --cross-file="$CROSS_FILE" \
                --prefix="${MINGW_PREFIX_DIR}" \
                --libdir="${MINGW_LIB}" \
                --includedir="${MINGW_INCLUDE}" \
                --default-library=static \
                -Dzlib=disabled \
                -Dbzip2=disabled \
                -Dpng=disabled \
                -Dharfbuzz=disabled \
                -Dbrotli=disabled
        ) >"$SHARED_BUILD_DIR/freetype-configure.log" 2>&1
        FT_CONFIG_STATUS=$?

        if [ $FT_CONFIG_STATUS -ne 0 ]; then
            echo "  ERROR: FreeType2 meson configuration failed"
            cat "$SHARED_BUILD_DIR/freetype-configure.log" | tail -30
            exit 1
        fi

        echo "  Compiling FreeType2..."
        ninja -C "$MESON_BUILD_DIR" >"$SHARED_BUILD_DIR/freetype-build.log" 2>&1 || {
            echo "  ERROR: FreeType2 compilation failed"
            cat "$SHARED_BUILD_DIR/freetype-build.log" | tail -30
            exit 1
        }

        # Verify the library is PE/COFF format (Windows), not ELF (Linux)
        if [ -f "$MESON_BUILD_DIR/libfreetype.a" ]; then
            OBJ_FORMAT=$(x86_64-w64-mingw32-objdump -a "$MESON_BUILD_DIR/libfreetype.a" 2>/dev/null | head -10)
            if echo "$OBJ_FORMAT" | grep -q "elf64"; then
                echo "  ERROR: FreeType2 was built with wrong compiler (ELF instead of PE/COFF)"
                exit 1
            else
                echo "  FreeType2 built successfully (PE/COFF format verified)"
            fi
        fi
    fi

    echo "  Installing FreeType2..."
    # Install using ninja (meson build)
    sudo ninja -C "$MESON_BUILD_DIR" install >"$SHARED_BUILD_DIR/freetype-install.log" 2>&1 || {
        echo "  ERROR: FreeType2 installation failed"
        cat "$SHARED_BUILD_DIR/freetype-install.log" | tail -10
        exit 1
    }

    # Verify the installed library is correct format (COFF/PE, not ELF)
    if [ -f "${MINGW_LIB}/libfreetype.a" ]; then
        LIB_FORMAT=$(x86_64-w64-mingw32-objdump -a "${MINGW_LIB}/libfreetype.a" 2>/dev/null | head -5)
        if echo "$LIB_FORMAT" | grep -q "elf64"; then
            echo "  ERROR: Installed FreeType library is in wrong format (ELF instead of PE/COFF)"
            exit 1
        else
            echo "  FreeType2 installed successfully"
        fi
    fi

    # FreeType2 installs to freetype2/ but some code expects freetype/
    # Create symlink for compatibility
    if [ -d "${MINGW_INCLUDE}/freetype2" ] && [ ! -d "${MINGW_INCLUDE}/freetype" ]; then
        echo "  Creating freetype symlink for compatibility..."
        sudo ln -sf freetype2 "${MINGW_INCLUDE}/freetype" 2>/dev/null || true
    fi

    if [ -f "${MINGW_INCLUDE}/ft2build.h" ]; then
        FREETYPE_HEADER="${MINGW_INCLUDE}"
        FREETYPE_AVAILABLE=true
    elif [ -f "${MINGW_INCLUDE}/freetype2/ft2build.h" ]; then
        FREETYPE_HEADER="${MINGW_INCLUDE}/freetype2"
        FREETYPE_AVAILABLE=true
        # Create ft2build.h in include/ for SDL_ttf
        if [ ! -f "${MINGW_INCLUDE}/ft2build.h" ]; then
            sudo cp "${MINGW_INCLUDE}/freetype2/ft2build.h" "${MINGW_INCLUDE}/ft2build.h" 2>/dev/null || true
        fi
    else
        echo "  ERROR: FreeType2 headers not found after installation"
        exit 1
    fi

    # Verify library was installed
    if [ -f "${MINGW_LIB}/libfreetype.a" ] || [ -f "${MINGW_LIB}/libfreetype.dll.a" ]; then
        FREETYPE_LIB="${MINGW_LIB}"
    fi

    echo "  ✓ FreeType2 installed"
    cd "$SCRIPT_DIR"
fi

# Check for SDL_ttf
SDL_TTF_AVAILABLE=false
if check_library "SDL/SDL_ttf.h"; then
    SDL_TTF_AVAILABLE=true
fi

# Build SDL_ttf for Windows if not available
if [ "$SDL_TTF_AVAILABLE" = false ]; then
    echo "Building SDL_ttf for Windows..."
    cd "$SHARED_BUILD_DIR"

    SDL_TTF_TAR="SDL_ttf-2.0.11.tar.gz"
    SDL_TTF_DIR="SDL_ttf-2.0.11"

    if [ ! -f "$SDL_TTF_TAR" ]; then
        echo "  Downloading SDL_ttf..."
        wget -q https://www.libsdl.org/projects/SDL_ttf/release/SDL_ttf-2.0.11.tar.gz || {
            echo "  ERROR: Failed to download SDL_ttf"
            exit 1
        }
    fi

    if [ ! -d "$SDL_TTF_DIR" ]; then
        tar xzf "$SDL_TTF_TAR"
    fi

    cd "$SDL_TTF_DIR"

    # Check if already built and installed
    if [ -f "config.status" ] && [ -f ".libs/libSDL_ttf.a" ] || [ -f "libSDL_ttf.a" ]; then
        if check_library "SDL/SDL_ttf.h"; then
            echo "  SDL_ttf already installed"
            SDL_TTF_AVAILABLE=true
            cd "$SCRIPT_DIR"
        else
            echo "  SDL_ttf built but not installed, reinstalling..."
            sudo make install >"$SHARED_BUILD_DIR/sdl_ttf-install.log" 2>&1 || true
            if check_library "SDL/SDL_ttf.h"; then
                SDL_TTF_AVAILABLE=true
                cd "$SCRIPT_DIR"
            fi
        fi
    fi

    if [ "$SDL_TTF_AVAILABLE" = false ]; then
        # Clean previous build
        if [ -f "Makefile" ]; then
            make clean >/dev/null 2>&1 || true
        fi
        rm -f config.cache config.status

        echo "  Configuring SDL_ttf..."
    SDL_CFLAGS="$(${SDL_CONFIG:-sdl-config} --cflags 2>/dev/null || echo '-I/usr/x86_64-w64-mingw32/include/SDL')"
    SDL_LIBS="$(${SDL_CONFIG:-sdl-config} --libs 2>/dev/null || echo '-L/usr/x86_64-w64-mingw32/lib -lSDL')"

    # Use the detected FreeType2 header location, or find it
    FT2_INCLUDE="$FREETYPE_HEADER"
    if [ -z "$FT2_INCLUDE" ]; then
        # Search for it
        FT2_FOUND=$(find "${MINGW_PREFIX_DIR}" -name "ft2build.h" -type f 2>/dev/null | head -1)
        if [ -n "$FT2_FOUND" ]; then
            FT2_INCLUDE=$(dirname "$FT2_FOUND")
        else
            echo "  ERROR: FreeType2 headers not found"
            echo "  Searched in: ${MINGW_INCLUDE}, ${MINGW_SYSROOT}/include"
            echo "  Installation may have failed. Check logs in $SHARED_BUILD_DIR"
            exit 1
        fi
    fi

    # SDL_ttf needs both freetype2/ and freetype/ (symlink) in include path
    # Also need parent include for ft2build.h
    export CPPFLAGS="-I${MINGW_INCLUDE}/freetype2 -I${MINGW_INCLUDE}/freetype -I${MINGW_INCLUDE} $CPPFLAGS"
    export CFLAGS="-I${MINGW_INCLUDE}/freetype2 -I${MINGW_INCLUDE}/freetype -I${MINGW_INCLUDE} ${CFLAGS:-}"

    export FT2_CFLAGS="-I${FT2_INCLUDE}"
    # Use FULL PATH to the static library to prevent libtool from finding system freetype
    export FT2_LIBS="${MINGW_LIB}/libfreetype.a"
    export CPPFLAGS="-I${FT2_INCLUDE} $CPPFLAGS"
    export CFLAGS="-I${FT2_INCLUDE} ${CFLAGS:-}"
    export LDFLAGS="-L${MINGW_LIB} $LDFLAGS"

    # Prevent pkg-config from finding system libraries
    export PKG_CONFIG_LIBDIR="${MINGW_LIB}/pkgconfig"
    unset PKG_CONFIG_PATH

    ./configure \
        --host="${MINGW_PREFIX}" \
        --prefix="${MINGW_PREFIX_DIR}" \
        --with-sdl-prefix="${MINGW_PREFIX_DIR}" \
        --with-freetype-prefix="${MINGW_PREFIX_DIR}" \
        SDL_CFLAGS="$SDL_CFLAGS" \
        SDL_LIBS="$SDL_LIBS" \
        FT2_CFLAGS="$FT2_CFLAGS" \
        FT2_LIBS="$FT2_LIBS" \
        --quiet >"$SHARED_BUILD_DIR/sdl_ttf-configure.log" 2>&1 || {
        echo "  ERROR: SDL_ttf configuration failed"
        cat "$SHARED_BUILD_DIR/sdl_ttf-configure.log" | tail -20
        exit 1
    }

    echo "  Compiling SDL_ttf..."

    # For static FreeType2, use --whole-archive to ensure all symbols are included
    # and use FULL PATH to prevent libtool from finding system library
    if [ -f "Makefile" ]; then
        # Replace any -lfreetype with full path to static library
        sed -i "s|-lfreetype|${MINGW_LIB}/libfreetype.a|g" Makefile
        # Also ensure LIBADD has freetype with full path
        if grep -q "^libSDL_ttf_la_LIBADD" Makefile; then
            if ! grep -q "libSDL_ttf_la_LIBADD.*libfreetype" Makefile; then
                sed -i "s|^libSDL_ttf_la_LIBADD =\(.*\)|libSDL_ttf_la_LIBADD =\1 ${MINGW_LIB}/libfreetype.a|" Makefile
            fi
        fi
        # Also add freetype to the test program link lines (showfont, glfont)
        sed -i "s|^showfont_LDADD =\(.*\)|showfont_LDADD =\1 ${MINGW_LIB}/libfreetype.a|" Makefile
        sed -i "s|^glfont_LDADD =\(.*\)|glfont_LDADD =\1 ${MINGW_LIB}/libfreetype.a|" Makefile
    fi

    # Build with explicit library path - prevent system library interference
    export LIBRARY_PATH="${MINGW_LIB}"
    export LDFLAGS="-L${MINGW_LIB}"
    # Prevent libtool from searching system paths
    export lt_cv_sys_lib_search_path_spec="${MINGW_LIB}"

    make -j$(nproc) >"$SHARED_BUILD_DIR/sdl_ttf-build.log" 2>&1 || {
        echo "  ERROR: SDL_ttf compilation failed"
        # Check if it's a library ordering issue - try building static instead
        if grep -q "undefined reference.*FT_" "$SHARED_BUILD_DIR/sdl_ttf-build.log"; then
            echo "  Trying static build instead of shared..."
            # Reconfigure for static build
            ./configure \
                --host="${MINGW_PREFIX}" \
                --prefix="${MINGW_PREFIX_DIR}" \
                --with-sdl-prefix="${MINGW_PREFIX_DIR}" \
                --with-freetype-prefix="${MINGW_PREFIX_DIR}" \
                --disable-shared \
                --enable-static \
                SDL_CFLAGS="$SDL_CFLAGS" \
                SDL_LIBS="$SDL_LIBS" \
                FT2_CFLAGS="$FT2_CFLAGS" \
                FT2_LIBS="$FT2_LIBS" \
                --quiet >"$SHARED_BUILD_DIR/sdl_ttf-static-configure.log" 2>&1

            # Update Makefile again - use full path to static library for all targets
            if [ -f "Makefile" ]; then
                sed -i "s|-lfreetype|${MINGW_LIB}/libfreetype.a|g" Makefile
                if grep -q "^libSDL_ttf_la_LIBADD" Makefile; then
                    if ! grep -q "libSDL_ttf_la_LIBADD.*libfreetype" Makefile; then
                        sed -i "s|^libSDL_ttf_la_LIBADD =\(.*\)|libSDL_ttf_la_LIBADD =\1 ${MINGW_LIB}/libfreetype.a|" Makefile
                    fi
                fi
                # Also add freetype to the test program link lines (showfont, glfont)
                sed -i "s|^showfont_LDADD =\(.*\)|showfont_LDADD =\1 ${MINGW_LIB}/libfreetype.a|" Makefile
                sed -i "s|^glfont_LDADD =\(.*\)|glfont_LDADD =\1 ${MINGW_LIB}/libfreetype.a|" Makefile
            fi

            # Build just the library if full build fails
            make -j$(nproc) >"$SHARED_BUILD_DIR/sdl_ttf-static-build.log" 2>&1 || {
                # If full build fails, try building just the library
                echo "  Full build failed, trying library only..."
                make libSDL_ttf.la >"$SHARED_BUILD_DIR/sdl_ttf-libonly-build.log" 2>&1 || {
                    # Check if library exists anyway
                    if [ -f ".libs/libSDL_ttf.a" ] || [ -f ".libs/libSDL_ttf.dll.a" ]; then
                        echo "  Library built successfully (test programs may have failed)"
                    else
                        cat "$SHARED_BUILD_DIR/sdl_ttf-static-build.log" | grep -E "(error|undefined|FT_)" | tail -10
                        exit 1
                    fi
                }
            }
        else
            cat "$SHARED_BUILD_DIR/sdl_ttf-build.log" | tail -20
            exit 1
        fi
    }

        echo "  Installing SDL_ttf..."
        # Install only the library and headers, not test programs
        # First try standard install, if it fails (due to test programs), install just lib/headers
        sudo make install >"$SHARED_BUILD_DIR/sdl_ttf-install.log" 2>&1 || {
            echo "  Standard install failed, installing library only..."
            # Install library, pkgconfig, and headers manually
            sudo make install-libLTLIBRARIES >"$SHARED_BUILD_DIR/sdl_ttf-install.log" 2>&1 || true
            sudo make install-pkgconfigDATA >>"$SHARED_BUILD_DIR/sdl_ttf-install.log" 2>&1 || true
            # Install header manually if install target doesn't exist
            sudo cp SDL_ttf.h "${MINGW_INCLUDE}/SDL/" 2>/dev/null || true
            # Fix permissions (cp preserves restrictive perms from build dir)
            sudo chmod 644 "${MINGW_INCLUDE}/SDL/SDL_ttf.h" 2>/dev/null || true
            # Verify installation
            if [ ! -f "${MINGW_INCLUDE}/SDL/SDL_ttf.h" ]; then
                echo "  ERROR: SDL_ttf header not installed"
                exit 1
            fi
            if [ ! -f "${MINGW_LIB}/libSDL_ttf.a" ] && [ ! -f "${MINGW_LIB}/libSDL_ttf.dll.a" ]; then
                echo "  ERROR: SDL_ttf library not installed"
                exit 1
            fi
        }

        echo "  ✓ SDL_ttf installed"
        SDL_TTF_AVAILABLE=true
        cd "$SCRIPT_DIR"
    fi
fi

# Check for SDL_image
SDL_IMAGE_AVAILABLE=false
if check_library "SDL/SDL_image.h"; then
    SDL_IMAGE_AVAILABLE=true
fi

# Build SDL_image for Windows if not available
if [ "$SDL_IMAGE_AVAILABLE" = false ]; then
    echo "Building SDL_image for Windows..."
    cd "$SHARED_BUILD_DIR"

    SDL_IMAGE_TAR="SDL_image-1.2.12.tar.gz"
    SDL_IMAGE_DIR="SDL_image-1.2.12"

    if [ ! -f "$SDL_IMAGE_TAR" ]; then
        echo "  Downloading SDL_image..."
        wget -q https://www.libsdl.org/projects/SDL_image/release/SDL_image-1.2.12.tar.gz || {
            echo "  ERROR: Failed to download SDL_image"
            exit 1
        }
    fi

    if [ ! -d "$SDL_IMAGE_DIR" ]; then
        tar xzf "$SDL_IMAGE_TAR"
    fi

    cd "$SDL_IMAGE_DIR"

    # Check if already built
    if [ -f "config.status" ] && [ -f ".libs/libSDL_image.a" ] || [ -f "libSDL_image.a" ]; then
        echo "  SDL_image already built, reinstalling..."
        sudo make install >"$SHARED_BUILD_DIR/sdl_image-install.log" 2>&1 || {
            if check_library "SDL/SDL_image.h"; then
                echo "  SDL_image already installed"
                SDL_IMAGE_AVAILABLE=true
                cd "$SCRIPT_DIR"
            fi
        }
    fi

    echo "  Configuring SDL_image..."
    SDL_CFLAGS="$(${SDL_CONFIG:-sdl-config} --cflags 2>/dev/null || echo '-I/usr/x86_64-w64-mingw32/include/SDL')"
    SDL_LIBS="$(${SDL_CONFIG:-sdl-config} --libs 2>/dev/null || echo '-L/usr/x86_64-w64-mingw32/lib -lSDL')"

    # Set up libpng paths for SDL_image
    export CPPFLAGS="-I${MINGW_INCLUDE} -I${MINGW_INCLUDE}/libpng16"
    export LDFLAGS="-L${MINGW_LIB}"
    export PNG_CFLAGS="-I${MINGW_INCLUDE} -I${MINGW_INCLUDE}/libpng16"
    export PNG_LIBS="-L${MINGW_LIB} -lpng16 -lz"

    ./configure \
        --host="${MINGW_PREFIX}" \
        --prefix="${MINGW_PREFIX_DIR}" \
        --with-sdl-prefix="${MINGW_PREFIX_DIR}" \
        --disable-webp \
        --enable-png \
        --disable-png-shared \
        --disable-jpg \
        --disable-tif \
        --enable-bmp \
        --enable-gif \
        --enable-pcx \
        --enable-pnm \
        --enable-tga \
        --enable-xcf \
        --enable-xpm \
        SDL_CFLAGS="$SDL_CFLAGS" \
        SDL_LIBS="$SDL_LIBS" \
        PNG_CFLAGS="$PNG_CFLAGS" \
        PNG_LIBS="$PNG_LIBS" \
        --quiet >"$SHARED_BUILD_DIR/sdl_image-configure.log" 2>&1 || {
        echo "  ERROR: SDL_image configuration failed"
        cat "$SHARED_BUILD_DIR/sdl_image-configure.log" | tail -20
        exit 1
    }

    echo "  Compiling SDL_image..."
    make -j$(nproc) >"$SHARED_BUILD_DIR/sdl_image-build.log" 2>&1 || {
        echo "  Full build failed (test programs may have failed), trying library only..."
        make libSDL_image.la >"$SHARED_BUILD_DIR/sdl_image-libonly-build.log" 2>&1 || {
            # Check if library exists anyway
            if [ ! -f ".libs/libSDL_image.a" ] && [ ! -f ".libs/libSDL_image.dll.a" ]; then
                echo "  ERROR: SDL_image compilation failed"
                cat "$SHARED_BUILD_DIR/sdl_image-build.log" | tail -20
                exit 1
            fi
        }
    }

    echo "  Installing SDL_image..."
    # Install only the library and headers, not test programs
    sudo make install >"$SHARED_BUILD_DIR/sdl_image-install.log" 2>&1 || {
        echo "  Standard install failed, installing library only..."
        sudo make install-libLTLIBRARIES >"$SHARED_BUILD_DIR/sdl_image-install.log" 2>&1 || true
        sudo make install-pkgconfigDATA >>"$SHARED_BUILD_DIR/sdl_image-install.log" 2>&1 || true
        # Install header manually if install target doesn't exist
        sudo cp SDL_image.h "${MINGW_INCLUDE}/SDL/" 2>/dev/null || true
        # Fix permissions (cp preserves restrictive perms from build dir)
        sudo chmod 644 "${MINGW_INCLUDE}/SDL/SDL_image.h" 2>/dev/null || true
        # Verify installation
        if [ ! -f "${MINGW_INCLUDE}/SDL/SDL_image.h" ]; then
            echo "  ERROR: SDL_image header not installed"
            exit 1
        fi
        if [ ! -f "${MINGW_LIB}/libSDL_image.a" ] && [ ! -f "${MINGW_LIB}/libSDL_image.dll.a" ]; then
            echo "  ERROR: SDL_image library not installed"
            exit 1
        fi
    }

    echo "  ✓ SDL_image installed"
    SDL_IMAGE_AVAILABLE=true
    cd "$SCRIPT_DIR"
fi

CONFIGURE_OPTS+=(--enable-sdl-client)
CONFIGURE_OPTS+=(--enable-sdl-gameloop)  # Use SDL gameloop instead of X11-optimized one

# Regenerate configure from configure.ac if needed
echo "Checking build system..."

# Copy SDL m4 macro from MinGW installation if not present
if [ ! -f "$SCRIPT_DIR/sdl.m4" ]; then
    SDL_M4="/usr/x86_64-w64-mingw32/share/aclocal/sdl.m4"
    if [ -f "$SDL_M4" ]; then
        echo "  Copying sdl.m4 from MinGW installation..."
        cp "$SDL_M4" "$SCRIPT_DIR/"
    else
        echo "  WARNING: sdl.m4 not found at $SDL_M4"
    fi
fi

# Regenerate build system if configure.ac or any Makefile.am is newer
NEED_REGEN=false
if [ ! -f "$SCRIPT_DIR/configure" ]; then
    NEED_REGEN=true
elif [ "$SCRIPT_DIR/configure.ac" -nt "$SCRIPT_DIR/configure" ]; then
    NEED_REGEN=true
elif [ -n "$(find "$SCRIPT_DIR" -name 'Makefile.am' -newer "$SCRIPT_DIR/configure" 2>/dev/null | head -1)" ]; then
    NEED_REGEN=true
fi

if [ "$NEED_REGEN" = true ]; then
    echo "  Regenerating build system (configure.ac or Makefile.am changed)..."
    cd "$SCRIPT_DIR"
    aclocal -I . 2>/dev/null || aclocal
    autoconf
    # Also regenerate Makefile.in files
    automake --add-missing --copy 2>/dev/null || automake --add-missing 2>/dev/null || true
    cd "$BUILD_DIR_ABS"
fi

# Patch configure script for Windows cross-compilation
echo "Patching configure for Windows cross-compilation..."

# Skip X11 check if SDL client is enabled (for Windows builds)
sed -i 's/if test x\$no_x == xyes; then/if test x$no_x == xyes \&\& test x$enable_sdl_client != xyes; then/' "$SCRIPT_DIR/configure"

# Remove -Dmain=SDL_main from SDL_CFLAGS in configure - this breaks configure tests
# because test programs use main(void) but SDL_main requires (int argc, char *argv[])
sed -i 's/-Dmain=SDL_main//g' "$SCRIPT_DIR/configure"

# Fix SDL_ttf/SDL_image tests - they use main(void) with no args
# Replace with proper argc/argv signature as a fallback
sed -i 's/main (void)/main (int argc, char *argv[])/g' "$SCRIPT_DIR/configure"
sed -i 's/main ()/main (int argc, char *argv[])/g' "$SCRIPT_DIR/configure"


# Pass through any additional configure options
for arg in "$@"; do
    if [[ "$arg" != "--enable-sdl-client" && "$arg" != "--disable-sdl-client" ]]; then
        CONFIGURE_OPTS+=("$arg")
    fi
done

# Fix permissions on installed headers (cp from build dir may have restrictive perms)
echo "Fixing header permissions..."
sudo chmod 644 "${MINGW_INCLUDE}/SDL/SDL_ttf.h" 2>/dev/null || true
sudo chmod 644 "${MINGW_INCLUDE}/SDL/SDL_image.h" 2>/dev/null || true
sudo chmod 644 "${MINGW_INCLUDE}/png.h" 2>/dev/null || true
sudo chmod 644 "${MINGW_INCLUDE}"/libpng16/*.h 2>/dev/null || true

echo "Configuring build..."
CONFIGURE_LOG="$(pwd)/configure.log"
"$SCRIPT_DIR/configure" "${CONFIGURE_OPTS[@]}" >"$CONFIGURE_LOG" 2>&1 || {
    echo ""
    echo "Configuration failed. Full log: $CONFIGURE_LOG"
    echo "Last 30 lines:"
    tail -30 "$CONFIGURE_LOG"
    exit 1
}

# Fix config.h - remove socklen_t definition that conflicts with xpcommon.h
# (MinGW provides socklen_t, and xpcommon.h also typedefs it for Windows)
if [ -f "config.h" ]; then
    sed -i 's/^#define socklen_t int$/\/* #define socklen_t int - disabled for MinGW *\//' config.h
fi

# Remove X11 client from build (we only want SDL client for Windows)
# The X11 client uses X11-specific code that won't compile on Windows
if [ -f "src/client/Makefile" ]; then
    echo "Disabling X11 client build (Windows uses SDL client only)..."
    sed -i 's/^bin_PROGRAMS = xpilot-ng-x11$(EXEEXT)$/bin_PROGRAMS =/' src/client/Makefile
fi

echo "Building Windows client..."
# For Windows, only build the client (not server or replay which have Unix dependencies)
# Build common first, then client
# Add -Werror during make (not during configure as it breaks configure tests)
set -o pipefail
make -C src/common V=1 CFLAGS="$CFLAGS -Werror" 2>&1 | tee "$SCRIPT_DIR/build-windows-make.log" || true
make -C src/client V=1 CFLAGS="$CFLAGS -Werror" 2>&1 | tee -a "$SCRIPT_DIR/build-windows-make.log" || true
set +o pipefail

# Check if client exe was built (may be in source or build directory)
CLIENT_EXE=""
if [ -f "$SCRIPT_DIR/src/client/sdl/xpilot-ng-sdl.exe" ]; then
    CLIENT_EXE="$SCRIPT_DIR/src/client/sdl/xpilot-ng-sdl.exe"
elif [ -f "src/client/sdl/xpilot-ng-sdl.exe" ]; then
    CLIENT_EXE="src/client/sdl/xpilot-ng-sdl.exe"
fi

if [ -z "$CLIENT_EXE" ]; then
    echo ""
    echo "Build failed! Client executable not found."
    echo "Last 50 lines of build log:"
    tail -50 "$SCRIPT_DIR/build-windows-make.log"
    exit 1
fi

echo ""
echo "=========================================="
echo "Build completed successfully!"
echo "=========================================="
echo ""

# CLIENT_EXE was already set above when we verified the build
# Server and replay are not built for Windows client-only build
SERVER_EXE=""
REPLAY_EXE=""

echo "Built binaries:"
[ -n "$CLIENT_EXE" ] && echo "  Client: $CLIENT_EXE"
[ -n "$SERVER_EXE" ] && echo "  Server: $SERVER_EXE"
[ -n "$REPLAY_EXE" ] && echo "  Replay: $REPLAY_EXE"
echo ""

# Create installer package
echo "Creating installer package..."

INSTALLER_DIR="$SCRIPT_DIR/installer-windows"
rm -rf "$INSTALLER_DIR"
mkdir -p "$INSTALLER_DIR"
mkdir -p "$INSTALLER_DIR/data"
mkdir -p "$INSTALLER_DIR/doc"

# Copy executables (directly to install root for Windows)
echo "  Copying executables..."
[ -n "$CLIENT_EXE" ] && cp "$CLIENT_EXE" "$INSTALLER_DIR/"
[ -n "$SERVER_EXE" ] && cp "$SERVER_EXE" "$INSTALLER_DIR/"
[ -n "$REPLAY_EXE" ] && cp "$REPLAY_EXE" "$INSTALLER_DIR/"

# Copy data files (excluding Makefiles and build files)
echo "  Copying data files..."
if [ -d "$SCRIPT_DIR/lib" ]; then
    # Copy only actual data files, not build system files
    cd "$SCRIPT_DIR/lib"
    find . -type f \
        ! -name "Makefile*" \
        ! -name "*.am" \
        ! -name "*.in" \
        ! -name ".gitignore" \
        -exec install -D -m 644 {} "$INSTALLER_DIR/data/{}" \;
    cd "$SCRIPT_DIR"
fi

# Copy documentation (excluding Makefiles and build files)
echo "  Copying documentation..."
if [ -d "$SCRIPT_DIR/doc" ]; then
    cd "$SCRIPT_DIR/doc"
    find . -type f \
        ! -name "Makefile*" \
        ! -name "*.am" \
        ! -name "*.in" \
        -exec install -D -m 644 {} "$INSTALLER_DIR/doc/{}" \;
    cd "$SCRIPT_DIR"
fi

# Copy required DLLs
echo "  Copying DLL dependencies..."

# Find and copy DLLs
find_dll() {
    local dll_name=$1
    local found=""

    # Check common locations
    for path in "${MINGW_LIB}" "${MINGW_PREFIX_DIR}/bin" "/usr/${MINGW_PREFIX}/bin"; do
        if [ -f "${path}/${dll_name}" ]; then
            found="${path}/${dll_name}"
            break
        fi
    done

    # Also check if it's in the build directory
    if [ -z "$found" ]; then
        local build_dll=$(find "$BUILD_DIR" -name "${dll_name}" -type f 2>/dev/null | head -1)
        if [ -n "$build_dll" ]; then
            found="$build_dll"
        fi
    fi

    echo "$found"
}

# Copy zlib DLL
ZLIB_DLL=$(find_dll "zlib1.dll")
if [ -n "$ZLIB_DLL" ] && [ -f "$ZLIB_DLL" ]; then
    cp "$ZLIB_DLL" "$INSTALLER_DIR/"
fi

# Copy expat DLL (if shared)
EXPAT_DLL=$(find_dll "libexpat-1.dll")
if [ -z "$EXPAT_DLL" ]; then
    EXPAT_DLL=$(find_dll "libexpat.dll")
fi
if [ -n "$EXPAT_DLL" ] && [ -f "$EXPAT_DLL" ]; then
    cp "$EXPAT_DLL" "$INSTALLER_DIR/"
fi

# Copy SDL DLLs if SDL client was built
if [ "$ENABLE_SDL" = true ]; then
    for sdl_dll in "SDL.dll" "SDL_ttf.dll" "SDL_image.dll"; do
        SDL_PATH=$(find_dll "$sdl_dll")
        if [ -n "$SDL_PATH" ] && [ -f "$SDL_PATH" ]; then
            cp "$SDL_PATH" "$INSTALLER_DIR/"
        fi
    done
fi

# Create README for Windows users
cat > "$INSTALLER_DIR/README.txt" << EOF
XPilot NG for Windows
=====================

This installer contains XPilot NG for Windows.

Files:
- xpilot-ng-sdl.exe : Main game executable
- *.dll             : Required libraries
- data/             : Game data (maps, textures, fonts)
- doc/              : Documentation

For more information, visit: http://xpilot.sourceforge.net/
EOF

# Create installer - prefer MSI (wixl), then NSIS, then ZIP
INSTALLER_NAME="xpilot-ng-windows"
INSTALLER_FILE=""
INSTALLER_TYPE=""
CLIENT_NAME=$(basename "$CLIENT_EXE")

# Install wixl if not available (for MSI creation - preferred for Windows)
if ! command -v wixl >/dev/null 2>&1; then
    echo "  Installing wixl for MSI creation..."
    install_package "wixl" "msitools" "msitools" || true
fi

# Try MSI first (wixl from msitools package) - preferred for Windows
if command -v wixl >/dev/null 2>&1; then
    echo "  Creating MSI installer..."

    # Generate WiX XML source
    WXS_FILE="$INSTALLER_DIR/xpilot.wxs"

    # Generate unique GUIDs for the installer
    PRODUCT_GUID=$(cat /proc/sys/kernel/random/uuid | tr '[:lower:]' '[:upper:]')
    UPGRADE_GUID="E7D1B2A3-C4D5-6E7F-8A9B-0C1D2E3F4A5B"

    cat > "$WXS_FILE" << WIXEOF
<?xml version="1.0" encoding="utf-8"?>
<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi">
  <Product Id="$PRODUCT_GUID"
           Name="XPilot NG"
           Language="1033"
           Version="4.6.3.0"
           Manufacturer="XPilot NG Team"
           UpgradeCode="$UPGRADE_GUID">

    <Package InstallerVersion="200" Compressed="yes" InstallScope="perMachine"
             Description="XPilot NG - Space Combat Game"
             Comments="XPilot NG for Windows" />

    <Media Id="1" Cabinet="xpilot.cab" EmbedCab="yes" />

    <Directory Id="TARGETDIR" Name="SourceDir">
      <Directory Id="ProgramFilesFolder">
        <Directory Id="INSTALLDIR" Name="XPilot NG">
          <Directory Id="DataDir" Name="data" />
          <Directory Id="DocDir" Name="doc" />
        </Directory>
      </Directory>
      <Directory Id="ProgramMenuFolder">
        <Directory Id="ApplicationProgramsFolder" Name="XPilot NG" />
      </Directory>
      <Directory Id="DesktopFolder" Name="Desktop" />
    </Directory>

    <DirectoryRef Id="INSTALLDIR">
      <Component Id="MainExecutable" Guid="*">
        <File Id="XPilotExe" Source="$INSTALLER_DIR/$CLIENT_NAME" KeyPath="yes" />
      </Component>
WIXEOF

    # Add DLLs to WXS
    DLL_COMP_ID=1
    for dll in "$INSTALLER_DIR/"*.dll; do
        if [ -f "$dll" ]; then
            DLL_NAME=$(basename "$dll")
            cat >> "$WXS_FILE" << DLEOF
      <Component Id="DLL_$DLL_COMP_ID" Guid="*">
        <File Id="DLL_${DLL_COMP_ID}_File" Source="$dll" KeyPath="yes" />
      </Component>
DLEOF
            DLL_COMP_ID=$((DLL_COMP_ID + 1))
        fi
    done

    cat >> "$WXS_FILE" << WIXEOF2
    </DirectoryRef>

    <DirectoryRef Id="ApplicationProgramsFolder">
      <Component Id="ApplicationShortcut" Guid="*">
        <Shortcut Id="ApplicationStartMenuShortcut"
                  Name="XPilot NG"
                  Description="XPilot NG Space Combat Game"
                  Target="[INSTALLDIR]$CLIENT_NAME"
                  WorkingDirectory="INSTALLDIR" />
        <RemoveFolder Id="CleanUpShortCut" On="uninstall" />
        <RegistryValue Root="HKCU" Key="Software\\XPilotNG" Name="installed" Type="integer" Value="1" KeyPath="yes" />
      </Component>
    </DirectoryRef>

    <DirectoryRef Id="DesktopFolder">
      <Component Id="DesktopShortcut" Guid="*">
        <Shortcut Id="DesktopShortcut"
                  Name="XPilot NG"
                  Description="XPilot NG Space Combat Game"
                  Target="[INSTALLDIR]$CLIENT_NAME"
                  WorkingDirectory="INSTALLDIR" />
        <RegistryValue Root="HKCU" Key="Software\\XPilotNG" Name="desktopshortcut" Type="integer" Value="1" KeyPath="yes" />
      </Component>
    </DirectoryRef>

    <Feature Id="ProductFeature" Title="XPilot NG" Level="1">
      <ComponentRef Id="MainExecutable" />
      <ComponentRef Id="ApplicationShortcut" />
      <ComponentRef Id="DesktopShortcut" />
WIXEOF2

    # Add DLL component refs
    for i in $(seq 1 $((DLL_COMP_ID - 1))); do
        echo "      <ComponentRef Id=\"DLL_$i\" />" >> "$WXS_FILE"
    done

    cat >> "$WXS_FILE" << WIXEOF3
    </Feature>

  </Product>
</Wix>
WIXEOF3

    # Run wixl to create MSI
    cd "$INSTALLER_DIR"
    if wixl -v -o "$SCRIPT_DIR/${INSTALLER_NAME}.msi" "$WXS_FILE" 2>"$SCRIPT_DIR/wixl.log"; then
        INSTALLER_FILE="$SCRIPT_DIR/${INSTALLER_NAME}.msi"
        INSTALLER_TYPE="MSI installer"
    else
        echo "  MSI creation failed, falling back..."
        cat "$SCRIPT_DIR/wixl.log" | tail -10
    fi
    cd "$SCRIPT_DIR"
fi

# Try NSIS if MSI didn't work
if [ -z "$INSTALLER_FILE" ]; then
    # Install NSIS if not available
    if ! command -v makensis >/dev/null 2>&1; then
        echo "  Installing NSIS..."
        install_package "nsis" || true
    fi
fi

if [ -z "$INSTALLER_FILE" ] && command -v makensis >/dev/null 2>&1; then
    echo "  Creating NSIS installer..."

    NSIS_SCRIPT="$INSTALLER_DIR/installer.nsi"

    cat > "$NSIS_SCRIPT" << NSISEOF
!define PRODUCT_NAME "XPilot NG"
!define PRODUCT_VERSION "4.6.3"
!define PRODUCT_PUBLISHER "XPilot NG Team"
!define PRODUCT_WEB_SITE "http://xpilot.sourceforge.net/"

Name "\${PRODUCT_NAME} \${PRODUCT_VERSION}"
OutFile "$SCRIPT_DIR/${INSTALLER_NAME}.exe"
InstallDir "\$PROGRAMFILES\\XPilot NG"
ShowInstDetails show

Section "MainSection"
  SetOutPath "\$INSTDIR"
  File "$INSTALLER_DIR/*.exe"
  File "$INSTALLER_DIR/*.dll"
  File "$INSTALLER_DIR/README.txt"

  SetOutPath "\$INSTDIR\\data"
  File /r "$INSTALLER_DIR/data/*"

  SetOutPath "\$INSTDIR\\doc"
  File /r "$INSTALLER_DIR/doc/*"

  CreateDirectory "\$SMPROGRAMS\\XPilot NG"
  CreateShortCut "\$SMPROGRAMS\\XPilot NG\\XPilot NG.lnk" "\$INSTDIR\\${CLIENT_NAME}"
  CreateShortCut "\$DESKTOP\\XPilot NG.lnk" "\$INSTDIR\\${CLIENT_NAME}"
SectionEnd
NSISEOF

    cd "$INSTALLER_DIR"
    if makensis "$NSIS_SCRIPT" >/dev/null 2>&1 && [ -f "$SCRIPT_DIR/${INSTALLER_NAME}.exe" ]; then
        INSTALLER_FILE="$SCRIPT_DIR/${INSTALLER_NAME}.exe"
        INSTALLER_TYPE="NSIS installer"
    fi
    cd "$SCRIPT_DIR"
fi

# Fall back to ZIP if no installer tools available
if [ -z "$INSTALLER_FILE" ]; then
    echo "  Creating ZIP archive (MSI/NSIS tools not available)..."
    INSTALLER_FILE="$SCRIPT_DIR/${INSTALLER_NAME}.zip"
    INSTALLER_TYPE="ZIP archive"
    cd "$SCRIPT_DIR"
    zip -rq "$INSTALLER_FILE" installer-windows/
fi

INSTALLER_SIZE=$(ls -lh "$INSTALLER_FILE" 2>/dev/null | awk '{print $5}')

echo ""
echo "=========================================="
echo "Installer created successfully!"
echo "=========================================="
echo ""
echo "Installer: $INSTALLER_FILE ($INSTALLER_SIZE)"
echo "Type: $INSTALLER_TYPE"
echo ""
echo "This file is ready to transfer to a Windows PC and install."
echo ""
