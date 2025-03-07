#include "../../include/components/VehicleInfoWidget.h"
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFont>

VehicleInfoWidget::VehicleInfoWidget(const QString& name, const QString& type, const QString& status, QWidget* parent)
    : QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    // Header widget
    QWidget* headerWidget = new QWidget;
    headerWidget->setStyleSheet("background-color: #252526;"); // Ensure background is colored
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(10, 10, 10, 10);

    // Left side - Name and type
    QVBoxLayout* nameLayout = new QVBoxLayout();
    QLabel* nameLabel = new QLabel(name);
    QFont nameFont = nameLabel->font();
    nameFont.setBold(true);
    nameFont.setPointSize(12);
    nameLabel->setFont(nameFont);
    nameLabel->setStyleSheet("color: #d4d4d4;");
    
    QLabel* typeLabel = new QLabel(type);
    typeLabel->setStyleSheet("color: #888888;");
    
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(typeLabel);
    headerLayout->addLayout(nameLayout);

    // Right side - Status and expand button
    QHBoxLayout* statusLayout = new QHBoxLayout();
    statusLabel = new QLabel(status);
    QString statusColor = status == "Online" ? "#4CAF50" : 
                         status == "Standby" ? "#FFA500" : "#F44336";
    statusLabel->setStyleSheet(QString("color: %1; font-weight: bold;").arg(statusColor));
    
    expandButton = new QPushButton("▼");
    expandButton->setFixedSize(30, 30);
    expandButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            color: #d4d4d4;
        }
        QPushButton:hover {
            color: #007acc;
        }
    )");
    
    statusLayout->addWidget(statusLabel);
    statusLayout->addWidget(expandButton);
    headerLayout->addLayout(statusLayout);

    mainLayout->addWidget(headerWidget);

    // Details widget (initially hidden)
    detailsWidget = new QWidget;
    detailsWidget->setStyleSheet("background-color: #252526;"); // Ensure background is colored
    QGridLayout* detailsLayout = new QGridLayout(detailsWidget);
    detailsLayout->setSpacing(15);
    detailsLayout->setContentsMargins(15, 15, 15, 15);

    // Add vehicle details with example values
    int row = 0;
    auto addDetail = [&](const QString& label, const QString& value) {
        QLabel* labelWidget = new QLabel(label);
        labelWidget->setStyleSheet("color: #d4d4d4;");
        detailsLayout->addWidget(labelWidget, row, 0);
        
        QLabel* valueLabel = new QLabel(value);
        valueLabel->setStyleSheet("color: #007acc;");
        detailsLayout->addWidget(valueLabel, row, 1);
        row++;
    };

    addDetail("Battery", "85%");
    addDetail("Latitude", "40.7128° N");
    addDetail("Longitude", "74.0060° W");
    addDetail("Altitude", "120m");
    addDetail("Barometer", "1013.25 hPa");
    addDetail("Signal Strength", "-65 dBm");

    // Add calibrate button
    QPushButton* calibrateBtn = new QPushButton("Calibrate");
    calibrateBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #0e639c;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #1177bb;
        }
    )");
    detailsLayout->addWidget(calibrateBtn, row, 0, 1, 2, Qt::AlignRight);

    detailsWidget->hide();
    mainLayout->addWidget(detailsWidget);

    connect(expandButton, &QPushButton::clicked, this, [this]() {
        isExpanded = !isExpanded;
        setExpanded(isExpanded);
    });

    setStyleSheet(R"(
        VehicleInfoWidget {
            background-color: #252526;
            border: 1px solid #454545;
            border-radius: 4px;
            margin: 5px;
        }
    )");
}

void VehicleInfoWidget::setExpanded(bool expanded) {
    detailsWidget->setVisible(expanded);
    expandButton->setText(expanded ? "▲" : "▼");
} 