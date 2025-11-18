cmake_minimum_required(VERSION 3.16)
project(BandwidthThrottler VERSION 1.0.0 LANGUAGES CXX)

# Windows-only application
if(NOT WIN32)
    message(FATAL_ERROR "This application only supports Windows. Please build on a Windows machine.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable Qt MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Windows-only platform sources
set(PLATFORM_SOURCES
    src/platform/windows/ProcessMonitor.cpp
    src/platform/windows/NetworkThrottler.cpp
)

set(PLATFORM_HEADERS
    src/platform/windows/ProcessMonitor.h
    src/platform/windows/NetworkThrottler.h
)

# Common sources
set(COMMON_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/BandwidthController.cpp
)

set(COMMON_HEADERS
    src/MainWindow.h
    src/ProcessInfo.h
    src/BandwidthController.h
    src/NumericTableWidgetItem.h
)

# UI files
set(UI_FILES
    src/MainWindow.ui
)

# Create executable
add_executable(${PROJECT_NAME}
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
    ${PLATFORM_SOURCES}
    ${PLATFORM_HEADERS}
    ${UI_FILES}
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
)

# Windows-specific libraries
target_link_libraries(${PROJECT_NAME} iphlpapi ws2_32)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

