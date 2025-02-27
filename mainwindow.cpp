#include "mainwindow.h"
#include <QMenu>
#include <QApplication>
#include <QScreen>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentPanelIndex(-1)
{
    // Set up the main window to be fullscreen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    resize(screenGeometry.width(), screenGeometry.height());
    setWindowState(Qt::WindowMaximized);
    setWindowTitle("Dark Theme Editor");

    // Create UI components
    createMenuBar();
    createSideToolBar();
    createSidePanels();
    setupMainArea();
    createStatusBar();
    applyStyles();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // File menu
    QMenu *fileMenu = menuBar->addMenu("File");
    fileMenu->addAction("New");
    fileMenu->addAction("Open");
    fileMenu->addAction("Save");
    fileMenu->addSeparator();
    fileMenu->addAction("Exit");

    // Edit menu
    QMenu *editMenu = menuBar->addMenu("Edit");
    editMenu->addAction("Undo");
    editMenu->addAction("Redo");
    editMenu->addSeparator();
    editMenu->addAction("Cut");
    editMenu->addAction("Copy");
    editMenu->addAction("Paste");

    // View menu
    QMenu *viewMenu = menuBar->addMenu("View");
    viewMenu->addAction("Toggle Sidebar");
}

void MainWindow::createSideToolBar()
{
    sideToolBar = new QToolBar(this);
    sideToolBar->setMovable(false);
    sideToolBar->setOrientation(Qt::Vertical);
    addToolBar(Qt::LeftToolBarArea, sideToolBar);

    // Create actions with temporary text instead of icons
    missionAction = new QAction("M", this);
    configAction = new QAction("C", this);
    simulationAction = new QAction("S", this);
    settingsAction = new QAction("⚙", this);

    // Set fixed size for the toolbar buttons
    sideToolBar->setIconSize(QSize(40, 40));
    
    // Add actions to toolbar
    sideToolBar->addAction(missionAction);
    sideToolBar->addAction(configAction);
    sideToolBar->addAction(simulationAction);
    sideToolBar->addAction(settingsAction);

    // Connect signals
    connect(missionAction, &QAction::triggered, this, [this]() { handleSidebarButton(0); });
    connect(configAction, &QAction::triggered, this, [this]() { handleSidebarButton(1); });
    connect(simulationAction, &QAction::triggered, this, [this]() { handleSidebarButton(2); });
    connect(settingsAction, &QAction::triggered, this, [this]() { handleSidebarButton(3); });
}

void MainWindow::createSidePanels()
{
    sidePanelDock = new QDockWidget(this);
    sidePanelDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    sidePanelDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, sidePanelDock);
    
    stackedWidget = new QStackedWidget(sidePanelDock);
    
    // Create panels with labels
    QWidget* missionPanel = new QWidget();
    QLabel* missionLabel = new QLabel("Mission Panel");
    QVBoxLayout* missionLayout = new QVBoxLayout(missionPanel);
    missionLayout->addWidget(missionLabel);
    
    QWidget* configPanel = new QWidget();
    QLabel* configLabel = new QLabel("Configuration Panel");
    QVBoxLayout* configLayout = new QVBoxLayout(configPanel);
    configLayout->addWidget(configLabel);
    
    QWidget* simulationPanel = new QWidget();
    QLabel* simulationLabel = new QLabel("Simulation Panel");
    QVBoxLayout* simulationLayout = new QVBoxLayout(simulationPanel);
    simulationLayout->addWidget(simulationLabel);
    
    QWidget* settingsPanel = new QWidget();
    QLabel* settingsLabel = new QLabel("Settings Panel");
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsPanel);
    settingsLayout->addWidget(settingsLabel);
    
    stackedWidget->addWidget(missionPanel);
    stackedWidget->addWidget(configPanel);
    stackedWidget->addWidget(simulationPanel);
    stackedWidget->addWidget(settingsPanel);
    
    sidePanelDock->setWidget(stackedWidget);
    sidePanelDock->hide();
}

void MainWindow::handleSidebarButton(int index)
{
    if (currentPanelIndex == index && sidePanelDock->isVisible()) {
        sidePanelDock->hide();
        currentPanelIndex = -1;
    } else {
        stackedWidget->setCurrentIndex(index);
        sidePanelDock->show();
        currentPanelIndex = index;
    }
}

void MainWindow::setupMainArea()
{
    centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    editor = new QTextEdit();
    editor->setPlaceholderText("Type your code here...");
    
    // Set font size for the editor
    QFont font = editor->font();
    font.setPointSize(12);
    editor->setFont(font);
    
    mainLayout->addWidget(editor);
}

void MainWindow::createStatusBar()
{
    bottomBar = new QStatusBar();
    setStatusBar(bottomBar);
    bottomBar->showMessage("Ready");
}

void MainWindow::applyStyles()
{
    // Dark theme styling
    QString styleSheet = R"(
        QMainWindow {
            background-color: #1e1e1e;
            color: #d4d4d4;
        }
        QMenuBar {
            background-color: #333333;
            color: #d4d4d4;
            font-size: 14px;
        }
        QMenuBar::item:selected {
            background-color: #505050;
        }
        QMenu {
            background-color: #252526;
            color: #d4d4d4;
            border: 1px solid #454545;
            font-size: 14px;
        }
        QMenu::item:selected {
            background-color: #04395e;
        }
        QToolBar {
            background-color: #333333;
            border: none;
            spacing: 10px;
            padding: 5px;
        }
        QToolBar QToolButton {
            background-color: #333333;
            color: #d4d4d4;
            border: none;
            border-radius: 5px;
            padding: 5px;
            font-size: 20px;
        }
        QToolBar QToolButton:hover {
            background-color: #404040;
        }
        QToolBar QToolButton:pressed {
            background-color: #505050;
        }
        QDockWidget {
            color: #d4d4d4;
            font-size: 14px;
        }
        QLabel {
            color: #d4d4d4;
            font-size: 14px;
        }
        QTextEdit {
            background-color: #1e1e1e;
            color: #d4d4d4;
            border: none;
            selection-background-color: #264f78;
            font-size: 12pt;
        }
        QStatusBar {
            background-color: #007acc;
            color: #ffffff;
            font-size: 14px;
        }
    )";

    qApp->setStyleSheet(styleSheet);
} 