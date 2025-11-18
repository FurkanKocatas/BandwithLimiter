# Bandwidth Throttler

A Windows C++ application that allows you to limit the internet bandwidth of specific processes (not the entire device). Built with Qt6 and Windows Filtering Platform (WFP).

## Features

- 🎯 **Per-Process Bandwidth Limiting** - Throttle individual applications, not your entire network
- 📊 **Real-Time Network Monitoring** - See download/upload speeds for all running processes
- 🔍 **Process Search** - Quickly find processes by name, PID, or path
- 🎚️ **Easy-to-Use Sliders** - Set bandwidth limits from 1-500 Mbps with precision checkpoints
- 🔄 **Auto-Refresh** - Process list and network stats update automatically
- 📈 **Sortable Columns** - Sort by download/upload speed to see which apps use the most bandwidth

## Screenshots

*Add screenshots here once you have the application running*

## Requirements

### For Building

- **Windows 10/11** (64-bit)
- **CMake 3.16+** - [Download](https://cmake.org/download/)
- **Qt6** (Core, Widgets modules) - [Download](https://www.qt.io/download)
- **Visual Studio 2019+** (with C++ support) OR **MinGW-w64**
- **Windows SDK** (usually included with Visual Studio)

### For Running

- Windows 10 or later
- Administrator privileges (required for network throttling)

## Installation

### Option 1: Build from Source (Recommended)

See [BUILD.md](BUILD.md) for detailed build instructions.

**Quick Start:**
```bash
# Clone the repository
git clone https://github.com/FurkanKocatas/BandwithLimiter.git
cd BandwithLimiter

# Create build directory
mkdir build
cd build

# Configure with Visual Studio
cmake .. -G "Visual Studio 17 2022" -A x64

# Build
cmake --build . --config Release

# Executable will be in: build/Release/BandwidthThrottler.exe
```

### Option 2: Download Pre-built Binary

*Add download link when available*

## Usage

1. **Run as Administrator**
   - Right-click `BandwidthThrottler.exe` → "Run as administrator"
   - Administrator privileges are required for network throttling

2. **Select a Process**
   - Browse the process list or use the search box
   - Click on a process to select it

3. **Set Bandwidth Limits**
   - Use the sliders to set download and upload limits (1-500 Mbps)
   - Sliders snap to checkpoints: 1, 5, 10, 25, 50, 75, 100, 250, 500 Mbps

4. **Start Throttling**
   - Click "Start Throttling" to apply the limits
   - The process will be limited to your specified speeds

5. **Monitor Network Usage**
   - View real-time download/upload speeds in the process table
   - Sort by speed to see which processes are using the most bandwidth

6. **Stop Throttling**
   - Click "Stop Throttling" to remove the limits

## Architecture

```
BandwidthThrottler/
├── src/
│   ├── main.cpp                 # Application entry point
│   ├── MainWindow.h/cpp         # Qt GUI implementation
│   ├── MainWindow.ui            # Qt Designer UI file
│   ├── BandwidthController.h/cpp # Main controller/abstraction layer
│   ├── ProcessInfo.h           # Process information structure
│   ├── NumericTableWidgetItem.h # Custom table item for numeric sorting
│   └── platform/
│       └── windows/
│           ├── ProcessMonitor.h/cpp    # Windows process enumeration
│           └── NetworkThrottler.h/cpp   # WFP-based bandwidth throttling
├── CMakeLists.txt              # CMake build configuration
└── README.md                   # This file
```

### Component Overview

- **MainWindow**: Qt-based GUI for user interaction
- **BandwidthController**: High-level interface for process monitoring and throttling
- **ProcessMonitor**: Windows-specific process enumeration and network statistics
- **NetworkThrottler**: Windows Filtering Platform (WFP) integration for bandwidth limiting

## Technical Details

### Network Throttling

The application uses **Windows Filtering Platform (WFP)** to implement per-process bandwidth limiting. WFP is a Windows API that allows filtering and modifying network traffic at the kernel level.

### Process Monitoring

Process enumeration uses Windows API functions:
- `CreateToolhelp32Snapshot` for process listing
- `GetExtendedTcpTable` / `GetExtendedUdpTable` for network statistics
- Socket inode matching to associate network connections with processes

### GUI Framework

Built with **Qt6** for a modern, native Windows interface:
- Qt Widgets for the main window
- Custom table widgets for process display
- Sliders with checkpoint snapping for bandwidth limits

## Troubleshooting

### "Permission Denied" Error

- **Solution**: Run the application as administrator
- Right-click the executable → "Run as administrator"

### "Failed to start throttling"

- **Possible causes**:
  - Not running as administrator
  - Windows Filtering Platform (WFP) not available
  - Process already terminated
- **Solution**: Ensure you're running as administrator and the process is still running

### Process Not Showing Network Activity

- Network statistics update every 3 seconds
- Processes with no active network connections will show 0 B/s
- Try refreshing the process list

### Build Errors

See [BUILD.md](BUILD.md) for detailed troubleshooting.

## Development

### Building from Source

See [BUILD.md](BUILD.md) for complete build instructions.

### Code Style

- C++17 standard
- Follow existing code style
- Use meaningful variable names
- Comment complex logic

### Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## License

MIT License - see [LICENSE](LICENSE) file for details

## Author

FurkanKocatas

## Acknowledgments

- Qt6 for the GUI framework
- Windows Filtering Platform (WFP) for network throttling capabilities
