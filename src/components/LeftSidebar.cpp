#include "../../include/components/LeftSidebar.h"
#include "../../include/components/LeftsideBar/missioncontrol.h"
#include "../../include/components/LeftsideBar/vechileconfiguration.h"
#include "../../include/components/LeftsideBar/settings.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QToolBar>
#include <QSizePolicy>
#include <QHBoxLayout>
#include <QFrame>
#include <QIcon>
#include <QMessageBox>

LeftSidebar::LeftSidebar(QWidget* parent) : QObject(parent), currentPanelIndex(-1)
{
    createToolBar(parent);
    createDockWidget(parent);
    createPanels();
    setupConnections();
}

void LeftSidebar::createToolBar(QWidget* parent)
{
    leftToolBar = new QToolBar(parent);
    leftToolBar->setMovable(false);
    leftToolBar->setOrientation(Qt::Vertical);
    
    // Set toolbar width
    leftToolBar->setMinimumWidth(90);
    leftToolBar->setMaximumWidth(90);
    
    // Create actions with PNG icons
    missionAction = new QAction(parent);
    missionAction->setIcon(QIcon("/home/sudhan/V6/assets/icons/mission.png"));
    missionAction->setToolTip("Mission Control");
    
    configAction = new QAction(parent);
    configAction->setIcon(QIcon("/home/sudhan/V6/assets/icons/config.png"));
    configAction->setToolTip("Vehicle Configuration");
    
    settingsAction = new QAction(parent);
    settingsAction->setIcon(QIcon("/home/sudhan/V6/assets/icons/settings.png"));
    settingsAction->setToolTip("Settings");

    // Fallback for icons if they don't load
    if (missionAction->icon().isNull()) {
        qDebug() << "Warning: Mission icon failed to load, using fallback";
        missionAction->setText("Mission");
    }
    if (configAction->icon().isNull()) {
        qDebug() << "Warning: Config icon failed to load, using fallback";
        configAction->setText("Config");
    }
    if (settingsAction->icon().isNull()) {
        qDebug() << "Warning: Settings icon failed to load, using fallback";
        settingsAction->setText("Settings");
    }
    
    // Set icon size
    leftToolBar->setIconSize(QSize(42, 42));
    
    // Remove spacing between items
    leftToolBar->setStyleSheet(R"(
        QToolBar {
            spacing: 0px;
            padding: 0px;
            background-color: #1a1a1a;
            border-right: 1px solid #00a6ff;
        }
        QToolButton {
            padding: 12px 0px;
            margin: 0px;
            border: none;
            background-color: #1a1a1a;
            min-width: 90px;
        }
        QToolButton:hover {
            background-color: #252525;
        }
        QToolButton:pressed, QToolButton:checked {
            background-color: #00a6ff;
        }
    )");
    
    // Add actions directly without spacers
    leftToolBar->addAction(missionAction);
    leftToolBar->addAction(configAction);
    leftToolBar->addAction(settingsAction);
}

void LeftSidebar::createDockWidget(QWidget* parent)
{
    leftPanelDock = new QDockWidget(parent);
    leftPanelDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    leftPanelDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    
    // Set panel width
    leftPanelDock->setMinimumWidth(350);
    leftPanelDock->setMaximumWidth(350);
    
    leftStackedWidget = new QStackedWidget(leftPanelDock);
    leftPanelDock->setWidget(leftStackedWidget);
    leftPanelDock->hide(); // Hide by default
}

void LeftSidebar::createPanels()
{
    // Create Mission Control Panel
    missionControlPanel = new MissionControl();
    
    // Create Vehicle Configuration Panel
    vehicleConfigPanel = new VehicleConfiguration();
    
    // Create Settings Panel
    settingsPanel = new Settings();
    
    // Add panels to stacked widget
    leftStackedWidget->addWidget(missionControlPanel);
    leftStackedWidget->addWidget(vehicleConfigPanel);
    leftStackedWidget->addWidget(settingsPanel);
}

void LeftSidebar::setupConnections()
{
    // Connect toolbar buttons
    connect(missionAction, &QAction::triggered, this, [this]() { handleButtonClick(0); });
    connect(configAction, &QAction::triggered, this, [this]() { handleButtonClick(1); });
    connect(settingsAction, &QAction::triggered, this, [this]() { handleButtonClick(2); });
    
    // Connect mission control signals
    connect(missionControlPanel, &MissionControl::missionAssigned, 
            this, &LeftSidebar::missionAssigned);
}

void LeftSidebar::handleButtonClick(int index)
{
    if (currentPanelIndex == index && leftPanelDock->isVisible()) {
        leftPanelDock->hide();
        currentPanelIndex = -1;
    } else {
        leftStackedWidget->setCurrentIndex(index);
        leftPanelDock->show();
        currentPanelIndex = index;
    }
    
    emit panelChanged(currentPanelIndex);
}

void LeftSidebar::setTitle(const QString& title)
{
    leftPanelDock->setWindowTitle(title);
}