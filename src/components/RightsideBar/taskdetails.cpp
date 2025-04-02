
#include "../../../include/components/RightsideBar/taskdetails.h"
#include <QMessageBox>
#include <QDialog>
#include <QComboBox>
#include <QTextEdit>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDebug>

TaskDetails::TaskDetails(QWidget* parent) : QWidget(parent), taskDetailsExpanded(true), assetDataExpanded(true), currentMissionId(-1)
{
    setupUI();
    setupConnections();
}

void TaskDetails::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Create mission list section
    QWidget* missionListSection = new QWidget();
    QVBoxLayout* missionListLayout = new QVBoxLayout(missionListSection);
    missionListLayout->setContentsMargins(10, 10, 10, 10);
    
    QLabel* missionListLabel = new QLabel("ASSIGNED TASKS");
    QFont listFont = missionListLabel->font();
    listFont.setBold(true);
    missionListLabel->setFont(listFont);
    
    missionListWidget = new QListWidget();
    missionListWidget->setStyleSheet("QListWidget { background-color: #1a1a1a; border: 1px solid #333; }");
    missionListWidget->setMaximumHeight(150);
    
    missionListLayout->addWidget(missionListLabel);
    missionListLayout->addWidget(missionListWidget);
    
    mainLayout->addWidget(missionListSection);
    
    // Create mission details section
    QWidget* missionDetailsSection = new QWidget();
    QVBoxLayout* missionDetailsLayout = new QVBoxLayout(missionDetailsSection);
    missionDetailsLayout->setContentsMargins(10, 10, 10, 10);
    
    // User who assigned the task
    QWidget* userWidget = new QWidget();
    QHBoxLayout* userLayout = new QHBoxLayout(userWidget);
    userLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* userLabel = new QLabel("Assigned by:");
    userNameLabel = new QLabel("Nicky");
    userNameLabel->setStyleSheet("font-weight: bold; color: #00a6ff;");
    
    userLayout->addWidget(userLabel);
    userLayout->addWidget(userNameLabel);
    userLayout->addStretch();
    
    // Mission title and type
    missionTitleLabel = new QLabel("Mission Title");
    QFont titleFont = missionTitleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(16);
    missionTitleLabel->setFont(titleFont);
    
    missionTypeLabel = new QLabel("Mission Type");
    missionTypeLabel->setStyleSheet("color: #00a6ff;");
    
    // Start/Pause and Cancel buttons
    QWidget* controlWidget = new QWidget();
    QHBoxLayout* controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setContentsMargins(0, 10, 0, 10);
    
    startPauseButton = new QPushButton("▶ Start");
    startPauseButton->setStyleSheet("background-color: #00a6ff; color: white; padding: 5px 15px;");
    
    cancelButton = new QPushButton("Cancel");
    cancelButton->setStyleSheet("background-color: #ff3b30; color: white; padding: 5px 15px;");
    
    controlLayout->addWidget(startPauseButton);
    controlLayout->addWidget(cancelButton);
    controlLayout->addStretch();
    
    missionDetailsLayout->addWidget(userWidget);
    missionDetailsLayout->addWidget(missionTitleLabel);
    missionDetailsLayout->addWidget(missionTypeLabel);
    missionDetailsLayout->addWidget(controlWidget);
    
    mainLayout->addWidget(missionDetailsSection);
    
    // Create task details section (collapsible)
    taskDetailsWidget = new QWidget();
    QVBoxLayout* taskDetailsLayout = new QVBoxLayout(taskDetailsWidget);
    taskDetailsLayout->setContentsMargins(10, 10, 10, 10);
    
    QWidget* taskHeaderWidget = new QWidget();
    QHBoxLayout* taskHeaderLayout = new QHBoxLayout(taskHeaderWidget);
    taskHeaderLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* taskHeaderLabel = new QLabel("TASK DETAILS");
    QFont taskHeaderFont = taskHeaderLabel->font();
    taskHeaderFont.setBold(true);
    taskHeaderLabel->setFont(taskHeaderFont);
    taskHeaderLabel->setStyleSheet("color: #00a6ff;");
    
    taskCollapseIcon = new QLabel("▼"); // Changed to down arrow to indicate collapsed
    taskCollapseIcon->setCursor(Qt::PointingHandCursor);
    
    taskHeaderLayout->addWidget(taskHeaderLabel);
    taskHeaderLayout->addStretch();
    taskHeaderLayout->addWidget(taskCollapseIcon);
    
    QWidget* taskContentWidget = new QWidget();
    QGridLayout* taskContentLayout = new QGridLayout(taskContentWidget);
    taskContentLayout->setContentsMargins(0, 10, 0, 0);
    
    // Task details content
    QLabel* statusLabel = new QLabel("Status:");
    statusLabel->setStyleSheet("color: #ffffff;");
    taskStatusLabel = new QLabel("Active");
    taskStatusLabel->setStyleSheet("color: #00a6ff;");
    
    QLabel* timeLabel = new QLabel("Time Active:");
    timeLabel->setStyleSheet("color: #ffffff;");
    taskTimeLabel = new QLabel("0m 0s");
    taskTimeLabel->setStyleSheet("color: #ffffff;");
    
    QLabel* assetLabel = new QLabel("Asset:");
    assetLabel->setStyleSheet("color: #ffffff;");
    taskAssetLabel = new QLabel("Atlas");
    taskAssetLabel->setStyleSheet("color: #ffffff;");
    
    QLabel* objectiveLabel = new QLabel("Objective:");
    objectiveLabel->setStyleSheet("color: #ffffff;");
    taskObjectiveLabel = new QLabel("Task objective will appear here");
    taskObjectiveLabel->setWordWrap(true);
    taskObjectiveLabel->setStyleSheet("color: #ffffff;");
    
    taskContentLayout->addWidget(statusLabel, 0, 0);
    taskContentLayout->addWidget(taskStatusLabel, 0, 1);
    taskContentLayout->addWidget(timeLabel, 1, 0);
    taskContentLayout->addWidget(taskTimeLabel, 1, 1);
    taskContentLayout->addWidget(assetLabel, 2, 0);
    taskContentLayout->addWidget(taskAssetLabel, 2, 1);
    taskContentLayout->addWidget(objectiveLabel, 3, 0, 1, 2);
    taskContentLayout->addWidget(taskObjectiveLabel, 4, 0, 1, 2);
    
    taskDetailsLayout->addWidget(taskHeaderWidget);
    taskDetailsLayout->addWidget(taskContentWidget);
    
    // Hide task details content by default
    taskContentWidget->hide();
    taskDetailsExpanded = false;
    
    mainLayout->addWidget(taskDetailsWidget);
    
    // Create asset data section (collapsible)
    assetDataWidget = new QWidget();
    QVBoxLayout* assetDataLayout = new QVBoxLayout(assetDataWidget);
    assetDataLayout->setContentsMargins(10, 10, 10, 10);
    
    QWidget* assetDataHeaderWidget = new QWidget();
    QHBoxLayout* assetDataHeaderLayout = new QHBoxLayout(assetDataHeaderWidget);
    assetDataHeaderLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* assetDataLabel = new QLabel("ASSET DATA");
    assetDataLabel->setObjectName("assetDataLabel");
    QFont assetDataFont = assetDataLabel->font();
    assetDataFont.setBold(true);
    assetDataLabel->setFont(assetDataFont);
    assetDataLabel->setStyleSheet("color: #00a6ff;");
    
    assetCollapseIcon = new QLabel("▼"); // Changed to down arrow to indicate collapsed
    assetCollapseIcon->setCursor(Qt::PointingHandCursor);
    
    assetDataHeaderLayout->addWidget(assetDataLabel);
    assetDataHeaderLayout->addStretch();
    assetDataHeaderLayout->addWidget(assetCollapseIcon);
    
    assetDataLayout->addWidget(assetDataHeaderWidget);
    
    // Create asset data grid
    QWidget* dataGridWidget = new QWidget();
    QGridLayout* dataGridLayout = new QGridLayout(dataGridWidget);
    dataGridLayout->setContentsMargins(0, 10, 0, 0);
    dataGridLayout->setHorizontalSpacing(30);
    dataGridLayout->setVerticalSpacing(20);
    
    // Altitude
    QLabel* altitudeLabel = new QLabel("Altitude");
    altitudeLabel->setStyleSheet("color: #ffffff;");
    altitudeMSLValue = new QLabel("0 ft MSL");
    altitudeMSLValue->setStyleSheet("color: #ffffff;");
    altitudeAGLValue = new QLabel("0 ft AGL");
    altitudeAGLValue->setStyleSheet("color: #ffffff;");
    
    // Heading
    QLabel* headingLabel = new QLabel("Heading");
    headingLabel->setStyleSheet("color: #ffffff;");
    headingValue = new QLabel("0°");
    headingValue->setStyleSheet("color: #ffffff;");
    
    // Speed
    QLabel* speedLabel = new QLabel("Speed");
    speedLabel->setStyleSheet("color: #ffffff;");
    speedValue = new QLabel("0 mph");
    speedValue->setStyleSheet("color: #ffffff;");
    
    // Location
    QLabel* locationLabel = new QLabel("Location");
    locationLabel->setStyleSheet("color: #ffffff;");
    locationValue = new QLabel("0.0°, 0.0°");
    locationValue->setStyleSheet("color: #ffffff;");
    
    // Add to grid
    dataGridLayout->addWidget(altitudeLabel, 0, 0);
    dataGridLayout->addWidget(altitudeMSLValue, 1, 0);
    dataGridLayout->addWidget(altitudeAGLValue, 2, 0);
    
    dataGridLayout->addWidget(headingLabel, 0, 1);
    dataGridLayout->addWidget(headingValue, 1, 1);
    
    dataGridLayout->addWidget(speedLabel, 3, 0);
    dataGridLayout->addWidget(speedValue, 4, 0);
    
    dataGridLayout->addWidget(locationLabel, 3, 1);
    dataGridLayout->addWidget(locationValue, 4, 1);
    
    assetDataLayout->addWidget(dataGridWidget);
    
    // Hide asset data by default
    dataGridWidget->hide();
    assetDataExpanded = false;
    
    mainLayout->addWidget(assetDataWidget);
    
    // Add stretch to push everything to the top
    mainLayout->addStretch();

    // Set panel styles
    setStyleSheet(R"(
        QWidget {
            background-color: #1a1a1a;
            color: #e0e0e0;
        }
        QLabel {
            color: #e0e0e0;
        }
        QListWidget::item {
            padding: 5px;
            border-bottom: 1px solid #333;
        }
        QListWidget::item:selected {
            background-color: #00a6ff;
            color: white;
        }
    )");
}

void TaskDetails::setupConnections()
{
    // Connect signals
    connect(startPauseButton, &QPushButton::clicked, this, &TaskDetails::handleStartPauseClicked);
    connect(cancelButton, &QPushButton::clicked, this, &TaskDetails::handleCancelClicked);
    connect(missionListWidget, &QListWidget::itemClicked, this, &TaskDetails::handleMissionItemClicked);
    
    // Make the collapse icons clickable
    taskCollapseIcon->setText("<a href='#'>▼</a>"); // Down arrow to indicate collapsed
    assetCollapseIcon->setText("<a href='#'>▼</a>"); // Down arrow to indicate collapsed
    
    connect(taskCollapseIcon, &QLabel::linkActivated, this, &TaskDetails::toggleTaskDetails);
    connect(assetCollapseIcon, &QLabel::linkActivated, this, &TaskDetails::toggleAssetData);
}

void TaskDetails::addTaskItem(const QString& taskName, const QString& status, const QString& time)
{
    QWidget* taskItem = new QWidget();
    QHBoxLayout* taskItemLayout = new QHBoxLayout(taskItem);
    taskItemLayout->setContentsMargins(0, 0, 0, 0);
    
    // Task icon (blue for active, orange for others)
    QLabel* taskIcon = new QLabel();
    if (status.contains("DESIGNATED")) {
        taskIcon->setText("🔵");
    } else {
        taskIcon->setText("🟠");
    }
    
    // Task name and status
    QWidget* taskInfoWidget = new QWidget();
    QVBoxLayout* taskInfoLayout = new QVBoxLayout(taskInfoWidget);
    taskInfoLayout->setContentsMargins(0, 0, 0, 0);
    taskInfoLayout->setSpacing(2);
    
    QLabel* taskNameLabel = new QLabel(taskName);
    QFont nameFont = taskNameLabel->font();
    nameFont.setBold(true);
    taskNameLabel->setFont(nameFont);
    
    QLabel* taskStatusLabel = new QLabel(status);
    taskStatusLabel->setStyleSheet("color: #888888;");
    
    taskInfoLayout->addWidget(taskNameLabel);
    if (!status.isEmpty()) {
        taskInfoLayout->addWidget(taskStatusLabel);
    }
    
    // Time display
    QLabel* timeLabel = new QLabel(time);
    timeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    taskItemLayout->addWidget(taskIcon);
    taskItemLayout->addWidget(taskInfoWidget);
    taskItemLayout->addStretch();
    if (!time.isEmpty()) {
        taskItemLayout->addWidget(timeLabel);
    }
    
    taskListLayout->addWidget(taskItem);
    taskItems.append(taskItem);
}

void TaskDetails::loadMissionList()
{
    // Clear the list
    missionListWidget->clear();
    itemToMissionId.clear();
    
    // Get mission history from database
    QSqlQuery query = DatabaseManager::instance().getMissionHistory();
    
    int itemIndex = 0;
    while (query.next()) {
        int missionId = query.value("id").toInt();
        QString missionType = query.value("mission_type").toString();
        QString missionTitle = query.value("mission_title").toString();
        QString userName = query.value("user_name").toString();
        QDateTime timestamp = query.value("timestamp").toDateTime();
        
        // If mission title is empty, use mission type
        if (missionTitle.isEmpty()) {
            missionTitle = missionType;
        }
        
        // Format the timestamp
        QString timeStr = timestamp.toString("dd/MM/yyyy HH:mm");
        
        // Create list item
        QString displayText = QString("%1 - %2").arg(missionTitle, timeStr);
        QListWidgetItem* item = new QListWidgetItem(displayText);
        
        // Store additional data
        item->setData(Qt::UserRole, missionId);
        
        // Add to list
        missionListWidget->addItem(item);
        
        // Store mission ID mapping
        itemToMissionId[itemIndex++] = missionId;
    }
    
    // Select first item if available
    if (missionListWidget->count() > 0) {
        missionListWidget->setCurrentRow(0);
        displayMissionDetails(itemToMissionId[0]);
    }
}

void TaskDetails::displayMissionDetails(int missionId)
{
    if (missionId <= 0) {
        return;
    }
    
    currentMissionId = missionId;
    
    // Get mission details from database
    QSqlQuery query = DatabaseManager::instance().getMissionDetails(missionId);
    
    if (query.next()) {
        QString missionType = query.value("mission_type").toString();
        QString missionTitle = query.value("mission_title").toString();
        QString userName = query.value("user_name").toString();
        QString vehicle = query.value("vehicle").toString();
        QString prompt = query.value("prompt").toString();
        QString status = query.value("status").toString();
        QDateTime timestamp = query.value("timestamp").toDateTime();
        
        // If mission title is empty, use mission type
        if (missionTitle.isEmpty()) {
            missionTitle = missionType;
        }
        
        // Update UI
        userNameLabel->setText(userName);
        missionTitleLabel->setText(missionTitle);
        missionTypeLabel->setText(missionType);
        
        // Update task details (even if hidden)
        taskStatusLabel->setText(status);
        taskAssetLabel->setText(vehicle);
        taskObjectiveLabel->setText(prompt);
        
        // Calculate time active
        QDateTime now = QDateTime::currentDateTime();
        int secsActive = timestamp.secsTo(now);
        int mins = secsActive / 60;
        int secs = secsActive % 60;
        taskTimeLabel->setText(QString("%1m %2s").arg(mins).arg(secs));
        
        // Update start/pause button based on status
        if (status == "ACTIVE") {
            startPauseButton->setText("⏸ Pause");
            startPauseButton->setStyleSheet("background-color: #ff9500; color: white; padding: 5px 15px;");
        } else {
            startPauseButton->setText("▶ Start");
            startPauseButton->setStyleSheet("background-color: #00a6ff; color: white; padding: 5px 15px;");
        }
        
        // Update asset data (even if hidden)
        updateAssetData(missionId);
        
        // Ensure sections remain collapsed if they were collapsed
        // Find task details content widget
        if (!taskDetailsExpanded) {
            QWidget* contentWidget = nullptr;
            if (taskDetailsWidget->layout()->count() > 1) {
                QLayoutItem* item = taskDetailsWidget->layout()->itemAt(1);
                if (item && item->widget()) {
                    contentWidget = item->widget();
                    contentWidget->hide();
                }
            }
        }
        
        // Find asset data content widget
        if (!assetDataExpanded) {
            QWidget* contentWidget = nullptr;
            if (assetDataWidget->layout()->count() > 1) {
                QLayoutItem* item = assetDataWidget->layout()->itemAt(1);
                if (item && item->widget()) {
                    contentWidget = item->widget();
                    contentWidget->hide();
                }
            }
        }
    }
}

void TaskDetails::updateAssetData(int missionId)
{
    // In a real application, this would fetch real-time data from the asset
    // For now, we'll just use placeholder values
    
    // Simulate different values based on mission ID
    int altitude = 100 + (missionId * 10);
    int heading = (missionId * 45) % 360;
    int speed = 20 + (missionId * 5);
    
    // Update UI
    altitudeMSLValue->setText(QString("%1 ft MSL").arg(altitude));
    altitudeAGLValue->setText(QString("%1 ft AGL").arg(altitude - 50));
    headingValue->setText(QString("%1°").arg(heading));
    speedValue->setText(QString("%1 mph").arg(speed));
    
    // Generate random-ish coordinates near San Francisco
    double lat = 37.7749 + ((missionId % 10) * 0.01);
    double lon = -122.4194 + ((missionId % 5) * 0.01);
    locationValue->setText(QString("%1°, %2°").arg(lat, 0, 'f', 4).arg(lon, 0, 'f', 4));
}

void TaskDetails::showAssignTaskDialog()
{
    QDialog dialog;
    dialog.setWindowTitle("Assign New Task");
    dialog.setMinimumWidth(400);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // Mission Type Selection
    QLabel* missionTypeLabel = new QLabel("Mission Type:");
    QComboBox* missionTypeCombo = new QComboBox();
    missionTypeCombo->addItems({"Surveillance", "Emergency", "Rescue", "Friendly"});
    
    // Vehicle Selection
    QLabel* vehicleLabel = new QLabel("Vehicle:");
    QComboBox* vehicleCombo = new QComboBox();
    vehicleCombo->addItems({"Atlas", "Bolt", "Barbarian"});
    
    // Prompt Input
    QLabel* promptLabel = new QLabel("Mission Details:");
    QTextEdit* promptTextEdit = new QTextEdit();
    promptTextEdit->setPlaceholderText("Enter mission details here...");
    promptTextEdit->setMinimumHeight(100);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* cancelButton = new QPushButton("Cancel");
    QPushButton* assignButton = new QPushButton("Assign Task");
    assignButton->setStyleSheet("background-color: #00a6ff; color: white;");
    
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(assignButton);
    
    // Add widgets to layout
    layout->addWidget(missionTypeLabel);
    layout->addWidget(missionTypeCombo);
    layout->addWidget(vehicleLabel);
    layout->addWidget(vehicleCombo);
    layout->addWidget(promptLabel);
    layout->addWidget(promptTextEdit);
    layout->addLayout(buttonLayout);
    
    // Connect signals
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(assignButton, &QPushButton::clicked, [&]() {
        // Validate input
        if (promptTextEdit->toPlainText().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Missing Information", "Please enter mission details.");
            return;
        }
        
        // Get values
        QString missionType = missionTypeCombo->currentText();
        QString vehicle = vehicleCombo->currentText();
        QString prompt = promptTextEdit->toPlainText().trimmed();
        
        // Emit signal
        emit assignTask(missionType, vehicle, prompt);
        
        // Update task list
        updateTaskList(missionType, vehicle, prompt);
        
        dialog.accept();
    });
    
    dialog.exec();
}

void TaskDetails::updateTaskList(const QString& missionType, const QString& vehicle, const QString& prompt)
{
    // In a real application, this would add the task to the database and update the UI
    // For now, we'll just reload the mission list
    loadMissionList();
}

void TaskDetails::handleMissionItemClicked(QListWidgetItem* item)
{
    if (item) {
        int row = missionListWidget->row(item);
        if (itemToMissionId.contains(row)) {
            int missionId = itemToMissionId[row];
            displayMissionDetails(missionId);
        }
    }
}

void TaskDetails::toggleTaskDetails()
{
    // Get the content widget (second widget in the layout)
    QWidget* contentWidget = nullptr;
    if (taskDetailsWidget->layout()->count() > 1) {
        QLayoutItem* item = taskDetailsWidget->layout()->itemAt(1);
        if (item && item->widget()) {
            contentWidget = item->widget();
        }
    }
    
    if (contentWidget) {
        taskDetailsExpanded = !taskDetailsExpanded;
        contentWidget->setVisible(taskDetailsExpanded);
        
        // Update icon with HTML link
        taskCollapseIcon->setText(taskDetailsExpanded ? "<a href='#'>▲</a>" : "<a href='#'>▼</a>");
    }
}

void TaskDetails::toggleAssetData()
{
    // Get the content widget (second widget in the layout)
    QWidget* contentWidget = nullptr;
    if (assetDataWidget->layout()->count() > 1) {
        QLayoutItem* item = assetDataWidget->layout()->itemAt(1);
        if (item && item->widget()) {
            contentWidget = item->widget();
        }
    }
    
    if (contentWidget) {
        assetDataExpanded = !assetDataExpanded;
        contentWidget->setVisible(assetDataExpanded);
        
        // Update icon with HTML link
        assetCollapseIcon->setText(assetDataExpanded ? "<a href='#'>▲</a>" : "<a href='#'>▼</a>");
    }
}

void TaskDetails::handleStartPauseClicked()
{
    if (currentMissionId <= 0) {
        return;
    }
    
    // Get current status
    QSqlQuery query = DatabaseManager::instance().getMissionDetails(currentMissionId);
    if (query.next()) {
        QString status = query.value("status").toString();
        
        // Toggle status
        QString newStatus = (status == "ACTIVE") ? "STANDBY" : "ACTIVE";
        
        // Update database
        DatabaseManager::instance().updateMissionStatus(currentMissionId, newStatus);
        
        // Update UI
        if (newStatus == "ACTIVE") {
            startPauseButton->setText("⏸ Pause");
            startPauseButton->setStyleSheet("background-color: #ff9500; color: white; padding: 5px 15px;");
            taskStatusLabel->setText("ACTIVE");
            taskStatusLabel->setStyleSheet("color: #00a6ff;");
        } else {
            startPauseButton->setText("▶ Start");
            startPauseButton->setStyleSheet("background-color: #00a6ff; color: white; padding: 5px 15px;");
            taskStatusLabel->setText("STANDBY");
            taskStatusLabel->setStyleSheet("color: #ff9500;");
        }
    }
}

void TaskDetails::handleCancelClicked()
{
    if (currentMissionId <= 0) {
        return;
    }
    
    // Update status to CANCELLED
    DatabaseManager::instance().updateMissionStatus(currentMissionId, "CANCELLED");
    
    // Update UI
    startPauseButton->setText("▶ Start");
    startPauseButton->setStyleSheet("background-color: #00a6ff; color: white; padding: 5px 15px;");
    taskStatusLabel->setText("CANCELLED");
    taskStatusLabel->setStyleSheet("color: #ff3b30;");
}