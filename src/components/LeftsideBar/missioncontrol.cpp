#include "../../../include/components/LeftsideBar/missioncontrol.h"
#include "../../../include/dialogs/ResponseDialog.h"

MissionControl::MissionControl(QWidget* parent) : QWidget(parent)
{
    setupUI();
    setupConnections();
}

void MissionControl::setupUI()
{
    QVBoxLayout* missionLayout = new QVBoxLayout(this);
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
    missionTypeCombo->addItems({"Surveillance", "Emergency", "Rescue","Friendly"});
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
    
    // Apply styling
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
    setStyleSheet(missionStyle);
}

void MissionControl::setupConnections()
{
    // Connect mission panel signals
    connect(assignTaskButton, &QPushButton::clicked, this, &MissionControl::handleAssignTask);
    
    // Connect ChatGPT client signals
    connect(&ChatGPTClient::instance(), &ChatGPTClient::responseReceived, 
            this, &MissionControl::handleChatGPTResponse);
    connect(&ChatGPTClient::instance(), &ChatGPTClient::errorOccurred,
            this, &MissionControl::handleApiError);
}

void MissionControl::handleAssignTask()
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

void MissionControl::handleChatGPTResponse(int missionId, const QString& response, const QString& functions)
{
    // Show response dialog
    ResponseDialog dialog;
    dialog.setResponse(response, functions);
    dialog.exec();
}

void MissionControl::handleApiError(const QString& errorMessage)
{
    QMessageBox::critical(nullptr, "API Error", errorMessage);
}