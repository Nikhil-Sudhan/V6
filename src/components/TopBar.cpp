#include "../../include/components/TopBar.h"
#include <QSizePolicy>
#include <QDateTime>
#include <QFont>
#include <QHBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QTimer>

TopBar::TopBar(QWidget* parent) : QToolBar(parent)
{
    setMovable(false);
    setFixedHeight(50);
    
    // Create a container widget for better layout control
    QWidget* container = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(20, 0, 20, 0);
    layout->setSpacing(20);
    
    // Left: Title
    QLabel* titleLabel = new QLabel("UAV Control System");
    QFont titleFont;
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #00a6ff;");
    
    // Center: Search bar
    searchBar = new QLineEdit();
    searchBar->setPlaceholderText("Search...");
    searchBar->setFixedWidth(300);
    searchBar->setStyleSheet(R"(
        QLineEdit {
            background-color: #252525;
            color: #e0e0e0;
            border: 1px solid #00a6ff;
            border-radius: 4px;
            padding: 5px 10px;
            font-size: 12px;
        }
        QLineEdit:focus {
            border: 2px solid #00a6ff;
        }
    )");
    
    // Right: Date/time and status
    QWidget* rightContainer = new QWidget();
    QHBoxLayout* rightLayout = new QHBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(15);
    
    dateTimeLabel = new QLabel();
    dateTimeLabel->setStyleSheet("color: #e0e0e0; font-size: 12px;");
    
    QLabel* statusLabel = new QLabel("● Online");
    statusLabel->setStyleSheet("color: #4CAF50; font-size: 12px;");
    
    rightLayout->addWidget(dateTimeLabel);
    rightLayout->addWidget(statusLabel);
    
    // Add to main layout
    layout->addWidget(titleLabel);
    layout->addStretch();
    layout->addWidget(searchBar);
    layout->addStretch();
    layout->addWidget(rightContainer);
    
    // Add the container to the toolbar
    addWidget(container);
    
    // Set styles
    setStyleSheet(R"(
        QToolBar {
            background-color: #1a1a1a;
            border-bottom: 1px solid #00a6ff;
            border-top: none;
            border-left: none;
            border-right: none;
        }
    )");
    
    // Setup timer for date/time updates
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &TopBar::updateDateTime);
    dateTimeTimer->start(1000);
    updateDateTime();
}

void TopBar::updateDateTime()
{
    QDateTime current = QDateTime::currentDateTime();
    dateTimeLabel->setText(current.toString("dd/MM/yyyy hh:mm:ss"));
} 