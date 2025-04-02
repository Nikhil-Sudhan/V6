#include "../../../include/components/LeftsideBar/vechileconfiguration.h"

VehicleConfiguration::VehicleConfiguration(QWidget* parent) : QWidget(parent)
{
    setupUI();
}

void VehicleConfiguration::setupUI()
{
    setStyleSheet("background-color: #1a1a1a;");
    QVBoxLayout* configLayout = new QVBoxLayout(this);
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
}