#include "../../include/components/LeftSidebar.h"
#include "../../include/components/VehicleInfoWidget.h"
#include "../../include/api/ChatGPTClient.h"
#include "../../include/dialogs/ResponseDialog.h"
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
    
    simulationAction = new QAction(parent);
    simulationAction->setIcon(QIcon("/home/sudhan/V6/assets/icons/telemetry.png"));
    simulationAction->setToolTip("Simulation");
    
    settingsAction = new QAction(parent);
    settingsAction->setIcon(QIcon("/home/sudhan/V6/assets/icons/settings.png"));
    settingsAction->setToolTip("Settings");

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
    
    missionTypeCombo = new QComboBox();
    missionTypeCombo->addItems({"Surveillance", "Emergency", "Rescue"});
    missionTypeCombo->setMinimumHeight(36);
    missionLayout->addWidget(missionTypeCombo);

    missionLayout->addSpacing(10);

    // Vehicle Selection with styled label
    QLabel* vehicleLabel = new QLabel("Select Vehicle");
    vehicleLabel->setStyleSheet("font-weight: bold; color: #0078d7; font-size: 14px;");
    missionLayout->addWidget(vehicleLabel);
    
    vehicleCombo = new QComboBox();
    vehicleCombo->addItems({"Atlas", "Bolt", "Barbarian"});
    vehicleCombo->setMinimumHeight(36);
    missionLayout->addWidget(vehicleCombo);

    missionLayout->addSpacing(10);

    // Prompt Input with styled label
    QLabel* promptLabel = new QLabel("Enter Prompt");
    promptLabel->setStyleSheet("font-weight: bold; color: #0078d7; font-size: 14px;");
    missionLayout->addWidget(promptLabel);
    
    promptTextEdit = new QTextEdit();
    promptTextEdit->setPlaceholderText("Enter your mission details here...");
    promptTextEdit->setMinimumHeight(100);
    promptTextEdit->setMaximumHeight(100);
    missionLayout->addWidget(promptTextEdit);

    missionLayout->addSpacing(10);

    // Assign Task Button with better styling
    assignTaskButton = new QPushButton("Assign Task");
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
    configScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
        QWidget {
            background-color: #1a1a1a;
        }
        QComboBox {
            background-color: #252525;
            color: #e0e0e0;
            border: 1px solid #00a6ff;
            border-radius: 4px;
            padding: 8px;
            min-height: 25px;
        }
        QComboBox:hover {
            border: 2px solid #00a6ff;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox::down-arrow {
            image: url(down_arrow.png);
        }
        QTextEdit {
            background-color: #252525;
            color: #e0e0e0;
            border: 1px solid #00a6ff;
            border-radius: 4px;
            padding: 8px;
        }
        QTextEdit:focus {
            border: 2px solid #00a6ff;
        }
        QPushButton {
            background-color: #252525;
            color: #00a6ff;
            border: 1px solid #00a6ff;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #00a6ff;
            color: #ffffff;
        }
        QPushButton:pressed {
            background-color: #0077cc;
            color: #ffffff;
        }
        QLabel {
            color: #e0e0e0;
            font-size: 14px;
        }
        QFrame[frameShape="4"] {
            color: #00a6ff;
            background-color: #00a6ff;
            height: 1px;
        }
    )";
    missionPanel->setStyleSheet(missionStyle);
}

void LeftSidebar::setupConnections()
{
    // Connect toolbar buttons
    connect(missionAction, &QAction::triggered, this, [this]() { handleButtonClick(0); });
    connect(configAction, &QAction::triggered, this, [this]() { handleButtonClick(1); });
    connect(simulationAction, &QAction::triggered, this, [this]() { handleButtonClick(2); });
    connect(settingsAction, &QAction::triggered, this, [this]() { handleButtonClick(3); });
    
    // Connect mission panel signals
    connect(assignTaskButton, &QPushButton::clicked, this, &LeftSidebar::handleAssignTask);
    
    // Connect ChatGPT client signals
    connect(&ChatGPTClient::instance(), &ChatGPTClient::responseReceived, 
            this, &LeftSidebar::handleChatGPTResponse);
    connect(&ChatGPTClient::instance(), &ChatGPTClient::errorOccurred,
            this, &LeftSidebar::handleApiError);
}

void LeftSidebar::handleAssignTask()
{
    // Validate inputs
    if (promptTextEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(nullptr, "Missing Information", "Please enter a prompt for the mission.");
        return;
    }
    
    // Get values from UI
    QString missionType = missionTypeCombo->currentText();
    QString vehicle = vehicleCombo->currentText();
    QString prompt = promptTextEdit->toPlainText().trimmed();
    
    // Emit signal for mission assignment
    emit missionAssigned(missionType, vehicle, prompt);
    
    // Send to ChatGPT API
    ChatGPTClient::instance().sendPrompt(missionType, vehicle, prompt);
    
    // Show loading indicator or message
    QMessageBox::information(nullptr, "Task Assigned", 
        "Task has been assigned and sent to ChatGPT. The GeoJSON data will be displayed in the terminal and in a dialog window.");
}

void LeftSidebar::handleChatGPTResponse(int missionId, const QString& response, const QString& functions)
{
    // Show response dialog
    ResponseDialog dialog;
    dialog.setResponse(response, functions);
    dialog.exec();
}

void LeftSidebar::handleApiError(const QString& errorMessage)
{
    QMessageBox::critical(nullptr, "API Error", errorMessage);
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