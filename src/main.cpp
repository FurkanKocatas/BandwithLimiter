#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Bandwidth Throttler");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("BandwidthThrottler");
    
    // Use native style if available
    app.setStyle(QStyleFactory::create("Fusion"));
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

