#include "../../include/dialogs/ResponseDialog.h"
#include <QJsonDocument>
#include <QMessageBox>
#include <QDebug>

ResponseDialog::ResponseDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("GeoJSON Data");
    setMinimumSize(600, 400);
    
    // Set dialog style
    setStyleSheet(R"(
        QDialog {
            background-color: #1a1a1a;
            border: 1px solid #00a6ff;
        }
        QLabel {
            color: #e0e0e0;
            font-size: 14px;
        }
        QTextEdit {
            background-color: #252525;
            color: #e0e0e0;
            border: 1px solid #00a6ff;
            border-radius: 4px;
            padding: 8px;
            font-family: monospace;
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
            min-width: 100px;
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
    )");
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // GeoJSON Data section
    QLabel* responseLabel = new QLabel("GeoJSON Data:", this);
    QFont titleFont = responseLabel->font();
    titleFont.setBold(true);
    responseLabel->setFont(titleFont);
    
    responseTextEdit = new QTextEdit(this);
    responseTextEdit->setReadOnly(true);
    
    // Button
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    closeButton = new QPushButton("Close", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    
    // Add to main layout
    mainLayout->addWidget(responseLabel);
    mainLayout->addWidget(responseTextEdit, 1);
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(closeButton, &QPushButton::clicked, this, &ResponseDialog::closeDialog);
}

ResponseDialog::~ResponseDialog()
{
}

void ResponseDialog::setResponse(const QString& response, const QString&)
{
    // Set response text (GeoJSON data)
    responseTextEdit->setText(response);
}

void ResponseDialog::closeDialog()
{
    accept();
}