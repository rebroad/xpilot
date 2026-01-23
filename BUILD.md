# Building XPilot NG

This document provides detailed instructions for building XPilot NG on Linux and cross-compiling for Windows.

## Quick Start

### Linux

```bash
./build.sh
```

### Windows (cross-compiled from Linux)

```bash
./build-windows.sh
```

This creates a Windows MSI installer automatically.

## Linux Build

### Prerequisites

**Debian/Ubuntu:**
```bash
sudo apt-get install build-essential autoconf automake \
    libx11-dev zlib1g-dev libexpat1-dev \
    libsdl1.2-dev libsdl-ttf2.0-dev libsdl-image1.2-dev \
    libgl1-mesa-dev libglu1-mesa-dev
```

**Fedora:**
```bash
sudo dnf install gcc make autoconf automake \
    libX11-devel zlib-devel expat-devel \
    SDL-devel SDL_ttf-devel SDL_image-devel \
    mesa-libGL-devel mesa-libGLU-devel
```

**Arch Linux:**
```bash
sudo pacman -S base-devel libx11 zlib expat \
    sdl sdl_ttf sdl_image mesa glu
```

### Building

The recommended way is to use the build script:

```bash
./build.sh
```

This will:
1. Regenerate autotools files if needed
2. Configure the build in `build-linux/` (out-of-tree)
3. Compile the server and clients
4. Report binary locations

### Build Output

- `build-linux/src/server/xpilot-ng-server` - Game server
- `build-linux/src/client/xpilot-ng-x11` - X11 client
- `build-linux/src/client/sdl/xpilot-ng-sdl` - SDL/OpenGL client
- `build-linux/src/replay/xpilot-ng-replay` - Replay viewer

### Manual Build

```bash
mkdir build-linux && cd build-linux
../configure --enable-sdl-client
make -j$(nproc)
```

## Windows Build (Cross-Compilation)

The `build-windows.sh` script provides fully automated Windows cross-compilation from Linux, including:

- Automatic dependency building (zlib, expat, FreeType2, SDL_ttf, SDL_image)
- MSI installer creation
- All required DLLs bundled

### Prerequisites

The script will automatically install MinGW-w64 if not present. You need:

- `sudo` access (for package installation)
- Internet connection (for downloading dependencies)

Pre-installed MinGW packages needed:
```bash
# Debian/Ubuntu
sudo apt-get install mingw-w64 libsdl1.2-dev:amd64

# Fedora
sudo dnf install mingw64-gcc mingw64-SDL mingw64-SDL_ttf mingw64-SDL_image

# Arch
sudo pacman -S mingw-w64-gcc
```

### Building

```bash
./build-windows.sh
```

The script will:
1. Install MinGW-w64 cross-compiler if needed
2. Download and cross-compile dependencies:
   - zlib (compression)
   - expat (XML parsing)
   - FreeType2 (font rendering, via Meson)
   - SDL_ttf (SDL font support)
   - SDL_image (image loading with PNG support)
3. Configure and build XPilot NG SDL client
4. Create an MSI installer with all files

### Build Output

- `build-windows/src/client/sdl/xpilot-ng-sdl.exe` - Windows SDL client
- `xpilot-ng-windows.msi` - Windows installer (ready to distribute)

### Installation Directory

The MSI installs to `C:\Program Files\XPilot NG\` with this structure:
```
C:\Program Files\XPilot NG\
├── xpilot-ng-sdl.exe     (main executable)
├── zlib1.dll             (compression library)
├── SDL_image.dll         (image loading)
├── data\                 (game data)
│   ├── fonts\
│   ├── maps\
│   ├── textures\
│   └── *.txt
├── doc\                  (documentation)
└── README.txt
```

### Installer Creation

The script prefers these installer formats (in order):
1. **MSI** (via `wixl`) - Windows Installer, integrates with Add/Remove Programs
2. **NSIS** (via `makensis`) - Executable installer
3. **ZIP** - Fallback if no installer tools available

Missing tools are installed automatically.

## Build Options

### Configure Options

```bash
./configure --help    # Show all options
```

Common options:
- `--enable-sdl-client` - Build SDL/OpenGL client (recommended for Windows)
- `--enable-sdl-gameloop` - Use SDL event loop (required for Windows SDL)
- `--prefix=/path` - Installation prefix
- `--enable-dbe` - X11 Double Buffer Extension
- `--enable-mbx` - X11 Multi-Buffer Extension

### Compiler Flags

The Windows build uses `-Werror` to catch all warnings as errors, ensuring clean code.

## Troubleshooting

### Linux Build Issues

**Missing X11 libraries:**
```bash
sudo apt-get install libx11-dev libxext-dev
```

**Missing OpenGL libraries:**
```bash
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev
```

**Missing SDL libraries:**
```bash
sudo apt-get install libsdl1.2-dev libsdl-ttf2.0-dev libsdl-image1.2-dev
```

### Windows Cross-Compilation Issues

**"configure: error: *** Math library not found!"**
- The script patches configure to handle MinGW quirks. If this persists, check that MinGW-w64 is properly installed.

**FreeType2 build produces ELF instead of PE/COFF:**
- The script uses Meson (not Autotools) for FreeType2 to ensure correct cross-compilation.

**Missing DLLs at runtime:**
- All required DLLs are bundled in the MSI installer automatically.
- If building manually, copy DLLs from `/usr/x86_64-w64-mingw32/bin/`.

**wixl/makensis not found:**
- The script will attempt to install these automatically via your package manager.
- Manual install: `sudo apt-get install wixl nsis`

### Build Artifact Conflicts

The build scripts use out-of-tree builds to prevent conflicts:
- Linux: `build-linux/`
- Windows: `build-windows/`

If you encounter stale artifacts, clean with:
```bash
rm -rf build-linux build-windows
```

## Development Notes

### Code Quality

The codebase builds with `-Werror` on Windows, meaning all compiler warnings are treated as errors. Key fixes applied:

- Proper header include order (`winsock2.h` before `windows.h`)
- Explicit function return types in NT compatibility headers
- Guarded macro redefinitions (`#ifndef` guards)
- Correct types for Windows API calls (`u_long` vs `int`)

### Architecture

- **src/common/** - Shared code between server and clients
- **src/server/** - Game server
- **src/client/** - X11 client and shared client code
- **src/client/sdl/** - SDL/OpenGL client (cross-platform)
- **src/replay/** - Replay viewer
- **lib/** - Game data (maps, textures, fonts, configs)

### Adding New Warnings

To add stricter warnings, modify `CFLAGS` in configure or the build scripts:
```bash
CFLAGS="-Wall -Wextra -Werror" ./configure
```

## Additional Resources

- Main website: http://xpilot.sourceforge.net/
- Source repository: (your repo URL)
- Documentation: See `doc/` directory
