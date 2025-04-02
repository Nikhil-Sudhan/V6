#include "../../../include/components/LeftsideBar/settings.h"

Settings::Settings(QWidget* parent) : QWidget(parent)
{
    setupUI();
}

void Settings::setupUI()
{
    // Create Settings Panel
    QVBoxLayout* settingsLayout = new QVBoxLayout(this);
    settingsLayout->setContentsMargins(20, 20, 20, 20);
    settingsLayout->setSpacing(20);
    
    // Add title with icon
    QWidget* titleContainer = new QWidget();
    QHBoxLayout* titleLayout = new QHBoxLayout(titleContainer);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* titleIcon = new QLabel("⚙️");
    QFont iconFont = titleIcon->font();
    iconFont.setPointSize(18);
    titleIcon->setFont(iconFont);
    
    QLabel* titleLabel = new QLabel("Settings");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    
    titleLayout->addWidget(titleIcon);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    
    settingsLayout->addWidget(titleContainer);
    
    // Add separator
    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("background-color: #3e3e42;");
    settingsLayout->addWidget(separator);
    
    // Add settings content
    QLabel* settingsLabel = new QLabel("Settings Panel\n\n- System Settings\n- User Preferences\n- Security Settings\n- Updates");
    settingsLabel->setStyleSheet("color: #e0e0e0; font-size: 14px;");
    settingsLayout->addWidget(settingsLabel);
    
    // Add stretch to push everything to the top
    settingsLayout->addStretch();
    
    // Apply styling
    setStyleSheet(R"(
        QWidget {
            background-color: #1a1a1a;
        }
        QLabel {
            color: #e0e0e0;
        }
    )");
}