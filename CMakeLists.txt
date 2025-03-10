cmake_minimum_required(VERSION 3.16)
project(qt_simple_interface)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets WebEngineWidgets Network Sql REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/components/TopBar.cpp
    src/components/LeftSidebar.cpp
    src/components/RightSidebar.cpp
    src/components/MapViewer.cpp
    src/components/VehicleInfoWidget.cpp
    src/database/DatabaseManager.cpp
    src/api/ChatGPTClient.cpp
    src/dialogs/ResponseDialog.cpp
    src/drone/DroneFunctions.cpp
    src/simulation/SimulationView.cpp
)

# Header files
set(HEADERS
    include/MainWindow.h
    include/components/TopBar.h
    include/components/LeftSidebar.h
    include/components/RightSidebar.h
    include/components/MapViewer.h
    include/components/VehicleInfoWidget.h
    include/database/DatabaseManager.h
    include/api/ChatGPTClient.h
    include/dialogs/ResponseDialog.h
    include/drone/DroneFunctions.h
    include/simulation/SimulationView.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::WebEngineWidgets
    Qt5::Network
    Qt5::Sql
) 