#!/bin/bash
# Script to build Windows executable using GitHub Actions or provide instructions

echo "=========================================="
echo "Windows Executable Builder"
echo "=========================================="
echo ""
echo "This script helps you build a Windows .exe file."
echo ""

# Check if we're on GitHub Actions
if [ -n "$GITHUB_ACTIONS" ]; then
    echo "Detected GitHub Actions environment. Building..."
    mkdir -p build
    cd build
    cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release
    echo "Build complete! Executable: build/Release/BandwidthThrottler.exe"
    exit 0
fi

echo "You're on Linux. Here are your options:"
echo ""
echo "1. Use GitHub Actions (Recommended):"
echo "   - Push code to GitHub"
echo "   - GitHub will automatically build the .exe"
echo "   - Download from Actions tab"
echo ""
echo "2. Cross-compile with MinGW (Complex):"
echo "   - Install: sudo apt-get install mingw-w64"
echo "   - Install Qt6 for Windows"
echo "   - Run: cmake -DCMAKE_TOOLCHAIN_FILE=toolchain-mingw.cmake .."
echo ""
echo "3. Use Windows VM or remote Windows machine"
echo ""
echo "Would you like to set up GitHub Actions? (y/n)"
read -r response
if [[ "$response" =~ ^([yY][eE][sS]|[yY])$ ]]; then
    echo ""
    echo "To set up GitHub Actions:"
    echo "1. Create a GitHub repository"
    echo "2. Push this code:"
    echo "   git init"
    echo "   git add ."
    echo "   git commit -m 'Initial commit'"
    echo "   git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO.git"
    echo "   git push -u origin main"
    echo "3. Go to Actions tab and download the .exe"
    echo ""
    echo "The workflow file is already created at: .github/workflows/build-windows.yml"
fi

