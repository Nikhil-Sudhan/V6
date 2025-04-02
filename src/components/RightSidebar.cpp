#include "../../../include/components/RightSidebar.h"
#include "../../../include/components/RightsideBar/taskdetails.h"
#include "../../../include/database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QSizePolicy>
#include <QIcon>
#include <QPushButton>
#include <QStandardPaths>
#include <QDebug>

RightSidebar::RightSidebar(QWidget* parent) : QObject(parent)
{
    createToolBar(parent);
    createDockWidget(parent);
    createPanel();
    
    // Connect signals
    connect(taskDetailsAction, &QAction::triggered, this, &RightSidebar::handleButtonClick);
}

void RightSidebar::createToolBar(QWidget* parent)
{
    rightToolBar = new QToolBar(parent);
    rightToolBar->setMovable(false);
    rightToolBar->setOrientation(Qt::Vertical);
    
    // Set toolbar width to match LeftSidebar
    rightToolBar->setMinimumWidth(90);
    rightToolBar->setMaximumWidth(90);
    
    taskDetailsAction = new QAction(parent);
    taskDetailsAction->setIcon(QIcon("/home/sudhan/V6/assets/icons/tasks.png"));
    taskDetailsAction->setToolTip("Task Details");
    
    // Fallback for icon if it doesn't load
    if (taskDetailsAction->icon().isNull()) {
        qDebug() << "Warning: Task details icon failed to load, using fallback";
        taskDetailsAction->setText("Tasks");
    }
    
    // Set icon size to match LeftSidebar
    rightToolBar->setIconSize(QSize(42, 42));
    
    // Style to match LeftSidebar
    rightToolBar->setStyleSheet(R"(
        QToolBar {
            spacing: 0px;
            padding: 0px;
            background-color: #1a1a1a;
            border-left: 1px solid #00a6ff;
        }
        QToolButton {
            padding: 12px 0px;
            margin: 0px;
            border: none;
            background-color: #1a1a1a;
            min-width: 50px;
        }
        QToolButton:hover {
            background-color: #252525;
        }
        QToolButton:pressed, QToolButton:checked {
            background-color: #00a6ff;
        }
    )");
    
    // Add action
    rightToolBar->addAction(taskDetailsAction);
}

void RightSidebar::createDockWidget(QWidget* parent)
{
    rightPanelDock = new QDockWidget(parent);
    rightPanelDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    rightPanelDock->setAllowedAreas(Qt::RightDockWidgetArea);
    
    // Set panel dimensions to match LeftSidebar
    rightPanelDock->setMinimumWidth(350);
    rightPanelDock->setMaximumWidth(350);
    rightPanelDock->setMinimumHeight(600);
    
    // Set panel styling
    rightPanelDock->setStyleSheet(R"(
        QDockWidget {
            border: none;
            background-color: #1a1a1a;
        }
        QWidget {
            background-color: #1a1a1a;
            color: #ffffff;
        }
    )");
    
    // Create a container widget for the dock
    QWidget* dockContents = new QWidget(rightPanelDock);
    rightPanelDock->setWidget(dockContents);
    
    rightPanelDock->hide(); // Hide by default
}

void RightSidebar::createPanel()
{
    // Create the task details widget
    TaskDetails* taskDetailsPanel = new TaskDetails();
    
    // Connect signals
    connect(taskDetailsPanel, &TaskDetails::assignTask, this, &RightSidebar::assignTask);
    
    // Set as dock widget content
    rightPanelDock->setWidget(taskDetailsPanel);
    
    // Load mission list
    taskDetailsPanel->loadMissionList();
}

void RightSidebar::handleButtonClick()
{
    // Toggle panel visibility
    if (rightPanelDock->isVisible()) {
        rightPanelDock->hide();
    } else {
        rightPanelDock->show();
    }
    
    // Emit visibility changed signal
    emit visibilityChanged(rightPanelDock->isVisible());
}

void RightSidebar::updateTaskList(const QString& missionType, const QString& vehicle, const QString& prompt)
{
    // Forward to the task details panel
    TaskDetails* taskDetailsPanel = qobject_cast<TaskDetails*>(rightPanelDock->widget());
    if (taskDetailsPanel) {
        taskDetailsPanel->updateTaskList(missionType, vehicle, prompt);
    }
}

void RightSidebar::loadMissionList()
{
    // Forward to the task details panel
    TaskDetails* taskDetailsPanel = qobject_cast<TaskDetails*>(rightPanelDock->widget());
    if (taskDetailsPanel) {
        taskDetailsPanel->loadMissionList();
    }
}

void RightSidebar::displayMissionDetails(int missionId)
{
    // Forward to the task details panel
    TaskDetails* taskDetailsPanel = qobject_cast<TaskDetails*>(rightPanelDock->widget());
    if (taskDetailsPanel) {
        taskDetailsPanel->displayMissionDetails(missionId);
    }
}

void RightSidebar::showAssignTaskDialog()
{
    // Forward to the task details panel
    TaskDetails* taskDetailsPanel = qobject_cast<TaskDetails*>(rightPanelDock->widget());
    if (taskDetailsPanel) {
        taskDetailsPanel->showAssignTaskDialog();
    }
}
