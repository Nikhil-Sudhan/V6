#include "../../include/components/RightSidebar.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QSizePolicy>
#include <QIcon>

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
    
    // Set toolbar width
    rightToolBar->setMinimumWidth(50);
    rightToolBar->setMaximumWidth(50);
    
    taskDetailsAction = new QAction(parent);
    taskDetailsAction->setIcon(QIcon("/home/sudhan/V6/assets/icons/tasks.png"));
    taskDetailsAction->setToolTip("Task Details");
    
    rightToolBar->setIconSize(QSize(32, 32));
    
    // Remove spacing between items
    rightToolBar->setStyleSheet(R"(
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
    
    // Add action directly without spacers
    rightToolBar->addAction(taskDetailsAction);
}

void RightSidebar::createDockWidget(QWidget* parent)
{
    rightPanelDock = new QDockWidget(parent);
    rightPanelDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    rightPanelDock->setAllowedAreas(Qt::RightDockWidgetArea);
    
    // Set panel width
    rightPanelDock->setMinimumWidth(350);
    rightPanelDock->setMaximumWidth(350);
    
    rightPanelDock->hide(); // Hide by default
}

void RightSidebar::createPanel()
{
    QWidget* taskPanel = new QWidget();
    QVBoxLayout* taskLayout = new QVBoxLayout(taskPanel);
    taskLayout->setSpacing(20);
    taskLayout->setContentsMargins(20, 20, 20, 20);

    // Add title with icon
    QWidget* titleContainer = new QWidget();
    QHBoxLayout* titleLayout = new QHBoxLayout(titleContainer);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* titleIcon = new QLabel("📋");
    QFont iconFont = titleIcon->font();
    iconFont.setPointSize(18);
    titleIcon->setFont(iconFont);
    
    QLabel* titleLabel = new QLabel("Task Details");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    
    titleLayout->addWidget(titleIcon);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    
    taskLayout->addWidget(titleContainer);
    
    // Add separator
    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("background-color: #3e3e42;");
    taskLayout->addWidget(separator);

    // Add content with styled sections
    QLabel* currentTasksTitle = new QLabel("Current Tasks");
    currentTasksTitle->setStyleSheet("font-weight: bold; color: #0078d7; font-size: 14px;");
    taskLayout->addWidget(currentTasksTitle);
    
    QLabel* currentTasksList = new QLabel(
        "• <b>Surveillance Mission</b> - <span style='color:#4CAF50;'>In Progress</span>\n"
        "• <b>Emergency Response</b> - <span style='color:#FFC107;'>Pending</span>"
    );
    currentTasksList->setStyleSheet("font-size: 12px; line-height: 1.5;");
    taskLayout->addWidget(currentTasksList);
    
    taskLayout->addSpacing(10);
    
    QLabel* progressTitle = new QLabel("Task Progress");
    progressTitle->setStyleSheet("font-weight: bold; color: #0078d7; font-size: 14px;");
    taskLayout->addWidget(progressTitle);
    
    QLabel* progressList = new QLabel(
        "• <b>Mission Planning:</b> <span style='color:#4CAF50;'>100%</span>\n"
        "• <b>Route Optimization:</b> <span style='color:#FFC107;'>75%</span>\n"
        "• <b>Resource Allocation:</b> <span style='color:#FFC107;'>50%</span>"
    );
    progressList->setStyleSheet("font-size: 12px; line-height: 1.5;");
    taskLayout->addWidget(progressList);
    
    taskLayout->addSpacing(10);
    
    QLabel* alertsTitle = new QLabel("Recent Alerts");
    alertsTitle->setStyleSheet("font-weight: bold; color: #0078d7; font-size: 14px;");
    taskLayout->addWidget(alertsTitle);
    
    QLabel* alertsList = new QLabel(
        "• <span style='color:#F44336;'>Low Battery Warning</span> - Atlas\n"
        "• <span style='color:#FFC107;'>Weather Alert</span> - High Winds"
    );
    alertsList->setStyleSheet("font-size: 12px; line-height: 1.5;");
    taskLayout->addWidget(alertsList);

    // Add stretch to push everything to the top
    taskLayout->addStretch();

    rightPanelDock->setWidget(taskPanel);
}

void RightSidebar::handleButtonClick()
{
    bool newVisibility = !rightPanelDock->isVisible();
    rightPanelDock->setVisible(newVisibility);
    emit visibilityChanged(newVisibility);
} 