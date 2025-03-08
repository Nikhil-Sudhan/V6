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
    // Initialize database
    if (!DatabaseManager::instance().initialize()) {
        QMessageBox::warning(this, "Database Error", "Failed to initialize the database. Some features may not work correctly.");
    }
    
    // Set window properties
    setWindowTitle("UAV Control System");
    resize(1280, 800);
    
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
    
    // Add top bar to main layout
    mainLayout->addWidget(topBar);
    
    // Set up main area
    setupMainArea();
    
    // Apply styles
    applyStyles();
    
    // Connect signals
    connect(leftSidebar, &LeftSidebar::panelChanged, this, &MainWindow::handleLeftPanelChanged);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupMainArea()
{
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
            background-color: #1e1e1e;
            color: #d4d4d4;
        }
        QDockWidget {
            background-color: #252526;
            color: #d4d4d4;
            border: none;
        }
        QDockWidget::title {
            background-color: #333333;
            padding: 5px;
            color: #d4d4d4;
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
    // This will be called when a task is assigned
    // For now, just update the map or status
    statusBar()->showMessage("Task assigned successfully", 3000);
} 