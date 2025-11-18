#include "ProcessMonitor.h"
#include "ProcessInfo.h"
#include <algorithm>
#include <vector>
#include <string>

ProcessMonitor::ProcessMonitor() {
    refresh();
}

ProcessMonitor::~ProcessMonitor() = default;

std::vector<ProcessInfo> ProcessMonitor::getRunningProcesses() {
    return processes_;
}

bool ProcessMonitor::refresh() {
    processes_.clear();
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(PROCESSENTRY32W);
    
    if (Process32FirstW(snapshot, &entry)) {
        do {
            ProcessInfo info;
            if (getProcessInfo(entry.th32ProcessID, info)) {
                processes_.push_back(info);
            }
        } while (Process32NextW(snapshot, &entry));
    }
    
    CloseHandle(snapshot);
    
    // Sort by PID
    std::sort(processes_.begin(), processes_.end(),
              [](const ProcessInfo& a, const ProcessInfo& b) {
                  return a.pid < b.pid;
              });
    
    return true;
}

bool ProcessMonitor::getProcessInfo(DWORD pid, ProcessInfo& info) {
    info.pid = pid;
    
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        // Try with less privileges
        hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProcess == NULL) {
            info.name = "unknown";
            info.path = "";
            return true; // Still add to list
        }
    }
    
    // Get process name
    WCHAR processName[MAX_PATH];
    DWORD size = MAX_PATH;
    if (QueryFullProcessImageNameW(hProcess, 0, processName, &size)) {
        // Extract filename from path
        std::wstring wpath(processName);
        size_t lastSlash = wpath.find_last_of(L"\\/");
        if (lastSlash != std::wstring::npos) {
            std::wstring wname = wpath.substr(lastSlash + 1);
            // Convert to narrow string
            int len = WideCharToMultiByte(CP_UTF8, 0, wname.c_str(), -1, NULL, 0, NULL, NULL);
            if (len > 0) {
                std::vector<char> buffer(len);
                WideCharToMultiByte(CP_UTF8, 0, wname.c_str(), -1, buffer.data(), len, NULL, NULL);
                info.name = buffer.data();
            }
        }
        
        // Get full path
        info.path = getProcessPath(pid);
    } else {
        info.name = "unknown";
        info.path = "";
    }
    
    CloseHandle(hProcess);
    return true;
}

std::string ProcessMonitor::getProcessPath(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProcess == NULL) {
            return "";
        }
    }
    
    WCHAR processPath[MAX_PATH];
    DWORD size = MAX_PATH;
    std::string result;
    
    if (QueryFullProcessImageNameW(hProcess, 0, processPath, &size)) {
        // Convert wide string to UTF-8
        int len = WideCharToMultiByte(CP_UTF8, 0, processPath, -1, NULL, 0, NULL, NULL);
        if (len > 0) {
            std::vector<char> buffer(len);
            WideCharToMultiByte(CP_UTF8, 0, processPath, -1, buffer.data(), len, NULL, NULL);
            result = buffer.data();
        }
    }
    
    CloseHandle(hProcess);
    return result;
}

bool ProcessMonitor::updateNetworkStats() {
    // TODO: Implement network stats tracking for Windows
    // Could use GetExtendedTcpTable/GetExtendedUdpTable and match by PID
    // For now, just return true
    return true;
}

