#include "../include/MainWindow.h"
#include "../include/components/TopBar.h"
#include "../include/components/LeftSidebar.h"
#include "../include/components/RightSidebar.h"
#include "../include/components/MapViewer.h"
#include <QDebug>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("UAV Control Interface");
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
    
    // Add toolbars
    addToolBar(Qt::TopToolBarArea, topBar);
    addToolBar(Qt::LeftToolBarArea, leftSidebar->getToolBar());
    addToolBar(Qt::RightToolBarArea, rightSidebar->getToolBar());
    
    // Setup main area with map first
    setupMainArea();
    
    // Add dock widgets on top of the map
    addDockWidget(Qt::LeftDockWidgetArea, leftSidebar->getDockWidget());
    addDockWidget(Qt::RightDockWidgetArea, rightSidebar->getDockWidget());
    
    // Make dock widgets float on top of the map
    leftSidebar->getDockWidget()->setFloating(false);
    rightSidebar->getDockWidget()->setFloating(false);
    
    // Apply global styles
    applyStyles();
    
    // Connect signals
    connect(leftSidebar, &LeftSidebar::panelChanged, this, &MainWindow::handleLeftPanelChanged);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupMainArea()
{
    qDebug() << "Setting up map in main area...";
    mapViewer = new MapViewer(this);
    mainLayout->addWidget(mapViewer);
}

void MainWindow::applyStyles()
{
    // Set modern dark theme with better styling
    QString styleSheet = R"(
        QMainWindow, QWidget {
            background-color: #1a1a1a;
            color: #f0f0f0;
        }
        
        QToolBar {
            background-color: #2d2d30;
            border: none;
            spacing: 0px;
            padding: 0px;
        }
        
        QToolButton {
            background-color: transparent;
            border: none;
            color: #f0f0f0;
            padding: 8px 0px;
            margin: 0px;
            font-size: 22px;
        }
        
        QToolButton:hover {
            background-color: #3e3e42;
        }
        
        QToolButton:pressed, QToolButton:checked {
            background-color: #0078d7;
        }
        
        QDockWidget {
            background-color: #252526;
            color: #f0f0f0;
            border: none;
            border-right: 1px solid #3e3e42;
            border-left: 1px solid #3e3e42;
        }
        
        QDockWidget::title {
            background-color: #252526;
            padding: 5px;
            border: none;
        }
        
        QLabel {
            color: #f0f0f0;
            font-size: 13px;
        }
        
        QLineEdit {
            background-color: #333337;
            color: #f0f0f0;
            border: 1px solid #3e3e42;
            border-radius: 4px;
            padding: 5px 10px;
            selection-background-color: #0078d7;
        }
        
        QLineEdit:focus {
            border: 1px solid #0078d7;
        }
        
        QComboBox {
            background-color: #333337;
            color: #f0f0f0;
            border: 1px solid #3e3e42;
            border-radius: 4px;
            padding: 5px 10px;
            min-height: 25px;
        }
        
        QComboBox:hover {
            border: 1px solid #0078d7;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        
        QPushButton {
            background-color: #0078d7;
            color: #ffffff;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #1c86e0;
        }
        
        QPushButton:pressed {
            background-color: #005fa3;
        }
        
        QScrollArea {
            border: none;
        }
        
        QTextEdit {
            background-color: #333337;
            color: #f0f0f0;
            border: 1px solid #3e3e42;
            border-radius: 4px;
            padding: 5px;
            selection-background-color: #0078d7;
        }
        
        QTextEdit:focus {
            border: 1px solid #0078d7;
        }
    )";
    
    QApplication::setStyle("Fusion");
    setStyleSheet(styleSheet);
}

void MainWindow::handleAssignTask()
{
    qDebug() << "Task assigned!";
    // Implementation would go here
}

void MainWindow::handleLeftPanelChanged(int index)
{
    qDebug() << "Left panel changed to index:" << index;
    // Implementation would go here
} 