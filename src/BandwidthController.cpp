#include "BandwidthController.h"
#include "ProcessInfo.h"
#include "platform/windows/ProcessMonitor.h"
#include "platform/windows/NetworkThrottler.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

BandwidthController::BandwidthController() {
    processMonitor_ = std::make_unique<ProcessMonitor>();
    networkThrottler_ = std::make_unique<NetworkThrottler>();
}

BandwidthController::~BandwidthController() = default;

std::vector<ProcessInfo> BandwidthController::getRunningProcesses() {
    if (processMonitor_) {
        return processMonitor_->getRunningProcesses();
    }
    return {};
}

bool BandwidthController::refreshProcessList() {
    if (processMonitor_) {
        return processMonitor_->refresh();
    }
    return false;
}

bool BandwidthController::updateNetworkStats() {
    if (processMonitor_) {
        return processMonitor_->updateNetworkStats();
    }
    return false;
}

bool BandwidthController::startThrottling(uint32_t pid, uint64_t downloadLimitBytesPerSec, uint64_t uploadLimitBytesPerSec) {
    if (networkThrottler_) {
        return networkThrottler_->startThrottling(pid, downloadLimitBytesPerSec, uploadLimitBytesPerSec);
    }
    return false;
}

bool BandwidthController::stopThrottling(uint32_t pid) {
    if (networkThrottler_) {
        return networkThrottler_->stopThrottling(pid);
    }
    return false;
}

bool BandwidthController::isThrottlingActive(uint32_t pid) const {
    if (networkThrottler_) {
        return networkThrottler_->isThrottlingActive(pid);
    }
    return false;
}

uint64_t BandwidthController::parseBandwidthString(const std::string& bandwidthStr) {
    if (bandwidthStr.empty()) return 0;
    
    std::string str = bandwidthStr;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    
    // Remove spaces
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    
    uint64_t value = 0;
    std::string unit;
    
    // Extract number and unit
    size_t i = 0;
    while (i < str.length() && (std::isdigit(str[i]) || str[i] == '.')) {
        i++;
    }
    
    if (i == 0) return 0;
    
    value = static_cast<uint64_t>(std::stod(str.substr(0, i)));
    if (i < str.length()) {
        unit = str.substr(i);
    }
    
    // Convert to bytes per second
    if (unit == "b" || unit == "bps" || unit.empty()) {
        return value;
    } else if (unit == "kb" || unit == "kbps" || unit == "k") {
        return value * 1024;
    } else if (unit == "mb" || unit == "mbps" || unit == "m") {
        return value * 1024 * 1024;
    } else if (unit == "gb" || unit == "gbps" || unit == "g") {
        return value * 1024ULL * 1024 * 1024;
    }
    
    return value;
}

std::string BandwidthController::formatBandwidth(uint64_t bytesPerSec) {
    std::ostringstream oss;
    
    if (bytesPerSec < 1024) {
        oss << bytesPerSec << " B/s";
    } else if (bytesPerSec < 1024 * 1024) {
        oss << std::fixed << std::setprecision(2) << (bytesPerSec / 1024.0) << " KB/s";
    } else if (bytesPerSec < 1024ULL * 1024 * 1024) {
        oss << std::fixed << std::setprecision(2) << (bytesPerSec / (1024.0 * 1024.0)) << " MB/s";
    } else {
        oss << std::fixed << std::setprecision(2) << (bytesPerSec / (1024.0 * 1024.0 * 1024.0)) << " GB/s";
    }
    
    return oss.str();
}

