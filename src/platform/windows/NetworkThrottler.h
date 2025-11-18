#ifndef WINDOWS_NETWORKTHROTTLER_H
#define WINDOWS_NETWORKTHROTTLER_H

#include <cstdint>
#include <map>
#include <mutex>
#include <windows.h>
#include <fwpmu.h>
#include <fwptypes.h>

class NetworkThrottler {
public:
    NetworkThrottler();
    ~NetworkThrottler();
    
    bool startThrottling(uint32_t pid, uint64_t downloadLimitBytesPerSec, uint64_t uploadLimitBytesPerSec);
    bool stopThrottling(uint32_t pid);
    bool isThrottlingActive(uint32_t pid) const;

private:
    struct ThrottleInfo {
        uint64_t downloadLimit;
        uint64_t uploadLimit;
        UINT64 filterId;
        bool active;
    };
    
    mutable std::mutex mutex_;
    std::map<uint32_t, ThrottleInfo> activeThrottles_;
    HANDLE engineHandle_;
    
    bool initializeWfp();
    void cleanupWfp();
    bool createFilter(uint32_t pid, uint64_t downloadLimit, uint64_t uploadLimit, UINT64& filterId);
    bool deleteFilter(UINT64 filterId);
    std::vector<uint64_t> getProcessSockets(uint32_t pid);
};

#endif // WINDOWS_NETWORKTHROTTLER_H

