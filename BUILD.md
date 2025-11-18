# Build Instructions

Complete guide for building BandwidthThrottler on Windows.

## Prerequisites

### Required Software

1. **CMake 3.16 or higher**
   - Download from: https://cmake.org/download/
   - Choose "Windows x64 Installer"
   - During installation, select "Add CMake to system PATH"

2. **Qt6 (Core and Widgets modules)**
   - Download from: https://www.qt.io/download
   - Install Qt 6.5.0 or later
   - Select "Qt 6.x.x" → "MSVC 2019 64-bit" (for Visual Studio) or "MinGW 11.2.0 64-bit" (for MinGW)
   - Ensure "Qt Core" and "Qt Widgets" are selected

3. **C++ Compiler** (choose one):

   **Option A: Visual Studio 2019/2022** (Recommended)
   - Download Visual Studio Community (free): https://visualstudio.microsoft.com/
   - During installation, select "Desktop development with C++"
   - This includes the Windows SDK

   **Option B: MinGW-w64**
   - Download from: https://www.mingw-w64.org/downloads/
   - Or use MSYS2: https://www.msys2.org/

### Verify Installation

Open PowerShell or Command Prompt and verify:

```bash
cmake --version          # Should show 3.16+
qmake --version          # Should show Qt6 version
```

## Building with Visual Studio

### Step 1: Open Developer Command Prompt

- Press `Win + R`, type `cmd`, press Enter
- Or search for "Developer Command Prompt for VS 2022" in Start Menu

### Step 2: Navigate to Project Directory

```bash
cd path\to\BandwithLimiter
```

### Step 3: Create Build Directory

```bash
mkdir build
cd build
```

### Step 4: Configure CMake

```bash
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
```

**If Qt6 is not found automatically**, specify the path:

```bash
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DQt6_DIR="C:/Qt/6.5.0/msvc2019_64/lib/cmake/Qt6"
```

Replace `C:/Qt/6.5.0/msvc2019_64` with your actual Qt6 installation path.

### Step 5: Build

```bash
cmake --build . --config Release
```

### Step 6: Find Executable

The executable will be at:
```
build/Release/BandwidthThrottler.exe
```

## Building with MinGW

### Step 1: Open MinGW Terminal

- If using MSYS2, open "MSYS2 MinGW 64-bit"
- If using standalone MinGW, use Command Prompt with MinGW in PATH

### Step 2: Navigate to Project

```bash
cd /c/path/to/BandwithLimiter
```

### Step 3: Create Build Directory

```bash
mkdir build
cd build
```

### Step 4: Configure CMake

```bash
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
```

**If Qt6 is not found**, specify the path:

```bash
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DQt6_DIR="C:/Qt/6.5.0/mingw_64/lib/cmake/Qt6"
```

### Step 5: Build

```bash
cmake --build .
```

### Step 6: Find Executable

The executable will be at:
```
build/BandwidthThrottler.exe
```

## Troubleshooting

### CMake Error: "Could not find Qt6"

**Solution**: Specify Qt6 path manually

```bash
cmake .. -DQt6_DIR="C:/Qt/6.5.0/msvc2019_64/lib/cmake/Qt6"
```

To find your Qt6 installation:
- Default location: `C:/Qt/6.x.x/`
- Check Qt Maintenance Tool for installation path
- Look for `lib/cmake/Qt6` directory

### Visual Studio Generator Not Found

**Solution**: Use the correct generator name for your Visual Studio version:

- Visual Studio 2022: `"Visual Studio 17 2022"`
- Visual Studio 2019: `"Visual Studio 16 2019"`
- Visual Studio 2017: `"Visual Studio 15 2017"`

### Linker Errors

**Possible causes**:
- Qt6 libraries not found
- Windows SDK not installed
- Wrong architecture (32-bit vs 64-bit)

**Solutions**:
- Ensure Qt6 is installed for the correct compiler (MSVC or MinGW)
- Install Windows SDK via Visual Studio Installer
- Use `-A x64` flag for 64-bit builds

### Missing DLL Files

If the executable can't find Qt DLLs:

1. **Copy DLLs to executable directory**:
   - Copy from: `C:/Qt/6.5.0/msvc2019_64/bin/`
   - Copy to: `build/Release/`
   - Required DLLs: `Qt6Core.dll`, `Qt6Widgets.dll`, `Qt6Gui.dll`

2. **Or add Qt bin directory to PATH**:
   ```bash
   set PATH=C:\Qt\6.5.0\msvc2019_64\bin;%PATH%
   ```

### Build Fails with "Permission Denied"

**Solution**: Close any programs that might be using the build directory, or run as administrator.

## Advanced Configuration

### Debug Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```

### Custom Install Location

```bash
cmake .. -DCMAKE_INSTALL_PREFIX="C:/Program Files/BandwidthThrottler"
cmake --install . --config Release
```

### Verbose Build Output

```bash
cmake --build . --config Release --verbose
```

## Distribution

### Creating a Release Package

1. Build in Release mode
2. Copy required DLLs to the executable directory:
   - `Qt6Core.dll`
   - `Qt6Widgets.dll`
   - `Qt6Gui.dll`
   - `msvcp140.dll` (Visual C++ Redistributable)
   - `vcruntime140.dll`

3. Optionally use `windeployqt` (Qt tool):
   ```bash
   windeployqt.exe build/Release/BandwidthThrottler.exe
   ```

### Visual C++ Redistributable

Users need Visual C++ Redistributable if built with Visual Studio:
- Download: https://aka.ms/vs/17/release/vc_redist.x64.exe

## Next Steps

After building:
1. Test the executable
2. Run as administrator to test throttling functionality
3. See [README.md](README.md) for usage instructions
