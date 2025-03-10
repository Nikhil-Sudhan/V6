#include "../../include/dialogs/ResponseDialog.h"
#include "../../include/drone/DroneFunctions.h"
#include <QJsonDocument>
#include <QMessageBox>
#include <QDebug>

ResponseDialog::ResponseDialog(QWidget* parent)
    : QDialog(parent), hasFunctions(false)
{
    setWindowTitle("ChatGPT Response");
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
        QPushButton:disabled {
            background-color: #252525;
            color: #666666;
            border: 1px solid #666666;
        }
    )");
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Response section
    QLabel* responseLabel = new QLabel("Response:", this);
    QFont titleFont = responseLabel->font();
    titleFont.setBold(true);
    responseLabel->setFont(titleFont);
    
    responseTextEdit = new QTextEdit(this);
    responseTextEdit->setReadOnly(true);
    
    // Functions section
    QLabel* functionsLabel = new QLabel("Functions:", this);
    functionsLabel->setFont(titleFont);
    
    functionsTextEdit = new QTextEdit(this);
    functionsTextEdit->setReadOnly(true);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    executeButton = new QPushButton("Execute Functions", this);
    closeButton = new QPushButton("Close", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(executeButton);
    buttonLayout->addWidget(closeButton);
    
    // Add to main layout
    mainLayout->addWidget(responseLabel);
    mainLayout->addWidget(responseTextEdit, 3);
    mainLayout->addWidget(functionsLabel);
    mainLayout->addWidget(functionsTextEdit, 2);
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(executeButton, &QPushButton::clicked, this, &ResponseDialog::executeFunctions);
    connect(closeButton, &QPushButton::clicked, this, &ResponseDialog::closeDialog);
    
    // Disable execute button by default
    executeButton->setEnabled(false);
}

ResponseDialog::~ResponseDialog()
{
}

void ResponseDialog::setResponse(const QString& response, const QString& functionCalls)
{
    // Set response text
    responseTextEdit->setText(response);
    
    // Parse and set function calls
    hasFunctions = false;
    if (!functionCalls.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson(functionCalls.toUtf8());
        if (!doc.isNull() && doc.isObject()) {
            parsedFunctionCalls = doc.object();
            
            // Format function calls for display
            QString functionName = parsedFunctionCalls["name"].toString();
            QJsonObject arguments = QJsonDocument::fromJson(
                parsedFunctionCalls["arguments"].toString().toUtf8()).object();
            
            QString formattedFunctions = QString("Function: %1\n\nArguments:\n").arg(functionName);
            
            for (auto it = arguments.begin(); it != arguments.end(); ++it) {
                formattedFunctions += QString("%1: %2\n")
                    .arg(it.key())
                    .arg(it.value().toVariant().toString());
            }
            
            functionsTextEdit->setText(formattedFunctions);
            hasFunctions = true;
            executeButton->setEnabled(true);
        } else {
            functionsTextEdit->setText("No functions to execute.");
        }
    } else {
        functionsTextEdit->setText("No functions to execute.");
    }
}

void ResponseDialog::executeFunctions()
{
    if (!hasFunctions) {
        QMessageBox::information(this, "No Functions", "There are no functions to execute.");
        return;
    }
    
    QString functionName = parsedFunctionCalls["name"].toString();
    QJsonObject arguments = QJsonDocument::fromJson(
        parsedFunctionCalls["arguments"].toString().toUtf8()).object();
    
    // Execute the function based on its name
    if (functionName == "takeoff") {
        double x = arguments["x"].toDouble();
        double y = arguments["y"].toDouble();
        double z = arguments["altitude"].toDouble();
        QString drone = arguments["drone"].toString();
        if (DroneFunctions::takeoff(x, y, z, drone)) {
            QMessageBox::information(this, "Function Executed", 
                QString("Executing takeoff for drone %1 to position (%2, %3, %4)")
                .arg(drone).arg(x).arg(y).arg(z));
        }
    } 
    else if (functionName == "move") {
        double x = arguments["x"].toDouble();
        double y = arguments["y"].toDouble();
        double z = arguments["z"].toDouble();
        QString drone = arguments["drone"].toString();
        if (DroneFunctions::move(x, y, z, drone)) {
            QMessageBox::information(this, "Function Executed", 
                QString("Moving drone %1 to position (%2, %3, %4)")
                .arg(drone).arg(x).arg(y).arg(z));
        }
    }
    else if (functionName == "land") {
        double x = arguments["x"].toDouble();
        double y = arguments["y"].toDouble();
        double z = arguments["z"].toDouble();
        QString drone = arguments["drone"].toString();
        if (DroneFunctions::land(x, y, z, drone)) {
            QMessageBox::information(this, "Function Executed", 
                QString("Landing drone %1 at position (%2, %3, %4)")
                .arg(drone).arg(x).arg(y).arg(z));
        }
    }
    else if (functionName == "arm") {
        QString drone = arguments["drone"].toString();
        if (DroneFunctions::arm(drone)) {
            QMessageBox::information(this, "Function Executed", 
                QString("Armed drone %1").arg(drone));
        }
    }
    else if (functionName == "disarm") {
        QString drone = arguments["drone"].toString();
        if (DroneFunctions::disarm(drone)) {
            QMessageBox::information(this, "Function Executed", 
                QString("Disarmed drone %1").arg(drone));
        }
    }
    else {
        QMessageBox::warning(this, "Unknown Function", 
            QString("Unknown function: %1").arg(functionName));
    }
}

void ResponseDialog::closeDialog()
{
    accept();
} 