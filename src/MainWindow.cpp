#include "MainWindow.h"
#include "BandwidthController.h"
#include "ProcessInfo.h"
#include "NumericTableWidgetItem.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>
#include <QTableWidgetItem>
#include <QList>
#include <QSlider>
#include <QLabel>
#include <QAbstractItemModel>
#include <cmath>
#ifdef _WIN32
#include <windows.h>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , controller_(std::make_unique<BandwidthController>())
    , refreshTimer_(new QTimer(this))
    , currentThrottledPid_(0)
{
    ui_.setupUi(this);
    setupUI();
    
    // Delay initial refresh to allow UI to render first
    QTimer::singleShot(100, this, &MainWindow::refreshProcessList);
    
    // Auto-refresh process list every 5 seconds
    connect(refreshTimer_, &QTimer::timeout, this, &MainWindow::refreshProcessList);
    refreshTimer_->start(5000);
}

MainWindow::~MainWindow() {
    if (currentThrottledPid_ != 0) {
        controller_->stopThrottling(currentThrottledPid_);
    }
}

void MainWindow::setupUI() {
    // Configure process table
    ui_.processTable->horizontalHeader()->setStretchLastSection(true);
    ui_.processTable->setAlternatingRowColors(true);
    ui_.processTable->setSortingEnabled(true);
    ui_.processTable->sortByColumn(2, Qt::DescendingOrder); // Sort by download speed by default
    
    // Set up custom sorting for network speed columns (columns 2 and 3)
    // We'll handle this by storing numeric values in UserRole
    
    // Connect signals
    connect(ui_.refreshButton, &QPushButton::clicked, this, &MainWindow::refreshProcessList);
    connect(ui_.startButton, &QPushButton::clicked, this, &MainWindow::startThrottling);
    connect(ui_.stopButton, &QPushButton::clicked, this, &MainWindow::stopThrottling);
    connect(ui_.processTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::onProcessSelected);
    connect(ui_.searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(ui_.clearSearchButton, &QPushButton::clicked, this, &MainWindow::clearSearch);
    
    // Connect slider signals
    connect(ui_.downloadSlider, &QSlider::valueChanged, this, &MainWindow::onDownloadSliderChanged);
    connect(ui_.uploadSlider, &QSlider::valueChanged, this, &MainWindow::onUploadSliderChanged);
    connect(ui_.downloadSlider, &QSlider::sliderPressed, this, &MainWindow::onSliderPressed);
    connect(ui_.downloadSlider, &QSlider::sliderReleased, this, &MainWindow::onSliderReleased);
    connect(ui_.uploadSlider, &QSlider::sliderPressed, this, &MainWindow::onSliderPressed);
    connect(ui_.uploadSlider, &QSlider::sliderReleased, this, &MainWindow::onSliderReleased);
    
    // Initialize slider values
    updateSliderValue(ui_.downloadSlider, ui_.downloadValueLabel, ui_.downloadSlider->value());
    updateSliderValue(ui_.uploadSlider, ui_.uploadValueLabel, ui_.uploadSlider->value());
    
    // Check if running as administrator
    bool isAdmin = false;
#ifdef _WIN32
    BOOL isElevated = FALSE;
    HANDLE token = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation;
        DWORD size;
        if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
            isElevated = elevation.TokenIsElevated;
        }
        CloseHandle(token);
    }
    isAdmin = (isElevated == TRUE);
#endif
    if (!isAdmin) {
        ui_.statusLabel->setText("Warning: Not running as administrator. Network throttling requires administrator privileges.");
        ui_.statusLabel->setStyleSheet("padding: 5px; background-color: #fff3cd; border: 1px solid #ffc107; color: #856404;");
    }
    
    // Network stats update timer (update every 3 seconds, delayed start)
    // Delay to avoid blocking UI on startup
    QTimer* networkStatsTimer = new QTimer(this);
    connect(networkStatsTimer, &QTimer::timeout, this, &MainWindow::updateNetworkStats);
    networkStatsTimer->setSingleShot(false);
    // Delay first network stats update by 2 seconds to let UI render
    QTimer::singleShot(2000, this, [networkStatsTimer]() { 
        if (networkStatsTimer) networkStatsTimer->start(3000); 
    });
    
    // Status update timer
    QTimer* statusTimer = new QTimer(this);
    connect(statusTimer, &QTimer::timeout, this, &MainWindow::updateStatus);
    statusTimer->start(1000);
}

void MainWindow::refreshProcessList() {
    if (controller_) {
        controller_->refreshProcessList();
        allProcesses_ = controller_->getRunningProcesses();
        updateProcessTable();
    }
}

void MainWindow::updateProcessTable() {
    if (!controller_) return;
    
    // Get search text and filter processes
    QString searchText = ui_.searchEdit->text();
    auto filteredProcesses = filterProcesses(allProcesses_, searchText);
    
    // Store current sort column and order before disabling sorting
    int sortColumn = ui_.processTable->horizontalHeader()->sortIndicatorSection();
    Qt::SortOrder sortOrder = ui_.processTable->horizontalHeader()->sortIndicatorOrder();
    if (sortColumn < 0) sortColumn = 2; // Default to download speed column
    
    // Disable sorting while updating to avoid issues
    ui_.processTable->setSortingEnabled(false);
    ui_.processTable->setRowCount(filteredProcesses.size());
    
    for (size_t i = 0; i < filteredProcesses.size(); ++i) {
        const auto& proc = filteredProcesses[i];
        
        // PID
        QTableWidgetItem* pidItem = new QTableWidgetItem(QString::number(proc.pid));
        pidItem->setData(Qt::UserRole, proc.pid); // Store for sorting
        ui_.processTable->setItem(i, 0, pidItem);
        
        // Process Name
        ui_.processTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(proc.name)));
        
        // Download Speed (formatted) - use custom item for proper numeric sorting
        QString downloadStr = QString::fromStdString(BandwidthController::formatBandwidth(proc.downloadSpeed));
        NumericTableWidgetItem* downloadItem = new NumericTableWidgetItem(downloadStr, proc.downloadSpeed);
        downloadItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui_.processTable->setItem(i, 2, downloadItem);
        
        // Upload Speed (formatted) - use custom item for proper numeric sorting
        QString uploadStr = QString::fromStdString(BandwidthController::formatBandwidth(proc.uploadSpeed));
        NumericTableWidgetItem* uploadItem = new NumericTableWidgetItem(uploadStr, proc.uploadSpeed);
        uploadItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui_.processTable->setItem(i, 3, uploadItem);
        
        // Path
        ui_.processTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(proc.path)));
    }
    
    // Re-enable sorting and restore sort order
    ui_.processTable->setSortingEnabled(true);
    ui_.processTable->sortByColumn(sortColumn, sortOrder);
    ui_.processTable->resizeColumnsToContents();
}

std::vector<ProcessInfo> MainWindow::filterProcesses(const std::vector<ProcessInfo>& processes, const QString& searchText) const {
    if (searchText.isEmpty()) {
        return processes;
    }
    
    std::vector<ProcessInfo> filtered;
    QString searchLower = searchText.toLower();
    
    for (const auto& proc : processes) {
        QString name = QString::fromStdString(proc.name).toLower();
        QString path = QString::fromStdString(proc.path).toLower();
        QString pid = QString::number(proc.pid);
        
        // Check if search text matches process name, path, or PID
        if (name.contains(searchLower) || path.contains(searchLower) || pid.contains(searchLower)) {
            filtered.push_back(proc);
        }
    }
    
    return filtered;
}

void MainWindow::onSearchTextChanged() {
    updateProcessTable();
}

void MainWindow::clearSearch() {
    ui_.searchEdit->clear();
    updateProcessTable();
}

int MainWindow::snapToCheckpoint(int value) const {
    // Find the nearest checkpoint
    int nearestCheckpoint = CHECKPOINTS[0];
    int minDistance = std::abs(value - CHECKPOINTS[0]);
    
    for (int i = 1; i < CHECKPOINT_COUNT; ++i) {
        int distance = std::abs(value - CHECKPOINTS[i]);
        if (distance < minDistance) {
            minDistance = distance;
            nearestCheckpoint = CHECKPOINTS[i];
        }
    }
    
    // Snap if within threshold
    if (minDistance <= SNAP_THRESHOLD) {
        return nearestCheckpoint;
    }
    
    return value;
}

void MainWindow::updateSliderValue(QSlider* slider, QLabel* label, int value) {
    if (label) {
        label->setText(QString("%1 Mbps").arg(value));
    }
}

void MainWindow::onDownloadSliderChanged(int value) {
    // Check if we're near a checkpoint and snap if close enough (for better UX while dragging)
    int snappedValue = snapToCheckpoint(value);
    if (snappedValue != value && std::abs(snappedValue - value) <= SNAP_THRESHOLD) {
        // Temporarily block signals to avoid recursion
        ui_.downloadSlider->blockSignals(true);
        ui_.downloadSlider->setValue(snappedValue);
        ui_.downloadSlider->blockSignals(false);
        updateSliderValue(ui_.downloadSlider, ui_.downloadValueLabel, snappedValue);
    } else {
        updateSliderValue(ui_.downloadSlider, ui_.downloadValueLabel, value);
    }
}

void MainWindow::onUploadSliderChanged(int value) {
    // Check if we're near a checkpoint and snap if close enough (for better UX while dragging)
    int snappedValue = snapToCheckpoint(value);
    if (snappedValue != value && std::abs(snappedValue - value) <= SNAP_THRESHOLD) {
        // Temporarily block signals to avoid recursion
        ui_.uploadSlider->blockSignals(true);
        ui_.uploadSlider->setValue(snappedValue);
        ui_.uploadSlider->blockSignals(false);
        updateSliderValue(ui_.uploadSlider, ui_.uploadValueLabel, snappedValue);
    } else {
        updateSliderValue(ui_.uploadSlider, ui_.uploadValueLabel, value);
    }
}

void MainWindow::onSliderPressed() {
    // When slider is pressed, we'll handle snapping on release
}

void MainWindow::onSliderReleased() {
    // Snap to nearest checkpoint when released
    QSlider* slider = qobject_cast<QSlider*>(sender());
    if (!slider) return;
    
    int currentValue = slider->value();
    int snappedValue = snapToCheckpoint(currentValue);
    
    if (snappedValue != currentValue) {
        slider->blockSignals(true);
        slider->setValue(snappedValue);
        slider->blockSignals(false);
        
        // Update the label
        if (slider == ui_.downloadSlider) {
            updateSliderValue(ui_.downloadSlider, ui_.downloadValueLabel, snappedValue);
        } else if (slider == ui_.uploadSlider) {
            updateSliderValue(ui_.uploadSlider, ui_.uploadValueLabel, snappedValue);
        }
    }
}

void MainWindow::onProcessSelected() {
    uint32_t pid = getSelectedPid();
    if (pid != 0) {
        ui_.statusLabel->setText(QString("Selected process: PID %1").arg(pid));
        bool isAdmin = false;
#ifdef _WIN32
        BOOL isElevated = FALSE;
        HANDLE token = NULL;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            TOKEN_ELEVATION elevation;
            DWORD size;
            if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
                isElevated = elevation.TokenIsElevated;
            }
            CloseHandle(token);
        }
        isAdmin = (isElevated == TRUE);
#endif
        if (isAdmin) {
            ui_.statusLabel->setStyleSheet("padding: 5px; background-color: #f0f0f0; border: 1px solid #ccc;");
        }
    }
}

uint32_t MainWindow::getSelectedPid() const {
    QList<QTableWidgetItem*> selected = ui_.processTable->selectedItems();
    if (!selected.isEmpty()) {
        QTableWidgetItem* pidItem = ui_.processTable->item(selected[0]->row(), 0);
        if (pidItem) {
            bool ok;
            uint32_t pid = pidItem->text().toUInt(&ok);
            return ok ? pid : 0;
        }
    }
    return 0;
}

void MainWindow::updateNetworkStats() {
    if (controller_ && !allProcesses_.empty()) {
        // Only update network stats, don't refresh the entire process list
        // This is faster and less blocking
        controller_->updateNetworkStats();
        allProcesses_ = controller_->getRunningProcesses();
        
        // Update table without full refresh (preserves selection and scroll position)
        updateProcessTable();
    }
}

void MainWindow::startThrottling() {
    uint32_t pid = getSelectedPid();
    if (pid == 0) {
        QMessageBox::warning(this, "No Process Selected", "Please select a process from the list.");
        return;
    }
    
    // Check if running as administrator
    bool isAdmin = false;
#ifdef _WIN32
    BOOL isElevated = FALSE;
    HANDLE token = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation;
        DWORD size;
        if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
            isElevated = elevation.TokenIsElevated;
        }
        CloseHandle(token);
    }
    isAdmin = (isElevated == TRUE);
#endif
    if (!isAdmin) {
        QMessageBox::critical(this, "Permission Denied", 
            "Network throttling requires administrator privileges.\n\n"
            "Please run the application as administrator (right-click and select 'Run as administrator').");
        return;
    }
    
    // Get values from sliders (in Mbps, convert to bytes per second)
    int downloadMbps = ui_.downloadSlider->value();
    int uploadMbps = ui_.uploadSlider->value();
    
    // Convert Mbps to bytes per second (1 Mbps = 1,000,000 bits/s = 125,000 bytes/s)
    uint64_t downloadLimit = static_cast<uint64_t>(downloadMbps) * 125000ULL;
    uint64_t uploadLimit = static_cast<uint64_t>(uploadMbps) * 125000ULL;
    
    // Stop previous throttling if any
    if (currentThrottledPid_ != 0 && currentThrottledPid_ != pid) {
        controller_->stopThrottling(currentThrottledPid_);
    }
    
    if (controller_->startThrottling(pid, downloadLimit, uploadLimit)) {
        currentThrottledPid_ = pid;
        ui_.startButton->setEnabled(false);
        ui_.stopButton->setEnabled(true);
        ui_.statusLabel->setText(QString("Throttling active for PID %1 - Down: %2 Mbps, Up: %3 Mbps")
            .arg(pid).arg(downloadMbps).arg(uploadMbps));
        ui_.statusLabel->setStyleSheet("padding: 5px; background-color: #d4edda; border: 1px solid #c3e6cb; color: #155724;");
    } else {
        QMessageBox::critical(this, "Error", 
            "Failed to start throttling.\n\n"
            "Possible causes:\n"
            "- Windows Filtering Platform (WFP) not available\n"
            "- Insufficient permissions\n"
            "- Process not found or already terminated\n\n"
            "Make sure you're running as administrator.");
    }
}

void MainWindow::stopThrottling() {
    if (currentThrottledPid_ != 0) {
        if (controller_->stopThrottling(currentThrottledPid_)) {
            ui_.statusLabel->setText("Throttling stopped.");
            currentThrottledPid_ = 0;
            ui_.startButton->setEnabled(true);
            ui_.stopButton->setEnabled(false);
        } else {
            QMessageBox::warning(this, "Error", "Failed to stop throttling.");
        }
    }
}

void MainWindow::updateStatus() {
    if (currentThrottledPid_ != 0) {
        if (controller_->isThrottlingActive(currentThrottledPid_)) {
            int downloadMbps = ui_.downloadSlider->value();
            int uploadMbps = ui_.uploadSlider->value();
            ui_.statusLabel->setText(QString("Throttling active for PID %1 - Down: %2 Mbps, Up: %3 Mbps")
                .arg(currentThrottledPid_).arg(downloadMbps).arg(uploadMbps));
            ui_.statusLabel->setStyleSheet("padding: 5px; background-color: #d4edda; border: 1px solid #c3e6cb; color: #155724;");
        } else {
            ui_.statusLabel->setText("Throttling stopped.");
            ui_.statusLabel->setStyleSheet("padding: 5px; background-color: #f0f0f0; border: 1px solid #ccc;");
            currentThrottledPid_ = 0;
            ui_.startButton->setEnabled(true);
            ui_.stopButton->setEnabled(false);
        }
    } else {
        // Check if running as administrator
        bool isAdmin = false;
#ifdef _WIN32
        BOOL isElevated = FALSE;
        HANDLE token = NULL;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            TOKEN_ELEVATION elevation;
            DWORD size;
            if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
                isElevated = elevation.TokenIsElevated;
            }
            CloseHandle(token);
        }
        isAdmin = (isElevated == TRUE);
#endif
        if (!isAdmin) {
            ui_.statusLabel->setText("Warning: Not running as administrator. Network throttling requires administrator privileges.");
            ui_.statusLabel->setStyleSheet("padding: 5px; background-color: #fff3cd; border: 1px solid #ffc107; color: #856404;");
        }
    }
}

