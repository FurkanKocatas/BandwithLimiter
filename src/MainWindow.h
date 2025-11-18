#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <memory>
#include <vector>
#include "ui_MainWindow.h"
#include "ProcessInfo.h"

class BandwidthController;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refreshProcessList();
    void onProcessSelected();
    void startThrottling();
    void stopThrottling();
    void updateStatus();
    void onSearchTextChanged();
    void clearSearch();
    void onDownloadSliderChanged(int value);
    void onUploadSliderChanged(int value);
    void onSliderPressed();
    void onSliderReleased();
    void updateNetworkStats();

private:
    void setupUI();
    void updateProcessTable();
    uint32_t getSelectedPid() const;
    std::vector<ProcessInfo> filterProcesses(const std::vector<ProcessInfo>& processes, const QString& searchText) const;
    int snapToCheckpoint(int value) const;
    void updateSliderValue(QSlider* slider, QLabel* label, int value);
    
    Ui::MainWindow ui_;
    std::unique_ptr<BandwidthController> controller_;
    QTimer* refreshTimer_;
    uint32_t currentThrottledPid_;
    std::vector<ProcessInfo> allProcesses_;
    static constexpr int CHECKPOINTS[] = {1, 5, 10, 25, 50, 75, 100, 250, 500};
    static constexpr int CHECKPOINT_COUNT = 9;
    static constexpr int SNAP_THRESHOLD = 5; // units threshold for snapping (increased for better usability)
};

#endif // MAINWINDOW_H

