#include "../../include/components/TopBar.h"
#include <QSizePolicy>
#include <QDateTime>
#include <QFont>
#include <QHBoxLayout>
#include <QPixmap>

TopBar::TopBar(QWidget* parent) : QToolBar(parent)
{
    setMovable(false);
    setMinimumHeight(60);
    setMaximumHeight(60);
    
    // Create a container widget for better layout control
    QWidget* container = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(10);
    
    // Left: Logo with icon
    logoLabel = new QLabel();
    // In a real app, you'd use a proper logo image
    logoLabel->setText("<span style='font-weight:bold; font-size:16pt; color:#0078d7;'>UAV</span> <span style='font-weight:normal; font-size:14pt;'>CONTROL</span>");
    logoLabel->setMinimumWidth(150);
    logoLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(logoLabel);
    
    // Add spacer
    layout->addStretch(1);
    
    // Center: Search with icon
    QWidget* searchContainer = new QWidget();
    QHBoxLayout* searchLayout = new QHBoxLayout(searchContainer);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(5);
    
    QLabel* searchIcon = new QLabel("🔍");
    searchIcon->setStyleSheet("font-size: 14pt;");
    
    searchBox = new QLineEdit();
    searchBox->setPlaceholderText("Search...");
    searchBox->setFixedWidth(300);
    searchBox->setFixedHeight(36);
    QFont searchFont = searchBox->font();
    searchFont.setPointSize(11);
    searchBox->setFont(searchFont);
    
    searchLayout->addWidget(searchIcon);
    searchLayout->addWidget(searchBox);
    
    layout->addWidget(searchContainer);
    
    // Add spacer
    layout->addStretch(1);
    
    // Right: User info, date/time, status with better styling
    QWidget* rightContainer = new QWidget();
    QHBoxLayout* rightLayout = new QHBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(15);
    
    userIcon = new QLabel("👤");
    QFont iconFont = userIcon->font();
    iconFont.setPointSize(14);
    userIcon->setFont(iconFont);
    userIcon->setStyleSheet("color: #0078d7;");
    
    dateTimeLabel = new QLabel();
    QFont dateTimeFont = dateTimeLabel->font();
    dateTimeFont.setPointSize(11);
    dateTimeLabel->setFont(dateTimeFont);
    
    statusLabel = new QLabel();
    statusLabel->setText("<span style='color:#4CAF50;'>●</span> <span style='color:#f0f0f0;'>Online</span>");
    QFont statusFont = statusLabel->font();
    statusFont.setPointSize(11);
    statusLabel->setFont(statusFont);
    
    rightLayout->addWidget(userIcon);
    rightLayout->addWidget(dateTimeLabel);
    rightLayout->addWidget(statusLabel);
    
    layout->addWidget(rightContainer);
    
    // Add the container to the toolbar
    addWidget(container);
    
    // Setup timer for date/time updates
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &TopBar::updateDateTime);
    dateTimeTimer->start(1000);
    updateDateTime();
}

TopBar::~TopBar()
{
    if (dateTimeTimer) {
        dateTimeTimer->stop();
    }
}

void TopBar::updateDateTime()
{
    QDateTime current = QDateTime::currentDateTime();
    dateTimeLabel->setText(current.toString("dd/MM/yyyy hh:mm:ss"));
} 