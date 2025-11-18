#ifndef WINDOWS_PROCESSMONITOR_H
#define WINDOWS_PROCESSMONITOR_H

#include "../../ProcessInfo.h"
#include <vector>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

class ProcessMonitor {
public:
    ProcessMonitor();
    ~ProcessMonitor();
    
    std::vector<ProcessInfo> getRunningProcesses();
    bool refresh();
    bool updateNetworkStats();

private:
    std::vector<ProcessInfo> processes_;
    bool getProcessInfo(DWORD pid, ProcessInfo& info);
    std::string getProcessPath(DWORD pid);
};

#endif // WINDOWS_PROCESSMONITOR_H

