#!/bin/bash
# Build script for XPilot NG Windows client using MinGW cross-compiler
# This script automatically handles all dependencies and cross-compiles the Windows client
#
# Usage: ./build-windows.sh [--clean] [--wine-test] [--wine-run]
#   --clean            Force a clean build by removing the build directory first
#   --wine-test        After building, smoke-test the generated MSI using Wine in a temporary prefix under /var/tmp (default)
#   --wine-run         Run the installed Windows client from the latest Wine test prefix (no rebuild, no timeout)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Parse command line arguments
FORCE_CLEAN=false
WINE_TEST=false
WINE_RUN=false
while [ $# -gt 0 ]; do
    case "$1" in
        --clean)
            FORCE_CLEAN=true
            shift
            ;;
        --wine-test)
            WINE_TEST=true
            shift
            ;;
        --wine-run)
            WINE_RUN=true
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [--clean] [--wine-test] [--wine-run]"
            echo "  --clean            Force a clean build by removing the build directory first"
            echo "  --wine-test        After building, smoke-test the generated MSI using Wine in a temporary prefix"
            echo "  --wine-run         Run the installed Windows client from the latest Wine test prefix (no rebuild)"
            echo ""
            echo "Wine run:"
            echo "  Uses latest prefix under /var/tmp/xpilot by default."
            echo "  Override with XPILOT_WINE_RUN_PREFIX=/path/to/prefix"
            echo ""
            echo "Wine test location:"
            echo "  Default base dir: /var/tmp/xpilot (fits systemd-tmpfiles cleanup when enabled for /var/tmp)"
            echo "  Override with XPILOT_WINE_TEST_BASEDIR."
            echo ""
            echo "Wine test overrides:"
            echo "  XPILOT_WINE_TEST_WINEDEBUG, XPILOT_WINE_TEST_*_MAX, XPILOT_WINE_TEST_*_IDLE, etc."
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Run '$0 --help' for usage."
            exit 1
            ;;
    esac
done

wine_find_installed_xpilot_exe() {
    # Print the full path to xpilot-ng-sdl.exe inside a Wine prefix, or nothing if not found.
    local prefix="$1"
    local exe_rel1="drive_c/Program Files/XPilot NG/xpilot-ng-sdl.exe"
    local exe_rel2="drive_c/Program Files (x86)/XPilot NG/xpilot-ng-sdl.exe"
    if [ -f "$prefix/$exe_rel1" ]; then
        echo "$prefix/$exe_rel1"
        return 0
    fi
    if [ -f "$prefix/$exe_rel2" ]; then
        echo "$prefix/$exe_rel2"
        return 0
    fi
    return 1
}

wine_run_latest_installed_client() {
    if ! command -v wine >/dev/null 2>&1; then
        echo "Wine run skipped: wine not found in PATH"
        echo "  On Ubuntu/Debian you can try: sudo apt-get install -y wine64"
        return 0
    fi

    local basedir="${XPILOT_WINE_TEST_BASEDIR:-/var/tmp/xpilot}"
    local prefix="${XPILOT_WINE_RUN_PREFIX:-}"
    if [ -z "$prefix" ]; then
        prefix="$(ls -1dt "$basedir"/xpilot-wine-test.* 2>/dev/null | head -1 || true)"
    fi
    if [ -z "$prefix" ] || [ ! -d "$prefix" ]; then
        echo "Wine run failed: no Wine test prefix found."
        echo "  Looked in: $basedir"
        echo "  Tip: run './build-windows.sh --wine-test' first, or set XPILOT_WINE_RUN_PREFIX."
        return 1
    fi

    local exe_path=""
    exe_path="$(wine_find_installed_xpilot_exe "$prefix" 2>/dev/null || true)"
    if [ -z "$exe_path" ]; then
        echo "Wine run failed: installed exe not found in prefix:"
        echo "  $prefix"
        return 1
    fi

    echo "=========================================="
    echo "Wine run (no rebuild)"
    echo "=========================================="
    echo "WINEPREFIX: $prefix"
    echo "EXE: $exe_path"
    echo ""
    export WINEPREFIX="$prefix"

    # Default to windowed mode under Wine to avoid fullscreen display-change issues.
    # Override by setting XPILOT_WINDOWED=0.
    local install_dir
    install_dir="$(dirname "$exe_path")"
    (cd "$install_dir" && XPILOT_WINDOWED="${XPILOT_WINDOWED:-1}" wine "$exe_path")
}

if [ "$WINE_RUN" = true ]; then
    wine_run_latest_installed_client
    exit $?
fi

# Wine MSI smoke test (optional)
#
# Behavior:
# - Creates a disposable Wine prefix under /var/tmp/xpilot (default)
# - Uses a cached template prefix under /var/tmp/xpilot to speed up repeated runs (fast path uses reflinks when supported)
# - Runs: wineboot -u, then msiexec /i <msi> with verbose logging
# - If an installed XPilot exe is found and DISPLAY is available, attempts to launch it
wine_clone_prefix_template() {
    local template_prefix="$1"
    local dest_prefix="$2"
    local label="${3:-template}"

    if [ -z "$template_prefix" ] || [ ! -d "$template_prefix" ]; then
        return 1
    fi

    echo "  Cloning Wine prefix ($label): $template_prefix"
    mkdir -p "$dest_prefix"

    # Fast copy on filesystems that support it; fall back to normal copy.
    if cp -a --reflink=auto "$template_prefix/." "$dest_prefix/" 2>/dev/null; then
        return 0
    fi
    cp -a "$template_prefix/." "$dest_prefix/"
}

wine_prepare_cached_template_prefix() {
    local source_prefix="${1:-}"
    local cached_prefix="$2"

    # If it already exists, we assume it's good enough; this is a best-effort cache.
    if [ -d "$cached_prefix" ]; then
        return 0
    fi

    rm -rf "$cached_prefix" >/dev/null 2>&1 || true

    if [ -n "$source_prefix" ] && [ -d "$source_prefix" ]; then
        echo "  Creating cached Wine template prefix: $cached_prefix"
        wine_clone_prefix_template "$source_prefix" "$cached_prefix" "source template"
        return 0
    fi

    # Default: create a fresh, minimal prefix (avoids inheriting ~/.wine quirks/popups).
    echo "  Creating fresh Wine template prefix: $cached_prefix"
    mkdir -p "$cached_prefix"
    (
        export WINEPREFIX="$cached_prefix"
        export WINEARCH=win64
        export WINEDEBUG=-all
        export WINEDLLOVERRIDES="winemenubuilder.exe=d;werfault.exe=d;wermgr.exe=d"
        if command -v timeout >/dev/null 2>&1; then
            timeout 90s wineboot -u >/dev/null 2>&1 || true
        else
            wineboot -u >/dev/null 2>&1 || true
        fi
    )
    return 0
}

wine_test_msi() {
    local msi_path="$1"

    if [ -z "$msi_path" ] || [ ! -f "$msi_path" ]; then
        echo "Wine test skipped: MSI not found: $msi_path"
        return 0
    fi

    if ! command -v wine >/dev/null 2>&1; then
        echo "Wine test skipped: wine not found in PATH"
        echo "  On Ubuntu/Debian you can try: sudo apt-get install -y wine64"
        return 0
    fi

    local tmp_prefix=""
    local tmp_log="/tmp/xpilot-msi.$(date +%Y%m%d-%H%M%S).log"
    local tmp_run_log="/tmp/xpilot-wine-run.$(date +%Y%m%d-%H%M%S).log"
    local tmp_clientlogs_dir="/tmp/xpilot-wine-clientlogs.$(date +%Y%m%d-%H%M%S)"
    local tmp_wineboot_log="/tmp/xpilot-wineboot.$(date +%Y%m%d-%H%M%S).log"
    local tmp_wine_reg_log="/tmp/xpilot-wine-reg.$(date +%Y%m%d-%H%M%S).log"
    local tmp_msiexec_out="/tmp/xpilot-wine-msiexec.$(date +%Y%m%d-%H%M%S).log"

    wine_run_watchdog() {
        # Run a Wine command and detect "hung waiting for UI" by lack of log output growth.
        #
        # Args:
        #   label max_total_secs idle_secs out_file cmd...
        local label="$1"
        local max_total="$2"
        local idle_limit="$3"
        local out_file="$4"
        shift 4

        : >"$out_file" 2>/dev/null || true
        "$@" >"$out_file" 2>&1 &
        local pid=$!

        local start_ts last_change_ts last_size now_ts
        start_ts=$(date +%s)
        last_change_ts="$start_ts"
        last_size=0

        while kill -0 "$pid" 2>/dev/null; do
            sleep 2
            now_ts=$(date +%s)

            # If the log is still growing, consider it "progress".
            local size=0
            size=$(wc -c <"$out_file" 2>/dev/null || echo 0)
            if [ "$size" -ne "$last_size" ]; then
                last_size="$size"
                last_change_ts="$now_ts"
            fi

            # Hard upper bound (true long-run cap).
            if [ $((now_ts - start_ts)) -ge "$max_total" ]; then
                echo "  WARNING: ${label} exceeded ${max_total}s; killing Wine processes..."
                set +e
                kill "$pid" >/dev/null 2>&1 || true
                wineserver -k >/dev/null 2>&1 || true
                set -e
                wait "$pid" >/dev/null 2>&1 || true
                return 124
            fi

            # Idle timeout: likely hung on a hidden dialog if nothing changes for a while.
            if [ $((now_ts - last_change_ts)) -ge "$idle_limit" ]; then
                echo "  WARNING: ${label} produced no new output for ${idle_limit}s; likely blocked on UI. Killing Wine processes..."
                set +e
                kill "$pid" >/dev/null 2>&1 || true
                wineserver -k >/dev/null 2>&1 || true
                set -e
                wait "$pid" >/dev/null 2>&1 || true
                return 124
            fi
        done

        wait "$pid"
        local rc=$?
        if [ "$rc" -ne 0 ]; then
            echo "  WARNING: ${label} exited with code ${rc} (details: $out_file)"
        fi
        return "$rc"
    }

    # Choose a source template prefix if explicitly provided (opt-in).
    local source_template_prefix=""
    if [ -n "${XPILOT_WINE_TEMPLATE_PREFIX:-}" ] && [ -d "$XPILOT_WINE_TEMPLATE_PREFIX" ]; then
        source_template_prefix="$XPILOT_WINE_TEMPLATE_PREFIX"
    fi

    # Pick a base directory for the temporary prefix.
    # Default: /var/tmp/xpilot (so systemd-tmpfiles can clean it when enabled for /var/tmp).
    # Override with XPILOT_WINE_TEST_BASEDIR if desired.
    local test_basedir=""
    if [ -n "${XPILOT_WINE_TEST_BASEDIR:-}" ]; then
        test_basedir="$XPILOT_WINE_TEST_BASEDIR"
    else
        test_basedir="/var/tmp/xpilot"
    fi
    mkdir -p "$test_basedir"

    # Cache a template inside the base dir so repeated tests can be fast (reflinks work when /var/tmp
    # is on the same filesystem as the cache and supports them).
    local template_prefix=""
    if [ -z "${XPILOT_WINE_DISABLE_TEMPLATE_CACHE:-}" ]; then
        local cached_template_prefix="$test_basedir/xpilot-wine-template-prefix"
        wine_prepare_cached_template_prefix "$source_template_prefix" "$cached_template_prefix" || true
        if [ -d "$cached_template_prefix" ]; then
            template_prefix="$cached_template_prefix"
        else
            template_prefix="$source_template_prefix"
        fi
    else
        template_prefix="$source_template_prefix"
    fi

    tmp_prefix="$(mktemp -d "$test_basedir/xpilot-wine-test.XXXXXX")"

    local cloned_from_template=false
    if [ -n "$template_prefix" ]; then
        # Create the prefix by cloning the template (to keep changes isolated)
        rm -rf "$tmp_prefix"
        wine_clone_prefix_template "$template_prefix" "$tmp_prefix" "cached template"
        cloned_from_template=true
    fi

    echo ""
    echo "=========================================="
    echo "Wine MSI smoke test"
    echo "=========================================="
    echo "MSI: $msi_path"
    echo "WINEPREFIX: $tmp_prefix"
    echo "Log: $tmp_log"
    echo ""

    # Ensure we don't accidentally inherit a user's WINEPREFIX while running the test
    local saved_wineprefix="${WINEPREFIX:-}"
    local saved_winedebug="${WINEDEBUG:-}"
    local saved_winedlloverrides="${WINEDLLOVERRIDES:-}"
    export WINEPREFIX="$tmp_prefix"

    # Heuristic: if we're running from a template, keep its arch; otherwise default to win64.
    if [ -z "$template_prefix" ] && [ -z "${WINEARCH:-}" ]; then
        export WINEARCH=win64
    fi

    # Unattended mode: avoid Wine background helpers popping up dialogs and blocking the script.
    # In particular, shortcut/menu helpers and Windows Error Reporting can trigger GUI prompts.
    export WINEDEBUG="${XPILOT_WINE_TEST_WINEDEBUG:--all}"
    if [ -n "$saved_winedlloverrides" ]; then
        # Also disable Mono/Gecko prompts (mscoree/mshtml) which commonly block unattended runs.
        export WINEDLLOVERRIDES="mscoree,mshtml=;winemenubuilder.exe=d;werfault.exe=d;wermgr.exe=d;$saved_winedlloverrides"
    else
        export WINEDLLOVERRIDES="mscoree,mshtml=;winemenubuilder.exe=d;werfault.exe=d;wermgr.exe=d"
    fi

    # Initialize prefix (this can trigger first-run UI and/or be silent for long periods).
    # If we cloned from a prepared template, skipping wineboot is usually faster and avoids hangs.
    if [ "$cloned_from_template" = true ] && [ -z "${XPILOT_WINE_TEST_FORCE_WINEBOOT:-}" ]; then
        echo "Skipping wineboot (prefix cloned from prepared template)."
    else
        local wineboot_max="${XPILOT_WINE_TEST_WINEBOOT_MAX:-600}"
        local wineboot_idle="${XPILOT_WINE_TEST_WINEBOOT_IDLE:-120}"
        echo "Initializing Wine prefix (wineboot -u, max ${wineboot_max}s, idle ${wineboot_idle}s)..."
        echo "  wineboot log: $tmp_wineboot_log"
        set +e
        wine_run_watchdog "wineboot" "$wineboot_max" "$wineboot_idle" "$tmp_wineboot_log" wineboot -u
        set -e
    fi

    # Best-effort: disable Windows Error Reporting UI inside the prefix.
    # (This helps prevent "Do you want to view information about this issue?" dialogs.)
    local winereg_max="${XPILOT_WINE_TEST_REG_MAX:-30}"
    local winereg_idle="${XPILOT_WINE_TEST_REG_IDLE:-10}"
    echo "Disabling WER UI in prefix (max ${winereg_max}s, idle ${winereg_idle}s)..."
    set +e
    wine_run_watchdog "wine reg add (WER Disabled)" "$winereg_max" "$winereg_idle" "$tmp_wine_reg_log" \
        wine reg add "HKCU\\Software\\Microsoft\\Windows\\Windows Error Reporting" /v Disabled /t REG_DWORD /d 1 /f
    wine_run_watchdog "wine reg add (WER DontShowUI)" "$winereg_max" "$winereg_idle" "$tmp_wine_reg_log" \
        wine reg add "HKCU\\Software\\Microsoft\\Windows\\Windows Error Reporting" /v DontShowUI /t REG_DWORD /d 1 /f
    set -e

    # Always run quietly to avoid hanging on dialogs (override with XPILOT_WINE_TEST_MSI_UI=1).
    local msiexec_args=(/i "$msi_path" DISABLEADVTSHORTCUTS=1 REBOOT=ReallySuppress /qn /norestart /l*v "$tmp_log")
    if [ -n "${XPILOT_WINE_TEST_MSI_UI:-}" ]; then
        msiexec_args=(/i "$msi_path" DISABLEADVTSHORTCUTS=1 REBOOT=ReallySuppress /l*v "$tmp_log")
    fi

    local msi_max="${XPILOT_WINE_TEST_MSI_MAX:-300}"
    local msi_idle="${XPILOT_WINE_TEST_MSI_IDLE:-30}"
    echo "Installing MSI via Wine (max ${msi_max}s, idle ${msi_idle}s)..."
    set +e
    wine_run_watchdog "wine msiexec" "$msi_max" "$msi_idle" "$tmp_msiexec_out" wine msiexec "${msiexec_args[@]}"
    local msi_rc=$?
    set -e

    if [ $msi_rc -ne 0 ]; then
        echo ""
        echo "Wine MSI install returned non-zero exit code: $msi_rc"
        echo "Check log: $tmp_log"
        echo "Wine msiexec output: $tmp_msiexec_out"
    else
        echo ""
        echo "Wine MSI install completed (exit code 0)."
    fi

    # Attempt to locate and launch the installed executable (best effort)
    local exe_path=""
    exe_path="$(wine_find_installed_xpilot_exe "$tmp_prefix" 2>/dev/null || true)"

    if [ -n "$exe_path" ]; then
        echo "Installed exe: $exe_path"
        if [ -n "${DISPLAY:-}" ] || [ -n "${WAYLAND_DISPLAY:-}" ]; then
            # Create a marker file so we can later find any logs created during/after launch.
            local marker="$tmp_prefix/.xpilot-wine-test.marker"
            touch "$marker" 2>/dev/null || true

            local install_dir
            install_dir="$(dirname "$exe_path")"

            echo "Launching under Wine (best effort; capturing output)..."
            set +e
            if command -v timeout >/dev/null 2>&1; then
                # The Windows SDL client defaults to fullscreen; under Wine this can fail when it tries to
                # change display settings. Force windowed mode for the smoke test.
                (cd "$install_dir" && XPILOT_WINDOWED=1 timeout 10s wine "$exe_path" >"$tmp_run_log" 2>&1)
            else
                (cd "$install_dir" && XPILOT_WINDOWED=1 wine "$exe_path" >"$tmp_run_log" 2>&1 &)
            fi
            local run_rc=$?
            set -e
            echo "Wine run log: $tmp_run_log"

            # Try to capture XPilot's own log file(s) from Windows %TEMP% (Wine usually maps it to C:\users\<user>\Temp\).
            mkdir -p "$tmp_clientlogs_dir" >/dev/null 2>&1 || true

            # Also capture any logs written next to the installed executable (XPilot writes xpilot-debug.log there).
            local found_install_logs=""
            for f in "$install_dir/xpilot-debug.log" "$install_dir/stdout.txt" "$install_dir/stderr.txt"; do
                if [ -f "$f" ] && [ "$f" -nt "$marker" ]; then
                    found_install_logs="${found_install_logs}${f}"$'\n'
                fi
            done
            if [ -n "$found_install_logs" ]; then
                echo "Found client log(s) next to the installed executable:"
                while IFS= read -r f; do
                    [ -z "$f" ] && continue
                    echo "  - $f"
                    cp -a "$f" "$tmp_clientlogs_dir/" 2>/dev/null || true
                done <<< "$found_install_logs"
                echo "Copied to: $tmp_clientlogs_dir"
            fi

            local found_client_logs=""
            found_client_logs="$(find "$tmp_prefix/drive_c/users" -type f -newer "$marker" \
                \( -iname '*xpilot*' -o -iname '*.log' -o -iname '*.txt' \) 2>/dev/null | head -50 || true)"
            if [ -n "$found_client_logs" ]; then
                echo "Found potential client log(s) created during launch:"
                while IFS= read -r f; do
                    [ -z "$f" ] && continue
                    echo "  - $f"
                    cp -a "$f" "$tmp_clientlogs_dir/" 2>/dev/null || true
                done <<< "$found_client_logs"
                echo "Copied to: $tmp_clientlogs_dir"
            else
                if [ -z "$found_install_logs" ]; then
                    echo "No obvious client log created under the Wine prefix."
                fi
            fi

            # Don't treat a timeout as failure; it usually means the GUI stayed open.
            if [ "$run_rc" -ne 0 ] && [ "$run_rc" -ne 124 ]; then
                echo "Wine client launch exited non-zero: $run_rc (details in $tmp_run_log)"
            fi
        else
            echo "No graphical display detected; skipping launch."
        fi
    else
        echo "Installed exe not found in expected location under the Wine prefix."
    fi

    # Restore env and cleanup
    if [ -n "$saved_wineprefix" ]; then
        export WINEPREFIX="$saved_wineprefix"
    else
        unset WINEPREFIX
    fi
    if [ -n "$saved_winedebug" ]; then
        export WINEDEBUG="$saved_winedebug"
    else
        unset WINEDEBUG
    fi
    if [ -n "$saved_winedlloverrides" ]; then
        export WINEDLLOVERRIDES="$saved_winedlloverrides"
    else
        unset WINEDLLOVERRIDES
    fi

    echo ""
    echo "Keeping Wine test prefix: $tmp_prefix"
    echo "Keeping Wine test log: $tmp_log"

    # Don't fail the overall build just because Wine returned non-zero (Wine quirks are common)
    return 0
}

echo "=========================================="
echo "XPilot NG Windows Cross-Compilation"
echo "=========================================="
echo ""

# Sudo helper - only prompts if actually needed
SUDO_CACHED=false
ensure_sudo() {
    if [ "$SUDO_CACHED" = true ]; then
        return 0
    fi
    if command -v sudo >/dev/null 2>&1; then
        echo "  (sudo access needed to install dependencies to system MinGW directories)"
        if sudo -v; then
            SUDO_CACHED=true
            # Keep sudo alive for the duration of the script
            (while true; do sudo -n true; sleep 60; kill -0 "$$" || exit; done 2>/dev/null) &
            SUDO_PID=$!
        else
            echo "  WARNING: sudo not available, may fail to install dependencies"
            return 1
        fi
    fi
    return 0
}

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

    ensure_sudo || return 1

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
        ensure_sudo && sudo apt-get update -qq
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
    ensure_sudo || { echo "  ERROR: sudo required to install zlib"; return 1; }
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
    ensure_sudo || { echo "  ERROR: sudo required to install libpng"; return 1; }
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
    ensure_sudo || { echo "  ERROR: sudo required to install expat"; return 1; }
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


# Create build directory (incremental builds - let Makefiles decide what needs recompiling)
if [ -d "$BUILD_DIR" ]; then
    if [ "$FORCE_CLEAN" = true ]; then
        echo "Cleaning previous Windows build (--clean specified)..."
        rm -rf "$BUILD_DIR"
    else
        echo "Using existing build directory (incremental build)..."
    fi
else
    echo "Creating build directory..."
fi

mkdir -p "$BUILD_DIR" 2>/dev/null
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
# Override CONF_DATADIR to "data/" for Windows (Makefile.am sets it to pkgdatadir which is wrong for Windows)
# Also override CONF_FONTDIR to "data/" (fonts are in data/ directly, not data/fonts/)
# The double backslashes escape the quotes so they're passed through to the compiler correctly
export CPPFLAGS="-I${MINGW_INCLUDE} -I${MINGW_INCLUDE}/SDL -D_WINDOWS -DWIN32 -DHAVE_GETTIMEOFDAY -I${BUILD_DIR_ABS}/src/common -DCONF_DATADIR=\\\"data/\\\" -DCONF_FONTDIR=\\\"data/\\\""
export CFLAGS="-I${MINGW_INCLUDE} -I${MINGW_INCLUDE}/SDL -D_WINDOWS -DWIN32 -DHAVE_GETTIMEOFDAY -I${BUILD_DIR_ABS}/src/common -DCONF_DATADIR=\\\"data/\\\" -DCONF_FONTDIR=\\\"data/\\\""
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
    ensure_sudo || { echo "  ERROR: sudo required to install SDL"; exit 1; }
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
    ensure_sudo || { echo "  ERROR: sudo required to install FreeType2"; exit 1; }
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
    ensure_sudo || { echo "  ERROR: sudo required to install SDL_ttf"; exit 1; }
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
    ensure_sudo || { echo "  ERROR: sudo required to install SDL_image"; exit 1; }
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

# Copy SDL m4 macro if not present (needed for AM_PATH_SDL)
if [ ! -f "$SCRIPT_DIR/sdl.m4" ]; then
    SDL_M4_FOUND=false
    for SDL_M4_PATH in \
        /usr/x86_64-w64-mingw32/share/aclocal/sdl.m4 \
        /usr/share/aclocal/sdl.m4 \
        /usr/local/share/aclocal/sdl.m4; do
        if [ -f "$SDL_M4_PATH" ]; then
            echo "  Copying sdl.m4 from $SDL_M4_PATH..."
            cp "$SDL_M4_PATH" "$SCRIPT_DIR/"
            SDL_M4_FOUND=true
            break
        fi
    done
    if [ "$SDL_M4_FOUND" = false ]; then
        echo "  WARNING: sdl.m4 not found - AM_PATH_SDL may fail"
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

# Pass through any additional configure options
for arg in "$@"; do
    if [[ "$arg" != "--enable-sdl-client" && "$arg" != "--disable-sdl-client" ]]; then
        CONFIGURE_OPTS+=("$arg")
    fi
done

# Only run configure if needed (Makefile doesn't exist or configure is newer)
NEED_CONFIGURE=false
if [ ! -f "Makefile" ]; then
    NEED_CONFIGURE=true
elif [ "$SCRIPT_DIR/configure" -nt "Makefile" ]; then
    NEED_CONFIGURE=true
elif [ "$FORCE_CLEAN" = true ]; then
    NEED_CONFIGURE=true
fi

if [ "$NEED_CONFIGURE" = true ]; then
    echo "Configuring build..."
    # Patch configure only when we run it; doing it every run would touch configure
    # and force NEED_CONFIGURE next time, causing full rebuilds every build
    echo "  Patching configure for Windows cross-compilation..."
    sed -i 's/if test x\$no_x == xyes; then/if test x$no_x == xyes \&\& test x$enable_sdl_client != xyes; then/' "$SCRIPT_DIR/configure"
    sed -i 's/-Dmain=SDL_main//g' "$SCRIPT_DIR/configure"
    sed -i 's/main (void)/main (int argc, char *argv[])/g' "$SCRIPT_DIR/configure"
    sed -i 's/main ()/main (int argc, char *argv[])/g' "$SCRIPT_DIR/configure"

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
else
    echo "Build already configured (use --clean to reconfigure)..."
fi

# Remove CONF_DATADIR and CONF_FONTDIR from Makefile AM_CPPFLAGS (we override them in CPPFLAGS/CFLAGS)
# This prevents redefinition errors since Makefile.am may set them
echo "  Removing CONF_DATADIR and CONF_FONTDIR from Makefile AM_CPPFLAGS (using override from build script)..."
find . -name "Makefile" -type f -exec sed -i 's/-DCONF_DATADIR="[^"]*"//g' {} \; 2>/dev/null || true
find . -name "Makefile" -type f -exec sed -i 's/-DCONF_DATADIR=\\"[^\\"]*\\"//g' {} \; 2>/dev/null || true
find . -name "Makefile" -type f -exec sed -i 's/-DCONF_FONTDIR="[^"]*"//g' {} \; 2>/dev/null || true
find . -name "Makefile" -type f -exec sed -i 's/-DCONF_FONTDIR=\\"[^\\"]*\\"//g' {} \; 2>/dev/null || true

# Embed application icon into the Windows executable (only if needed)
ICON_SRC="$SCRIPT_DIR/src/client/NT/res/xpilot.ico"
ICON_OBJ="src/client/sdl/xpilot_icon.o"
ICON_RC="src/client/sdl/xpilot_icon.rc"

# Check if we need to (re)compile the icon resource
NEED_ICON_COMPILE=false
if [ ! -f "$ICON_OBJ" ]; then
    NEED_ICON_COMPILE=true
elif [ "$ICON_SRC" -nt "$ICON_OBJ" ]; then
    NEED_ICON_COMPILE=true
elif [ "$NEED_CONFIGURE" = true ]; then
    # Makefile was regenerated, need to patch it again
    NEED_ICON_COMPILE=true
fi

if [ "$NEED_ICON_COMPILE" = true ] && [ -f "$ICON_SRC" ]; then
    echo "Setting up icon resource for Windows executable..."
    # Create resource directory and copy icon
    mkdir -p "src/client/sdl/res"
    cp "$ICON_SRC" "src/client/sdl/res/"

    # Create a minimal resource file for MinGW (the original uses MFC headers)
    cat > "$ICON_RC" << 'RCEOF'
// Minimal resource file for XPilot icon (MinGW compatible)
// The "1" ID ensures this is the application icon
1 ICON "res/xpilot.ico"
RCEOF

    # Compile the resource file
    echo "  Compiling icon resource..."
    WINDRES="${MINGW_PREFIX}-windres"
    if $WINDRES "$ICON_RC" -o "$ICON_OBJ" 2>/dev/null; then
        echo "  ✓ Icon resource compiled"

        # Modify the SDL client Makefile to include the icon resource in the link
        # (only if not already added)
        if [ -f "src/client/sdl/Makefile" ]; then
            if ! grep -q "xpilot_icon.o" "src/client/sdl/Makefile"; then
                sed -i 's|xpilot_ng_sdl_LDADD = |xpilot_ng_sdl_LDADD = xpilot_icon.o |' src/client/sdl/Makefile
                echo "  ✓ Icon will be embedded in executable"
            fi
        fi
    else
        echo "  Warning: Could not compile icon resource (windres failed)"
    fi
elif [ ! -f "$ICON_SRC" ]; then
    echo "  Warning: Icon file not found at $ICON_SRC"
fi

echo "Building Windows client..."

# Generic function to detect changed defines and rebuild affected files
# Extracts all -D defines from CPPFLAGS/CFLAGS, compares to previous build,
# and touches source files that use any changed defines
check_defines_and_rebuild() {
    local DEFINES_STAMP="$(pwd)/.cpp_defines_stamp"
    local CLIENT_EXE_CHECK="src/client/sdl/xpilot-ng-sdl.exe"

    # Extract all -D defines from CPPFLAGS and CFLAGS
    # Handles both -DNAME and -DNAME=value formats
    # For -DNAME="value", we need to handle escaped quotes (\\" becomes ")
    # First, split on spaces before -D, then extract the define name and value
    local CURRENT_DEFINES=$(echo "$CPPFLAGS $CFLAGS" | \
        tr ' ' '\n' | \
        grep '^-D' | \
        sed 's/^-D//' | \
        sed 's/\\\\\\"/"/g' | \
        sed 's/\\\\"/"/g' | \
        sed 's/\\"/"/g' | \
        sort | \
        uniq)

    # Read previous defines from stamp file
    local LAST_DEFINES=""
    if [ -f "$DEFINES_STAMP" ]; then
        LAST_DEFINES=$(cat "$DEFINES_STAMP" 2>/dev/null || echo "")
    fi

    # Check if executable is older than stamp (built before defines were tracked)
    local NEED_REBUILD=false
    if [ -f "$CLIENT_EXE_CHECK" ] && [ -f "$DEFINES_STAMP" ]; then
        if [ "$CLIENT_EXE_CHECK" -ot "$DEFINES_STAMP" ]; then
            NEED_REBUILD=true
        fi
    fi

    # Compare current and last defines
    local DEFINES_CHANGED=false
    if [ -z "$LAST_DEFINES" ] || [ "$NEED_REBUILD" = true ]; then
        DEFINES_CHANGED=true
    elif [ "$CURRENT_DEFINES" != "$LAST_DEFINES" ]; then
        DEFINES_CHANGED=true
    fi

    if [ "$DEFINES_CHANGED" = true ]; then
        if [ -z "$LAST_DEFINES" ]; then
            echo "  Defines not previously recorded (first run or --clean) - detecting affected files..."
        elif [ "$NEED_REBUILD" = true ]; then
            echo "  Executable was built before defines were tracked - detecting affected files..."
        else
            echo "  Defines changed - detecting affected files..."
        fi

        # Find which defines changed (if we have previous values)
        local CHANGED_DEFINES=""
        if [ -n "$LAST_DEFINES" ]; then
            # Find defines that are in current but not in last, or have different values
            while IFS= read -r define; do
                local def_name=$(echo "$define" | cut -d'=' -f1)
                local def_value=$(echo "$define" | cut -d'=' -f2-)
                local last_value=$(echo "$LAST_DEFINES" | grep "^${def_name}=" | cut -d'=' -f2-)
                if [ "$def_value" != "$last_value" ]; then
                    CHANGED_DEFINES="${CHANGED_DEFINES}${def_name} "
                fi
            done <<< "$CURRENT_DEFINES"
        else
            # First run: check all defines
            CHANGED_DEFINES=$(echo "$CURRENT_DEFINES" | cut -d'=' -f1 | tr '\n' ' ')
        fi

        # For each changed define, find source files that use it and touch them
        local TOUCHED_FILES=""
        for def_name in $CHANGED_DEFINES; do
            # Skip system defines that are unlikely to be in source code
            # (these are typically only used in preprocessor conditionals)
            if [[ "$def_name" =~ ^(_WINDOWS|WIN32|HAVE_|__) ]]; then
                continue
            fi

            # Search for the define name in source files (case-sensitive)
            # Look in .c, .cpp, .h files in src/ directory
            # Pattern matches: word boundary before and after the define name
            local FOUND_FILES=$(grep -r -l --include="*.c" --include="*.cpp" --include="*.h" \
                -E "(^|[^A-Za-z0-9_])${def_name}([^A-Za-z0-9_]|$)" \
                "$SCRIPT_DIR/src" 2>/dev/null || true)

            if [ -n "$FOUND_FILES" ]; then
                local file_count=$(echo "$FOUND_FILES" | wc -l)
                echo "    Define ${def_name} changed - found ${file_count} affected file(s)"
                for f in $FOUND_FILES; do
                    # Only touch .c and .cpp files (headers will be recompiled when .c files are)
                    if [[ "$f" =~ \.(c|cpp)$ ]]; then
                        if [ -f "$f" ]; then
                            touch "$f" 2>/dev/null || true
                            TOUCHED_FILES="${TOUCHED_FILES}$(basename "$f") "
                        fi
                    fi
                done
            fi
        done

        if [ -n "$TOUCHED_FILES" ]; then
            echo "  Touched files: $(echo "$TOUCHED_FILES" | tr ' ' '\n' | sort -u | tr '\n' ' ')"
        fi
    fi

    # Write current defines to stamp file
    echo "$CURRENT_DEFINES" > "$DEFINES_STAMP" 2>/dev/null || {
        # Fallback if pwd failed or we're not in build dir
        echo "$CURRENT_DEFINES" > "$BUILD_DIR/.cpp_defines_stamp" 2>/dev/null || true
    }
}

# Check for changed defines and rebuild affected files
check_defines_and_rebuild

# For Windows, only build the client (not server or replay which have Unix dependencies)
# Build common first, then client
# Add -Werror during make (not during configure as it breaks configure tests)
set -o pipefail
MAKE_FAILED=0
make -C src/common V=1 CFLAGS="$CFLAGS -Werror" 2>&1 | tee "$SCRIPT_DIR/build-windows-make.log" || MAKE_FAILED=1
make -C src/client V=1 CFLAGS="$CFLAGS -Werror" 2>&1 | tee -a "$SCRIPT_DIR/build-windows-make.log" || MAKE_FAILED=1
set +o pipefail

if [ "$MAKE_FAILED" -ne 0 ]; then
    echo ""
    echo "Build failed. Last 50 lines of build log:"
    tail -50 "$SCRIPT_DIR/build-windows-make.log"
    exit 1
fi

# Check if client exe was built (may be in source or build directory)
CLIENT_EXE=""
if [ -f "$SCRIPT_DIR/src/client/sdl/xpilot-ng-sdl.exe" ]; then
    CLIENT_EXE="$SCRIPT_DIR/src/client/sdl/xpilot-ng-sdl.exe"
elif [ -f "src/client/sdl/xpilot-ng-sdl.exe" ]; then
    CLIENT_EXE="src/client/sdl/xpilot-ng-sdl.exe"
fi

if [ -z "$CLIENT_EXE" ]; then
    echo ""
    echo "Build failed: client executable not found."
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

# Copy icon for installer shortcuts
echo "  Copying icon..."
ICON_SRC="$SCRIPT_DIR/src/client/NT/res/xpilot.ico"
if [ -f "$ICON_SRC" ]; then
    cp "$ICON_SRC" "$INSTALLER_DIR/"
fi

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

# Some distros/repos may contain a corrupted FreeSansBoldOblique.ttf (seen as a bad sfnt header),
# which makes the Windows SDL client exit immediately under Wine (SDL_ttf can't load the font).
# If we detect an invalid font, try to replace it with a known-good system font at build time.
fixup_windows_font() {
    # Depending on how data files are laid out in the tree, the font may be under data/ or data/fonts/.
    local dest_fonts=(
        "$INSTALLER_DIR/data/FreeSansBoldOblique.ttf"
        "$INSTALLER_DIR/data/fonts/FreeSansBoldOblique.ttf"
    )

    local any_found=false
    local any_invalid=false
    local invalid_paths=()

    for dest_font in "${dest_fonts[@]}"; do
        if [ ! -f "$dest_font" ]; then
            continue
        fi
        any_found=true

        # Validate basic sfnt header fields (fast sanity check).
        local valid=0
        if command -v python3 >/dev/null 2>&1; then
            python3 - <<'PY' "$dest_font" >/dev/null 2>&1 || valid=1
import struct, sys, pathlib, math
p = pathlib.Path(sys.argv[1])
d = p.read_bytes()
if len(d) < 12:
    raise SystemExit(1)
sfntVersion, numTables, searchRange, entrySelector, rangeShift = struct.unpack(">IHHHH", d[:12])
if sfntVersion not in (0x00010000, 0x4F54544F, 0x74727565, 0x74797031):  # 1.0, OTTO, true, typ1
    raise SystemExit(1)
if numTables == 0 or numTables > 200:
    raise SystemExit(1)
expected_search = 16 * (2 ** int(math.floor(math.log(numTables, 2))))
expected_entry = int(math.log(expected_search // 16, 2))
expected_shift = numTables * 16 - expected_search
if searchRange != expected_search or entrySelector != expected_entry or rangeShift != expected_shift:
    raise SystemExit(1)
PY
        else
            # No python3: skip validation.
            valid=0
        fi

        if [ "$valid" -ne 0 ]; then
            any_invalid=true
            invalid_paths+=("$dest_font")
        fi
    done

    if [ "$any_found" != true ]; then
        return 0
    fi
    if [ "$any_invalid" != true ]; then
        return 0
    fi

    echo "  WARNING: Detected invalid FreeSansBoldOblique.ttf in installer data:"
    for p in "${invalid_paths[@]}"; do
        echo "    - $p"
    done
    echo "  Attempting replacement with a known-good system font..."
    local candidates=(
        "/usr/share/fonts/truetype/freefont/FreeSansBoldOblique.ttf"
        "/usr/share/fonts/gnu-free/FreeSansBoldOblique.ttf"
        "/usr/share/fonts/truetype/FreeSansBoldOblique.ttf"
    )
    for cand in "${candidates[@]}"; do
        if [ -f "$cand" ]; then
            for p in "${invalid_paths[@]}"; do
                cp -f "$cand" "$p"
            done
            echo "  Using system font: $cand"
            return 0
        fi
    done

    if [ -n "${XPILOT_ALLOW_INVALID_FONT:-}" ]; then
        echo "  WARNING: No system replacement font found; proceeding anyway because XPILOT_ALLOW_INVALID_FONT is set."
        echo "           The resulting installer is likely to fail at runtime."
        return 0
    fi

    echo "  ERROR: FreeSansBoldOblique.ttf appears to be invalid, and no system replacement was found."
    echo "         Refusing to package a known-bad font."
    echo "         Install FreeFont and retry (examples):"
    echo "           - Debian/Ubuntu: sudo apt-get install -y fonts-freefont-ttf"
    echo "           - Fedora:       sudo dnf install -y gnu-free-fonts"
    exit 1
}
fixup_windows_font

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

    <Media Id="1" Cabinet="xpilot.cab" EmbedCab="yes" CompressionLevel="high" />

    <!-- Application icon for shortcuts -->
    <Icon Id="XPilotIcon" SourceFile="$INSTALLER_DIR/xpilot.ico" />
    <Property Id="ARPPRODUCTICON" Value="XPilotIcon" />

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

    # Add README
    cat >> "$WXS_FILE" << READMEEOF
      <Component Id="ReadmeComponent" Guid="*">
        <File Id="ReadmeFile" Source="$INSTALLER_DIR/README.txt" KeyPath="yes" />
      </Component>
READMEEOF

    cat >> "$WXS_FILE" << WIXEOF2
    </DirectoryRef>

    <!-- Data directory -->
    <DirectoryRef Id="DataDir">
WIXEOF2

    # Add data files to WXS (recursively)
    DATA_COMP_ID=1
    find "$INSTALLER_DIR/data" -type f | while read -r datafile; do
        REL_PATH="${datafile#$INSTALLER_DIR/data/}"
        FILE_ID="DataFile_$DATA_COMP_ID"
        COMP_ID="DataComp_$DATA_COMP_ID"
        cat >> "$WXS_FILE" << DATAEOF
      <Component Id="$COMP_ID" Guid="*">
        <File Id="$FILE_ID" Source="$datafile" Name="$(basename "$datafile")" KeyPath="yes" />
      </Component>
DATAEOF
        DATA_COMP_ID=$((DATA_COMP_ID + 1))
    done
    # Save count for later
    DATA_COMP_COUNT=$((DATA_COMP_ID - 1))

    cat >> "$WXS_FILE" << WIXEOF2B
    </DirectoryRef>

    <!-- Doc directory -->
    <DirectoryRef Id="DocDir">
WIXEOF2B

    # Add doc files to WXS
    DOC_COMP_ID=1
    find "$INSTALLER_DIR/doc" -type f | while read -r docfile; do
        FILE_ID="DocFile_$DOC_COMP_ID"
        COMP_ID="DocComp_$DOC_COMP_ID"
        cat >> "$WXS_FILE" << DOCEOF
      <Component Id="$COMP_ID" Guid="*">
        <File Id="$FILE_ID" Source="$docfile" Name="$(basename "$docfile")" KeyPath="yes" />
      </Component>
DOCEOF
        DOC_COMP_ID=$((DOC_COMP_ID + 1))
    done
    DOC_COMP_COUNT=$((DOC_COMP_ID - 1))

    cat >> "$WXS_FILE" << WIXEOF2C
    </DirectoryRef>

    <DirectoryRef Id="ApplicationProgramsFolder">
      <Component Id="ApplicationShortcut" Guid="*">
        <Shortcut Id="ApplicationStartMenuShortcut"
                  Name="XPilot NG"
                  Description="XPilot NG Space Combat Game"
                  Target="[INSTALLDIR]$CLIENT_NAME"
                  WorkingDirectory="INSTALLDIR"
                  Icon="XPilotIcon" />
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
                  WorkingDirectory="INSTALLDIR"
                  Icon="XPilotIcon" />
        <RegistryValue Root="HKCU" Key="Software\\XPilotNG" Name="desktopshortcut" Type="integer" Value="1" KeyPath="yes" />
      </Component>
    </DirectoryRef>

    <Feature Id="ProductFeature" Title="XPilot NG" Level="1">
      <ComponentRef Id="MainExecutable" />
      <ComponentRef Id="ReadmeComponent" />
      <ComponentRef Id="ApplicationShortcut" />
      <ComponentRef Id="DesktopShortcut" />
WIXEOF2C

    # Add DLL component refs
    for i in $(seq 1 $((DLL_COMP_ID - 1))); do
        echo "      <ComponentRef Id=\"DLL_$i\" />" >> "$WXS_FILE"
    done

    # Add data component refs (count the files again)
    DATA_COUNT=$(find "$INSTALLER_DIR/data" -type f | wc -l)
    for i in $(seq 1 $DATA_COUNT); do
        echo "      <ComponentRef Id=\"DataComp_$i\" />" >> "$WXS_FILE"
    done

    # Add doc component refs
    DOC_COUNT=$(find "$INSTALLER_DIR/doc" -type f | wc -l)
    for i in $(seq 1 $DOC_COUNT); do
        echo "      <ComponentRef Id=\"DocComp_$i\" />" >> "$WXS_FILE"
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

# Optional: smoke-test MSI in a temporary Wine prefix
if [ "$WINE_TEST" = true ]; then
    if [[ "$INSTALLER_FILE" == *.msi ]]; then
        wine_test_msi "$INSTALLER_FILE"
    else
        echo ""
        echo "Wine test skipped: installer is not an MSI: $INSTALLER_FILE"
    fi
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
