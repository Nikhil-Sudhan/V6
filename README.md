# Aerial System

A Qt-based user interface for aerial vehicle control with Mapbox integration.

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
│       ├── MapViewer.h       # Map component
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

## Installation

### Prerequisites

- Qt 5.15 or higher
- CMake 3.10 or higher
- C++17 compatible compiler

### Dependencies

Install the required dependencies:

```bash
# For Ubuntu/Debian
sudo apt update
sudo apt install -y build-essential cmake qt5-default qtwebengine5-dev libqt5webenginewidgets5 libqt5webengine5 libqt5webenginecore5 libqt5webchannel5-dev

# For Fedora/RHEL
sudo dnf install -y cmake qt5-qtbase-devel qt5-qtwebengine-devel qt5-qtwebchannel-devel
```

### Building the Project

```bash
# Clone the repository (if you haven't already)
git clone https://github.com/yourusername/aerial-system.git
cd aerial-system

# Create build directory and compile
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

## Running the Application

```bash
cd build
./aerialsystem
```

## API Key

Add the OpenAI API key to the in the ChatGPTClient.cpp file. 


