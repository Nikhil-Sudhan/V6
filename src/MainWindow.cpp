#include "../include/MainWindow.h"
#include "../include/components/TopBar.h"
#include "../include/components/LeftSidebar.h"
#include "../include/components/RightSidebar.h"
#include "../include/components/MapViewer.h"
#include "../include/database/DatabaseManager.h"
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include <QDockWidget>
#include <QStatusBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    
    
    // Set window properties
    setWindowTitle("Aerial System");
    showMaximized();
    
    // Create central widget
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Create main layout
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create components
    topBar = new TopBar(this);
    leftSidebar = new LeftSidebar(this);
    rightSidebar = new RightSidebar(this);
    mapViewer = new MapViewer(this);
    
    // Set up main area
    setupMainArea();
    
    // Apply styles
    applyStyles();
    
    // Connect signals
    connect(leftSidebar, &LeftSidebar::panelChanged, this, &MainWindow::handleLeftPanelChanged);
    
    // Connect mission assigned signal to the drone task confirmation dialog
    connect(leftSidebar, &LeftSidebar::missionAssigned, mapViewer, &MapViewer::confirmDroneTask);
    
    // Connect drone animation completed signal to handle completion
    connect(mapViewer, &MapViewer::droneAnimationCompleted, this, &MainWindow::handleDroneAnimationCompleted);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupMainArea()
{
    // Add top bar at the top of the window
    addToolBar(Qt::TopToolBarArea, topBar);
    
    // Add left toolbar
    addToolBar(Qt::LeftToolBarArea, leftSidebar->getToolBar());
    
    // Add right toolbar
    addToolBar(Qt::RightToolBarArea, rightSidebar->getToolBar());
    
    // Add left dock widget
    addDockWidget(Qt::LeftDockWidgetArea, leftSidebar->getDockWidget());
    
    // Add right dock widget
    addDockWidget(Qt::RightDockWidgetArea, rightSidebar->getDockWidget());
    
    // Add map viewer to main layout
    mainLayout->addWidget(mapViewer);
}

void MainWindow::applyStyles()
{
    // Set application style
    setStyleSheet(R"(
        QMainWindow {
            background-color: #121212;
            color: #e0e0e0;
        }
        QDockWidget {
            background-color: #1a1a1a;
            color: #e0e0e0;
            border: 1px solid #00a6ff;
        }
        QDockWidget::title {
            background-color: #1a1a1a;
            padding: 8px;
            border-bottom: 1px solid #00a6ff;
            color: #e0e0e0;
        }
        QToolBar {
            background-color: #1a1a1a;
            border: 1px solid #00a6ff;
            spacing: 0;
        }
        QStatusBar {
            background-color: #1a1a1a;
            color: #e0e0e0;
            border-top: 1px solid #00a6ff;
        }
        QScrollArea {
            background-color: #1a1a1a;
            border: 1px solid #00a6ff;
        }
        QWidget#centralWidget {
            background-color: #121212;
        }
        QWidget#topBar {
            background-color: #1a1a1a;
            border-bottom: 1px solid #00a6ff;
            min-height: 50px;
        }
    )");
}

void MainWindow::handleLeftPanelChanged(int index)
{
    // Update title based on selected panel
    switch (index) {
        case 0:
            leftSidebar->setTitle("Mission Control");
            break;
        case 1:
            leftSidebar->setTitle("Vehicle Configuration");
            break;
        case 2:
            leftSidebar->setTitle("Simulation");
            break;
        case 3:
            leftSidebar->setTitle("Settings");
            break;
        default:
            leftSidebar->setTitle("");
            break;
    }
}

void MainWindow::handleAssignTask()
{
    // For now, just update the map or status
    statusBar()->showMessage("Task assigned successfully", 3000);
}

void MainWindow::handleDroneAnimationCompleted()
{
    // TO DO: implement handling for drone animation completion
}