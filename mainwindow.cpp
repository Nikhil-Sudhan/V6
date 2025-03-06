#include "mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QFont>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEnginePage>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentLeftPanelIndex(-1), isRightPanelVisible(false)
{
    // Set up the main window to be fullscreen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    resize(screenGeometry.width(), screenGeometry.height());
    setWindowState(Qt::WindowMaximized);
    setWindowTitle("UAV Aerial Systems");

    // Create UI components
    createTopBar();
    createLeftSideBar();
    createRightSideBar();
    createLeftPanels();
    createRightPanel();
    setupMainArea();
    applyStyles();

    // Initialize and start the date/time timer
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &MainWindow::updateDateTime);
    dateTimeTimer->start(1000); // Update every second
    updateDateTime(); // Initial update
}

MainWindow::~MainWindow()
{
}

void MainWindow::createTopBar()
{
    topBar = new QToolBar(this);
    topBar->setMovable(false);
    addToolBar(Qt::TopToolBarArea, topBar);

    // Left: Logo
    logoLabel = new QLabel("UAV LOGO");
    logoLabel->setMinimumWidth(100);
    topBar->addWidget(logoLabel);

    // Add spacer
    QWidget* spacer1 = new QWidget();
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    topBar->addWidget(spacer1);

    // Center: Search
    searchBox = new QLineEdit();
    searchBox->setPlaceholderText("Search...");
    searchBox->setFixedWidth(300);
    topBar->addWidget(searchBox);

    // Add spacer
    QWidget* spacer2 = new QWidget();
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    topBar->addWidget(spacer2);

    // Right: User info, date/time, status
    userIcon = new QLabel("👤");
    dateTimeLabel = new QLabel();
    statusLabel = new QLabel("● Online");
    
    topBar->addWidget(userIcon);
    topBar->addWidget(dateTimeLabel);
    topBar->addWidget(statusLabel);
}

void MainWindow::createLeftSideBar()
{
    leftToolBar = new QToolBar(this);
    leftToolBar->setMovable(false);
    leftToolBar->setOrientation(Qt::Vertical);
    addToolBar(Qt::LeftToolBarArea, leftToolBar);

    // Create actions with icons
    missionAction = new QAction("🎯", this);
    configAction = new QAction("⚙️", this);
    simulationAction = new QAction("🚁", this);
    settingsAction = new QAction("🔧", this);

    // Set fixed size for the toolbar buttons
    leftToolBar->setIconSize(QSize(40, 40));
    
    // Add actions to toolbar
    leftToolBar->addAction(missionAction);
    leftToolBar->addAction(configAction);
    leftToolBar->addAction(simulationAction);
    leftToolBar->addAction(settingsAction);

    // Connect signals
    connect(missionAction, &QAction::triggered, this, [this]() { handleLeftSidebarButton(0); });
    connect(configAction, &QAction::triggered, this, [this]() { handleLeftSidebarButton(1); });
    connect(simulationAction, &QAction::triggered, this, [this]() { handleLeftSidebarButton(2); });
    connect(settingsAction, &QAction::triggered, this, [this]() { handleLeftSidebarButton(3); });
}

void MainWindow::createRightSideBar()
{
    rightToolBar = new QToolBar(this);
    rightToolBar->setMovable(false);
    rightToolBar->setOrientation(Qt::Vertical);
    addToolBar(Qt::RightToolBarArea, rightToolBar);

    taskDetailsAction = new QAction("📋", this);
    rightToolBar->setIconSize(QSize(40, 40));
    rightToolBar->addAction(taskDetailsAction);

    connect(taskDetailsAction, &QAction::triggered, this, &MainWindow::handleRightSidebarButton);
}

void MainWindow::createLeftPanels()
{
    leftPanelDock = new QDockWidget(this);
    leftPanelDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    leftPanelDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, leftPanelDock);
    
    leftStackedWidget = new QStackedWidget(leftPanelDock);
    
    // Create panels with content
    QWidget* missionPanel = new QWidget();
    QLabel* missionLabel = new QLabel("Mission Control Panel\n\n- Flight Planning\n- Waypoint Management\n- Mission Parameters\n- Emergency Protocols");
    QVBoxLayout* missionLayout = new QVBoxLayout(missionPanel);
    missionLayout->addWidget(missionLabel);
    
    QWidget* configPanel = new QWidget();
    QLabel* configLabel = new QLabel("Configuration Panel\n\n- UAV Settings\n- Sensor Calibration\n- Communication Setup\n- Flight Parameters");
    QVBoxLayout* configLayout = new QVBoxLayout(configPanel);
    configLayout->addWidget(configLabel);
    
    QWidget* simulationPanel = new QWidget();
    QLabel* simulationLabel = new QLabel("Simulation Panel\n\n- Flight Simulation\n- Weather Conditions\n- Scenario Testing\n- Performance Analysis");
    QVBoxLayout* simulationLayout = new QVBoxLayout(simulationPanel);
    simulationLayout->addWidget(simulationLabel);
    
    QWidget* settingsPanel = new QWidget();
    QLabel* settingsLabel = new QLabel("Settings Panel\n\n- System Settings\n- User Preferences\n- Security Settings\n- Updates");
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsPanel);
    settingsLayout->addWidget(settingsLabel);
    
    leftStackedWidget->addWidget(missionPanel);
    leftStackedWidget->addWidget(configPanel);
    leftStackedWidget->addWidget(simulationPanel);
    leftStackedWidget->addWidget(settingsPanel);
    
    leftPanelDock->setWidget(leftStackedWidget);
    leftPanelDock->hide();
}

void MainWindow::createRightPanel()
{
    rightPanelDock = new QDockWidget(this);
    rightPanelDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    rightPanelDock->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, rightPanelDock);

    QWidget* taskPanel = new QWidget();
    QLabel* taskLabel = new QLabel("Task Details Panel\n\n- Current Tasks\n- Task Progress\n- Task History\n- Performance Metrics\n- Alerts and Notifications");
    QVBoxLayout* taskLayout = new QVBoxLayout(taskPanel);
    taskLayout->addWidget(taskLabel);

    rightPanelDock->setWidget(taskPanel);
    rightPanelDock->hide();
}

void MainWindow::handleLeftSidebarButton(int index)
{
    if (currentLeftPanelIndex == index && leftPanelDock->isVisible()) {
        leftPanelDock->hide();
        currentLeftPanelIndex = -1;
    } else {
        leftStackedWidget->setCurrentIndex(index);
        leftPanelDock->show();
        currentLeftPanelIndex = index;
    }
}

void MainWindow::handleRightSidebarButton()
{
    if (rightPanelDock->isVisible()) {
        rightPanelDock->hide();
    } else {
        rightPanelDock->show();
    }
}

void MainWindow::updateDateTime()
{
    QDateTime current = QDateTime::currentDateTime();
    dateTimeLabel->setText(current.toString("dd/MM/yyyy hh:mm:ss"));
}

void MainWindow::setupMainArea()
{
    centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Create and configure profile
    auto profile = new QWebEngineProfile("MapboxProfile", this);
    profile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    profile->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
    
    // Create web view and page
    m_webView = new QWebEngineView(this);
    auto page = new QWebEnginePage(profile, m_webView);
    m_webView->setPage(page);
    
    // Configure settings
    auto settings = page->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
    
    mainLayout->addWidget(m_webView);

    // Connect error handling
    connect(page, &QWebEnginePage::loadFinished, this, [this](bool ok) {
        if (!ok) {
            qDebug() << "Failed to load the map";
        }
    });

    initializeMap();
}

void MainWindow::initializeMap() {
    const QString mapboxToken = "pk.eyJ1Ijoibmlja3lqMTIxIiwiYSI6ImNtN3N3eHFtcTB1MTkya3M4Mnc0dmQxanAifQ.gLJZYJe_zH9b9yxFxQZm6g";
    
    QString html = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset='utf-8'>
            <title>Mapbox 3D Map</title>
            <meta name='viewport' content='initial-scale=1,maximum-scale=1,user-scalable=no'>
            <meta http-equiv='Content-Security-Policy' content="default-src * blob: data: 'unsafe-inline' 'unsafe-eval'; script-src * blob: data: 'unsafe-inline' 'unsafe-eval'; connect-src * blob: data:; img-src * blob: data:; frame-src *; style-src * 'unsafe-inline';">
            <link href='https://api.mapbox.com/mapbox-gl-js/v2.15.0/mapbox-gl.css' rel='stylesheet'>
            <script src='https://api.mapbox.com/mapbox-gl-js/v2.15.0/mapbox-gl.js'></script>
            <style>
                body { margin: 0; padding: 0; }
                #map { position: absolute; top: 0; bottom: 0; width: 100%; }
            </style>
        </head>
        <body>
            <div id='map'></div>
            <script>
                mapboxgl.accessToken = '%1';
                const map = new mapboxgl.Map({
                    container: 'map',
                    style: 'mapbox://styles/mapbox/streets-v12',
                    center: [-74.5, 40],
                    zoom: 9,
                    pitch: 60,
                    bearing: -60,
                    antialias: true,
                    preserveDrawingBuffer: true,
                    maxPitch: 85
                });

                map.on('load', () => {
                    map.addSource('mapbox-dem', {
                        'type': 'raster-dem',
                        'url': 'mapbox://mapbox.mapbox-terrain-dem-v1',
                        'tileSize': 512,
                        'maxzoom': 14
                    });
                    map.setTerrain({ 'source': 'mapbox-dem', 'exaggeration': 1.5 });

                    map.addLayer({
                        'id': '3d-buildings',
                        'source': 'composite',
                        'source-layer': 'building',
                        'filter': ['==', 'extrude', 'true'],
                        'type': 'fill-extrusion',
                        'minzoom': 15,
                        'paint': {
                            'fill-extrusion-color': '#aaa',
                            'fill-extrusion-height': [
                                'interpolate',
                                ['linear'],
                                ['zoom'],
                                15,
                                0,
                                15.05,
                                ['get', 'height']
                            ],
                            'fill-extrusion-base': [
                                'interpolate',
                                ['linear'],
                                ['zoom'],
                                15,
                                0,
                                15.05,
                                ['get', 'min_height']
                            ],
                            'fill-extrusion-opacity': 0.6
                        }
                    });
                });
            </script>
        </body>
        </html>
    )";

    m_webView->setHtml(html.arg(mapboxToken), QUrl("https://api.mapbox.com"));
}

void MainWindow::applyStyles()
{
    QString styleSheet = R"(
        QMainWindow {
            background-color: #1e1e1e;
            color: #d4d4d4;
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
            min-width: 40px;
            min-height: 40px;
        }
        QToolBar QToolButton:hover {
            background-color: #404040;
        }
        QToolBar QToolButton:pressed {
            background-color: #505050;
        }
        QLineEdit {
            background-color: #252526;
            color: #d4d4d4;
            border: 1px solid #454545;
            border-radius: 4px;
            padding: 5px;
            font-size: 14px;
        }
        QLabel {
            color: #d4d4d4;
            font-size: 14px;
        }
        QDockWidget {
            color: #d4d4d4;
            font-size: 14px;
            background-color: #252526;
            border: 1px solid #454545;
        }
        QDockWidget QLabel {
            padding: 10px;
            line-height: 1.5;
        }
    )";

    qApp->setStyleSheet(styleSheet);
}