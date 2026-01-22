# Building XPilot NG

This document provides detailed instructions for building XPilot NG on Linux and Windows.

## Quick Start (Linux)

The easiest way to build on Linux is to use the provided build script:

```bash
./build.sh
```

This will automatically check dependencies, configure, and build the project.

## Prerequisites

### Linux Build Requirements

- **Build tools**: `gcc`, `make`, `autoconf`, `automake`
- **Libraries**:
  - X11 development libraries (`libx11-dev` on Debian/Ubuntu)
  - zlib development libraries (`zlib1g-dev` on Debian/Ubuntu)
  - expat XML parser (`libexpat1-dev` on Debian/Ubuntu)

**Install on Debian/Ubuntu:**
```bash
sudo apt-get install build-essential autoconf automake \
    libx11-dev zlib1g-dev libexpat1-dev
```

**Install on Fedora:**
```bash
sudo dnf install gcc make autoconf automake \
    libX11-devel zlib-devel expat-devel
```

**Install on Arch Linux:**
```bash
sudo pacman -S base-devel libx11 zlib expat
```

### Windows Build Requirements

For cross-compilation from Linux:
- MinGW-w64 cross-compiler (`mingw-w64` package)
- Windows versions of required libraries (zlib, expat, SDL)

For native Windows builds:
- Visual Studio (project files `.dsp` are provided)
- Or MinGW/MSYS2 with required libraries

## Building on Linux

### Method 1: Using the Build Script (Recommended)

```bash
./build.sh
```

The script will:
1. Check for required dependencies
2. Configure the build system
3. Compile the server and client
4. Show you where the binaries are located

### Method 2: Manual Build

1. **Configure the build:**
   ```bash
   ./configure
   ```

   Common configure options:
   - `--prefix=/usr/local` - Installation prefix (default)
   - `--enable-sdl-client` - Build SDL/OpenGL client (requires SDL)
   - `--enable-dbe` - Enable X Doublebuffer Extension
   - `--enable-mbx` - Enable X Multibuffer Extension

2. **Build:**
   ```bash
   make -j$(nproc)
   ```

3. **Install (optional):**
   ```bash
   sudo make install
   ```

### Build Output

After building, you'll find these binaries:
- `src/server/xpilot-ng-server` - Game server
- `src/client/xpilot-ng-x11` - X11 client
- `src/replay/xpilot-ng-replay` - Replay viewer

If SDL client is enabled:
- `src/client/sdl/xpilot-ng-sdl` - SDL/OpenGL client

## Building for Windows

### Option 1: Cross-Compilation from Linux

1. **Install MinGW-w64:**
   ```bash
   # Debian/Ubuntu
   sudo apt-get install mingw-w64

   # Fedora
   sudo dnf install mingw64-gcc

   # Arch
   sudo pacman -S mingw-w64-gcc
   ```

2. **Run the Windows build script:**
   ```bash
   ./build-windows.sh
   ```

   Note: This requires Windows versions of dependencies. For a complete
   Windows build, you may need to:
   - Install Windows libraries in `/usr/x86_64-w64-mingw32/` or similar
   - Use `--enable-sdl-client` for SDL-based client (recommended)

3. **Copy required DLLs:**
   The Windows executable will need these DLLs:
   - `zlib1.dll`
   - `libexpat-1.dll`
   - `SDL.dll`, `SDL_ttf.dll`, `SDL_image.dll` (if using SDL client)

### Option 2: Native Windows Build with Visual Studio

The project includes Visual Studio project files (`.dsp` files):

1. Open `XPilot.dsw` in Visual Studio
2. Select the configuration (Release/Debug)
3. Build the solution

### Option 3: Native Windows Build with MinGW/MSYS2

1. Install MSYS2 from https://www.msys2.org/
2. Install required packages:
   ```bash
   pacman -S mingw-w64-x86_64-gcc \
            mingw-w64-x86_64-zlib \
            mingw-w64-x86_64-expat \
            mingw-w64-x86_64-SDL \
            mingw-w64-x86_64-SDL_ttf \
            mingw-w64-x86_64-SDL_image
   ```

3. In MSYS2 MinGW64 shell:
   ```bash
   ./configure --enable-sdl-client
   make
   ```

## Troubleshooting

### Build Errors

**"getline: conflicting types"**
- Fixed in the source code. If you see this, ensure you have the latest version.

**"multiple definition of 'players_exposed'"**
- Fixed in the source code. Ensure `xpaint.c` doesn't define these variables.

**Missing X11 libraries**
- Install `libx11-dev` (Debian/Ubuntu) or equivalent for your distribution.

**Missing zlib or expat**
- Install development packages: `zlib1g-dev libexpat1-dev` (Debian/Ubuntu)

### Windows Build Issues

**Cross-compilation fails**
- Ensure MinGW-w64 is properly installed
- Check that Windows libraries are available in the MinGW prefix directory
- Consider using SDL client instead of X11 client for Windows

**Missing DLLs at runtime**
- Copy required DLLs to the same directory as the executable
- Or add the MinGW bin directory to PATH

## Advanced Configuration

### Enable SDL Client

The SDL client provides better cross-platform support and OpenGL rendering:

```bash
./configure --enable-sdl-client
```

This requires:
- SDL 1.2.0 or later
- SDL_ttf
- SDL_image
- OpenGL libraries

### Custom Installation Prefix

Install to a custom location:

```bash
./configure --prefix=/opt/xpilot
make
sudo make install
```

### Development Build

Enable development features:

```bash
./configure --enable-development
```

## Creating Windows Installer

### Using NSIS (Recommended)

A NSIS installer script is provided (`build-windows.nsi`). To create an installer:

1. Install NSIS from https://nsis.sourceforge.io/
2. Build the Windows client first (see Windows build instructions above)
3. Copy required DLLs to the build directory
4. Compile the NSIS script:
   ```bash
   makensis build-windows.nsi
   ```

This will create `xpilot-ng-setup.exe` installer.

### Alternative Installer Tools

- **WiX Toolset**: https://wixtoolset.org/ (creates MSI installers)
- **Inno Setup**: https://jrsoftware.org/isinfo.php (another popular installer)

A basic installer should include:
- The client executable
- Required DLLs (zlib, expat, SDL if used)
- Data files from `lib/` directory
- Documentation

## Additional Resources

- Main website: http://xpilot.sourceforge.net/
- FAQ: telnet meta.xpilot.org 4402
- Documentation: See `doc/man/` directory
