#include "../../include/components/LeftSidebar.h"
#include "../../include/components/VehicleInfoWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QToolBar>
#include <QSizePolicy>
#include <QHBoxLayout>
#include <QFrame>
#include <QIcon>

LeftSidebar::LeftSidebar(QWidget* parent) : QObject(parent), currentPanelIndex(-1)
{
    createToolBar(parent);
    createDockWidget(parent);
    createPanels();
    
    // Connect signals
    connect(missionAction, &QAction::triggered, this, [this]() { handleButtonClick(0); });
    connect(configAction, &QAction::triggered, this, [this]() { handleButtonClick(1); });
    connect(simulationAction, &QAction::triggered, this, [this]() { handleButtonClick(2); });
    connect(settingsAction, &QAction::triggered, this, [this]() { handleButtonClick(3); });
}

void LeftSidebar::createToolBar(QWidget* parent)
{
    leftToolBar = new QToolBar(parent);
    leftToolBar->setMovable(false);
    leftToolBar->setOrientation(Qt::Vertical);
    
    // Set toolbar width
    leftToolBar->setMinimumWidth(50);
    leftToolBar->setMaximumWidth(50);
    
    // Create actions with better icons
    missionAction = new QAction(parent);
    missionAction->setText("🎯");
    missionAction->setToolTip("Mission Control");
    
    configAction = new QAction(parent);
    configAction->setText("⚙️");
    configAction->setToolTip("Vehicle Configuration");
    
    simulationAction = new QAction(parent);
    simulationAction->setText("🚁");
    simulationAction->setToolTip("Simulation");
    
    settingsAction = new QAction(parent);
    settingsAction->setText("🔧");
    settingsAction->setToolTip("Settings");

    // Set icon size
    leftToolBar->setIconSize(QSize(32, 32));
    
    // Remove spacing between items
    leftToolBar->setStyleSheet(R"(
        QToolBar {
            spacing: 0px;
            padding: 0px;
        }
        QToolButton {
            padding: 8px 0px;
            margin: 0px;
            border: none;
        }
        QToolButton:hover {
            background-color: #3e3e42;
        }
        QToolButton:pressed, QToolButton:checked {
            background-color: #0078d7;
        }
    )");
    
    // Add actions directly without spacers
    leftToolBar->addAction(missionAction);
    leftToolBar->addAction(configAction);
    leftToolBar->addAction(simulationAction);
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
    QWidget* missionPanel = new QWidget();
    QVBoxLayout* missionLayout = new QVBoxLayout(missionPanel);
    missionLayout->setSpacing(20);
    missionLayout->setContentsMargins(20, 20, 20, 20);

    // Add title with icon
    QWidget* titleContainer = new QWidget();
    QHBoxLayout* titleLayout = new QHBoxLayout(titleContainer);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* titleIcon = new QLabel("🎯");
    QFont iconFont = titleIcon->font();
    iconFont.setPointSize(18);
    titleIcon->setFont(iconFont);
    
    QLabel* titleLabel = new QLabel("Mission Control");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    
    titleLayout->addWidget(titleIcon);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    
    missionLayout->addWidget(titleContainer);
    
    // Add separator
    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("background-color: #3e3e42;");
    missionLayout->addWidget(separator);

    // Mission Type Selection with styled label
    QLabel* missionTypeLabel = new QLabel("Select Mission Type");
    missionTypeLabel->setStyleSheet("font-weight: bold; color: #0078d7; font-size: 14px;");
    missionLayout->addWidget(missionTypeLabel);
    
    QComboBox* missionTypeCombo = new QComboBox();
    missionTypeCombo->addItems({"Surveillance", "Emergency", "Rescue"});
    missionTypeCombo->setMinimumHeight(36);
    missionLayout->addWidget(missionTypeCombo);

    missionLayout->addSpacing(10);

    // Vehicle Selection with styled label
    QLabel* vehicleLabel = new QLabel("Select Vehicle");
    vehicleLabel->setStyleSheet("font-weight: bold; color: #0078d7; font-size: 14px;");
    missionLayout->addWidget(vehicleLabel);
    
    QComboBox* vehicleCombo = new QComboBox();
    vehicleCombo->addItems({"Atlas", "Bolt", "Barbarian"});
    vehicleCombo->setMinimumHeight(36);
    missionLayout->addWidget(vehicleCombo);

    missionLayout->addSpacing(10);

    // Prompt Input with styled label
    QLabel* promptLabel = new QLabel("Enter Prompt");
    promptLabel->setStyleSheet("font-weight: bold; color: #0078d7; font-size: 14px;");
    missionLayout->addWidget(promptLabel);
    
    QTextEdit* promptTextEdit = new QTextEdit();
    promptTextEdit->setPlaceholderText("Enter your mission details here...");
    promptTextEdit->setMinimumHeight(100);
    promptTextEdit->setMaximumHeight(100);
    missionLayout->addWidget(promptTextEdit);

    missionLayout->addSpacing(10);

    // Assign Task Button with better styling
    QPushButton* assignTaskButton = new QPushButton("Assign Task");
    assignTaskButton->setMinimumHeight(40);
    assignTaskButton->setText("✓ Assign Task");
    assignTaskButton->setIconSize(QSize(16, 16));
    missionLayout->addWidget(assignTaskButton);

    // Add stretch to push everything to the top
    missionLayout->addStretch();
    
    // Create Configuration Panel
    QWidget* configPanel = new QWidget();
    configPanel->setStyleSheet("background-color: #1a1a1a;");
    QVBoxLayout* configLayout = new QVBoxLayout(configPanel);
    configLayout->setSpacing(0);
    configLayout->setContentsMargins(0, 0, 0, 0);

    // Add scroll area
    QScrollArea* configScrollArea = new QScrollArea();
    configScrollArea->setWidgetResizable(true);
    configScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    configScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    configScrollArea->setStyleSheet("QScrollArea { background-color: #1a1a1a; border: none; }");
    
    QWidget* scrollContent = new QWidget();
    scrollContent->setStyleSheet("background-color: #1a1a1a;");
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setSpacing(20);
    scrollLayout->setContentsMargins(20, 20, 20, 20);

    // Add title with icon
    QWidget* configTitleContainer = new QWidget();
    QHBoxLayout* configTitleLayout = new QHBoxLayout(configTitleContainer);
    configTitleLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* configTitleIcon = new QLabel("⚙️");
    QFont configIconFont = configTitleIcon->font();
    configIconFont.setPointSize(18);
    configTitleIcon->setFont(configIconFont);
    
    QLabel* configTitleLabel = new QLabel("Vehicle Configuration");
    QFont configTitleFont = configTitleLabel->font();
    configTitleFont.setPointSize(16);
    configTitleFont.setBold(true);
    configTitleLabel->setFont(configTitleFont);
    
    configTitleLayout->addWidget(configTitleIcon);
    configTitleLayout->addWidget(configTitleLabel);
    configTitleLayout->addStretch();
    
    scrollLayout->addWidget(configTitleContainer);
    
    // Add separator
    QFrame* configSeparator = new QFrame();
    configSeparator->setFrameShape(QFrame::HLine);
    configSeparator->setFrameShadow(QFrame::Sunken);
    configSeparator->setStyleSheet("background-color: #3e3e42;");
    scrollLayout->addWidget(configSeparator);

    // Add vehicle info widgets with better styling
    scrollLayout->addWidget(new VehicleInfoWidget("Atlas", "Quad", "Online"));
    scrollLayout->addWidget(new VehicleInfoWidget("Voyager", "VTOL", "Standby"));
    scrollLayout->addWidget(new VehicleInfoWidget("Specter", "Quad", "Offline"));

    // Add stretch at the bottom
    scrollLayout->addStretch();

    configScrollArea->setWidget(scrollContent);
    configLayout->addWidget(configScrollArea);

    // Create Simulation Panel
    QWidget* simulationPanel = new QWidget();
    QLabel* simulationLabel = new QLabel("Simulation Panel\n\n- Flight Simulation\n- Weather Conditions\n- Scenario Testing\n- Performance Analysis");
    QVBoxLayout* simulationLayout = new QVBoxLayout(simulationPanel);
    simulationLayout->addWidget(simulationLabel);
    
    // Create Settings Panel
    QWidget* settingsPanel = new QWidget();
    QLabel* settingsLabel = new QLabel("Settings Panel\n\n- System Settings\n- User Preferences\n- Security Settings\n- Updates");
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsPanel);
    settingsLayout->addWidget(settingsLabel);
    
    // Add panels to stacked widget
    leftStackedWidget->addWidget(missionPanel);
    leftStackedWidget->addWidget(configPanel);
    leftStackedWidget->addWidget(simulationPanel);
    leftStackedWidget->addWidget(settingsPanel);

    // Add additional styles for the mission control panel
    QString missionStyle = R"(
        QComboBox {
            background-color: #252526;
            color: #d4d4d4;
            border: 1px solid #454545;
            border-radius: 4px;
            padding: 5px;
            min-height: 25px;
        }
        QComboBox:hover {
            border: 1px solid #007acc;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox::down-arrow {
            image: url(down_arrow.png);
        }
        QTextEdit {
            background-color: #252526;
            color: #d4d4d4;
            border: 1px solid #454545;
            border-radius: 4px;
            padding: 5px;
        }
        QTextEdit:focus {
            border: 1px solid #007acc;
        }
        QPushButton {
            background-color: #0e639c;
            color: #ffffff;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #1177bb;
        }
        QPushButton:pressed {
            background-color: #0d5289;
        }
        QLabel {
            color: #d4d4d4;
            font-size: 14px;
        }
    )";
    missionPanel->setStyleSheet(missionStyle);
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