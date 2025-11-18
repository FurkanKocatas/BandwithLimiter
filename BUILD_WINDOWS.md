# Building Windows Executable from Linux

There are several ways to build a Windows executable from Linux:

## Option 1: GitHub Actions (Recommended - Easiest)

1. Push your code to GitHub
2. GitHub Actions will automatically build the Windows executable
3. Download the `.exe` file from the Actions tab

The workflow is already configured in `.github/workflows/build-windows.yml`

## Option 2: Cross-Compilation with MinGW-w64

### Prerequisites

Install MinGW-w64 and Qt6 for Windows:

```bash
# On Debian/Ubuntu
sudo apt-get update
sudo apt-get install mingw-w64 g++-mingw-w64-x86-64

# Download Qt6 for Windows (you'll need to get this from qt.io)
# Extract it to a location like ~/qt6-windows
```

### Building

```bash
# Set Qt6 path (adjust to your Qt6 installation)
export Qt6_DIR=~/qt6-windows/lib/cmake/Qt6

# Create build directory
mkdir build-windows
cd build-windows

# Configure with cross-compilation toolchain
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DQt6_DIR=$Qt6_DIR

# Build
cmake --build .
```

The executable will be in `build-windows/BandwidthThrottler.exe`

## Option 3: Use a Windows Virtual Machine

1. Install VirtualBox or similar
2. Install Windows in the VM
3. Build normally on Windows

## Option 4: Use Docker with Windows Container

If you have Docker with Windows containers support, you can build in a Windows container.

## Quick GitHub Actions Setup

If you want to use GitHub Actions:

1. Create a GitHub repository
2. Push your code:
   ```bash
   git init
   git add .
   git commit -m "Initial commit"
   git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO.git
   git push -u origin main
   ```
3. Go to Actions tab in GitHub
4. The workflow will run automatically
5. Download the `.exe` from the artifacts

