# UAV Control Interface

A Qt-based user interface for UAV control with Mapbox integration.

## Project Structure

The project is organized into components for better maintainability:

```
.
├── include/                  # Header files
│   ├── MainWindow.h          # Main application window
│   └── components/           # UI components
│       ├── TopBar.h          # Top toolbar
│       ├── LeftSidebar.h     # Left sidebar with mission control
│       ├── RightSidebar.h    # Right sidebar with task details
│       ├── MapViewer.h       # 3D map component
│       └── VehicleInfoWidget.h # Vehicle information widget
│
├── src/                      # Source files
│   ├── main.cpp              # Application entry point
│   ├── MainWindow.cpp        # Main window implementation
│   └── components/           # Component implementations
│       ├── TopBar.cpp        # Top toolbar implementation
│       ├── LeftSidebar.cpp   # Left sidebar implementation
│       ├── RightSidebar.cpp  # Right sidebar implementation
│       ├── MapViewer.cpp     # Map viewer implementation
│       └── VehicleInfoWidget.cpp # Vehicle info widget implementation
│
├── CMakeLists.txt            # CMake build configuration
└── README.md                 # This file
```

## Building the Project

```bash
mkdir -p build
cd build
cmake ..
make
```

## Running the Application

```bash
cd build
./qt_simple_interface
```

## Features

- 3D map visualization with Mapbox
- Mission control panel
- Vehicle configuration
- Task management
- Dark theme UI 