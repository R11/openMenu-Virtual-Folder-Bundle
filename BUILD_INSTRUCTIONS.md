# Building openMenu with DC Now Feature

This document explains how to build openMenu with the fully functional DC Now (Dreamcast Live) network feature.

## Overview

The DC Now feature is now **FULLY IMPLEMENTED** with real network functionality:
- ✅ Complete HTTP client using KallistiOS network stack
- ✅ Custom lightweight JSON parser (no external dependencies)
- ✅ Async network requests with timeout support
- ✅ Real-time data fetching from dreamcast.online/now
- ✅ Automatic caching of results
- ✅ Thread-safe implementation

## Prerequisites

You have two options for building:

### Option 1: Using Docker (Recommended)

The project includes a DevContainer with the full KallistiOS toolchain pre-configured.

**Requirements:**
- Docker installed on your system
- ~2GB disk space for the Docker image

### Option 2: Native KallistiOS Toolchain

Install the KallistiOS toolchain manually:
- KallistiOS v2.1.x
- sh-elf-gcc toolchain
- CMake 3.19+

## Building with Docker (Recommended)

### Method 1: Using DevContainer (VS Code)

1. Open the project in VS Code
2. Install the "Remote - Containers" extension
3. Click "Reopen in Container" when prompted
4. Once inside the container, run:

```bash
cd /workspaces/openMenu-Virtual-Folder-Bundle/openMenu
mkdir build
cd build
cmake -G Ninja -DBUILD_DREAMCAST=ON -DBUILD_PC=OFF ..
ninja
```

The compiled `1ST_READ.BIN` will be in `build/bin/1ST_READ.BIN`

### Method 2: Using Docker Directly

```bash
# From the repository root
cd openMenu-Virtual-Folder-Bundle

# Pull the pre-built Docker image
docker pull sbstnc/openmenu-dev:0.2.2

# Or build it yourself
cd openMenu/docker
./build.sh 0.2.2
cd ../..

# Run the Docker container
docker run --rm -it \
  -v "$(pwd)":/workspace \
  -w /workspace/openMenu \
  sbstnc/openmenu-dev:0.2.2 \
  /bin/bash

# Inside the container:
source /opt/toolchains/dc/kos/environ.sh
mkdir -p build
cd build
cmake -G Ninja -DBUILD_DREAMCAST=ON -DBUILD_PC=OFF ..
ninja
```

### Quick Build Script

Save this as `build_openmenu.sh` in the repository root:

```bash
#!/bin/bash
set -e

echo "Building openMenu with DC Now feature..."

docker run --rm \
  -v "$(pwd)":/workspace \
  -w /workspace/openMenu \
  sbstnc/openmenu-dev:0.2.2 \
  /bin/bash -c "
    source /opt/toolchains/dc/kos/environ.sh && \
    rm -rf build && \
    mkdir -p build && \
    cd build && \
    cmake -G Ninja -DBUILD_DREAMCAST=ON -DBUILD_PC=OFF .. && \
    ninja
  "

echo "Build complete!"
echo "Binary location: openMenu/build/bin/1ST_READ.BIN"
```

Then run:
```bash
chmod +x build_openmenu.sh
./build_openmenu.sh
```

## Network Configuration Notes

### For Real Dreamcast Hardware

The DC Now feature requires a network connection on your Dreamcast:

**Option 1: Broadband Adapter (BBA)**
- Plug and play network connectivity
- Recommended for best performance
- Requires BBA hardware

**Option 2: DreamPi (Modem Emulation)**
- Uses Raspberry Pi as a modem emulator
- Connects via phone line port
- See https://dreamcast.wiki/DreamPi for setup

**Option 3: DC-Load Serial/IP**
- Development setup using serial cable
- Requires additional configuration

### Network Initialization

The openMenu application needs network support initialized. If your Dreamcast doesn't have network initialized at startup, you may need to add this to your main initialization code (already supported in openMenu):

The network initialization is handled automatically by KallistiOS when the Dreamcast boots with network hardware detected.

## Testing the Build

### On Dreamcast Hardware

1. Copy `1ST_READ.BIN` to your SD card in folder `01`
2. Boot your Dreamcast with GDEMU
3. Once in openMenu, press **L + R triggers simultaneously**
4. The DC Now popup should appear and attempt to fetch data
5. If connected to the internet, you'll see real player counts
6. If offline, you'll see an error message

### Testing Without Hardware (Stub Data)

To test the UI without Dreamcast hardware or network:

1. Edit `openMenu/src/openmenu/CMakeLists.txt`
2. Uncomment the stub data line:
   ```cmake
   target_compile_definitions(openmenu PRIVATE
       OPENMENU_BUILD_VERSION="${OPENMENU_VERSION}"
       DCNOW_USE_STUB_DATA=1  # Enable stub data
   )
   ```
3. Rebuild the project
4. The popup will show sample game data instead of making real network requests

## Build Output

After a successful build, you'll find:

```
openMenu/build/bin/
├── 1ST_READ.BIN          # Main Dreamcast executable
└── ...                   # Other build artifacts
```

## Implementation Details

### Network Stack

The DC Now feature uses:
- **KallistiOS lwIP**: Lightweight TCP/IP stack
- **BSD Sockets API**: Standard socket programming interface
- **Non-blocking I/O**: For responsive UI during network operations
- **Timeouts**: Configurable timeout support (default 5000ms)

### JSON Parsing

Custom minimal JSON parser:
- **Zero dependencies**: No external JSON libraries needed
- **Lightweight**: ~200 lines of code
- **Focused**: Only parses the specific format from dreamcast.online/now
- **Robust**: Handles malformed JSON gracefully

### Architecture

```
User Input (L+R)
    ↓
dcnow_setup()
    ↓
dcnow_fetch_data()
    ↓
http_get_request()
    ├─→ socket() → connect()
    ├─→ send() HTTP GET
    └─→ recv() response
        ↓
    HTTP header parsing
        ↓
    dcnow_json_parse()
        ↓
    Populate dcnow_data_t
        ↓
draw_dcnow_tr()
```

## Troubleshooting

### Build Errors

**Error: "network.h: No such file or directory"**
- Make sure you're building inside the Docker container
- Verify KOS environment is sourced: `source /opt/toolchains/dc/kos/environ.sh`

**Error: "CMake 3.19 or higher is required"**
- Update CMake or use the Docker container which has the correct version

**Error: "ninja: command not found"**
- Install Ninja: `apk add ninja` (Alpine), or use `make` instead:
  ```bash
  cmake -DBUILD_DREAMCAST=ON -DBUILD_PC=OFF ..
  make
  ```

### Runtime Errors

**"DNS lookup failed"**
- Check your Dreamcast network connection
- Verify DNS is configured correctly
- Test with other network apps (web browser, PSO)

**"Connection failed/timeout"**
- Ensure dreamcast.online is reachable
- Check firewall settings
- Increase timeout in `dcnow_fetch_data()` call (default 5000ms)

**"JSON parse error"**
- The server response format may have changed
- Enable debug logging to see raw JSON
- Report issue with the server response

## Performance Notes

- **First request**: ~2-5 seconds (DNS + connection + data fetch)
- **Cached requests**: Instant (no network call)
- **Timeout**: 5 seconds default (configurable)
- **Memory usage**: ~8KB for HTTP response buffer
- **Thread safety**: Non-blocking with `thd_pass()` calls

## File Manifest

New files added for DC Now feature:

```
openMenu/src/openmenu/src/dcnow/
├── dcnow_api.h          # Public API interface
├── dcnow_api.c          # Network implementation
├── dcnow_json.h         # JSON parser interface
└── dcnow_json.c         # JSON parser implementation
```

Modified files:
- `openMenu/src/openmenu/CMakeLists.txt` - Added new source files
- `openMenu/src/openmenu/src/ui/ui_*` - Integrated popup in all UI modes
- `openMenu/src/openmenu_settings/include/openmenu_settings.h` - Added DRAW_DCNOW_PLAYERS state
- `openMenu/src/openmenu/src/ui/ui_menu_credits.{c,h}` - Added popup drawing/input handlers

## Download Pre-built Binary

If you don't want to build yourself, check the Releases section for pre-built binaries:

https://github.com/likeagfeld/openMenu-Virtual-Folder-Bundle/releases

Look for the latest release with DC Now support.

## Support

For build issues or questions:
- GitHub Issues: https://github.com/likeagfeld/openMenu-Virtual-Folder-Bundle/issues
- Check DCNOW_IMPLEMENTATION.md for implementation details
- Review the KallistiOS documentation for toolchain issues

## License

The DC Now feature follows the same BSD 3-Clause license as openMenu.

---

**Built with ❤️ for the Dreamcast community**
