# Bandwidth Throttler

A Windows C++ application that allows you to limit the internet bandwidth of specific processes (not the entire device).

## Features

- Windows-only application
- Qt-based GUI for easy process selection
- Per-process bandwidth limiting
- Real-time bandwidth monitoring
- Process list auto-refresh

## Requirements

- Windows 10 or later
- CMake 3.16 or higher
- Qt6 (Core, Widgets)
- C++17 compatible compiler (Visual Studio 2019 or later recommended)
- Windows SDK
- Windows Filtering Platform (WFP) support (built into Windows)

## Building

### Windows
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

Or using MinGW:
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

## Usage

- The application requires **administrator privileges** to throttle network traffic
- Run the executable as administrator (right-click → "Run as administrator")

1. Launch the application (as administrator)
2. Select a process from the list
3. Set download and upload bandwidth limits using the sliders (1-500 Mbps)
4. Click "Start Throttling" to apply the limits
5. Click "Stop Throttling" to remove the limits

## Architecture

The application uses a layered architecture:

- **MainWindow**: Qt GUI layer for user interaction
- **BandwidthController**: Abstraction layer for process monitoring and throttling
- **ProcessMonitor**: Windows-specific process enumeration and network statistics
- **NetworkThrottler**: Windows Filtering Platform (WFP) integration for bandwidth limiting

### Platform-Specific Implementation

- **Windows**: Uses Windows Filtering Platform (WFP) API for per-process network throttling

## Implementation Notes

- ✅ Windows process enumeration using Windows API
- ✅ Network statistics tracking using Windows networking APIs
- ✅ Bandwidth throttling using Windows Filtering Platform (WFP)
- ✅ Qt6 for cross-platform GUI (Windows-only in this build)

## Future Enhancements

- Process search/filter functionality
- Bandwidth limit presets
- Historical bandwidth usage graphs
- Export bandwidth statistics
