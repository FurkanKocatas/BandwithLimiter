#ifndef BANDWIDTHCONTROLLER_H
#define BANDWIDTHCONTROLLER_H

#include "ProcessInfo.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// Forward declarations
class ProcessMonitor;
class NetworkThrottler;

class BandwidthController {
public:
    BandwidthController();
    ~BandwidthController();
    
    // Process monitoring
    std::vector<ProcessInfo> getRunningProcesses();
    bool refreshProcessList();
    bool updateNetworkStats(); // Update network usage statistics
    
    // Bandwidth throttling
    bool startThrottling(uint32_t pid, uint64_t downloadLimitBytesPerSec, uint64_t uploadLimitBytesPerSec);
    bool stopThrottling(uint32_t pid);
    bool isThrottlingActive(uint32_t pid) const;
    
    // Utility
    static uint64_t parseBandwidthString(const std::string& bandwidthStr);
    static std::string formatBandwidth(uint64_t bytesPerSec);
    
private:
    std::unique_ptr<ProcessMonitor> processMonitor_;
    std::unique_ptr<NetworkThrottler> networkThrottler_;
};

#endif // BANDWIDTHCONTROLLER_H

