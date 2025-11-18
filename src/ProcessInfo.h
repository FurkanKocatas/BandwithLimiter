#ifndef PROCESSINFO_H
#define PROCESSINFO_H

#include <string>
#include <cstdint>

struct ProcessInfo {
    uint32_t pid;
    std::string name;
    std::string path;
    
    // Network usage (bytes per second)
    uint64_t downloadSpeed;  // bytes/sec
    uint64_t uploadSpeed;     // bytes/sec
    uint64_t totalDownloaded; // total bytes downloaded
    uint64_t totalUploaded;   // total bytes uploaded
    
    ProcessInfo(uint32_t p = 0, const std::string& n = "", const std::string& pa = "")
        : pid(p), name(n), path(pa), downloadSpeed(0), uploadSpeed(0), 
          totalDownloaded(0), totalUploaded(0) {}
};

#endif // PROCESSINFO_H

