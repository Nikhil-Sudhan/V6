#include "../../include/dialogs/ResponseDialog.h"
#include <QJsonDocument>
#include <QMessageBox>
#include <QDebug>

ResponseDialog::ResponseDialog(QWidget* parent)
    : QDialog(parent), hasFunctions(false)
{
    setWindowTitle("ChatGPT Response");
    setMinimumSize(600, 400);
    
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
        double altitude = arguments["altitude"].toDouble();
        QMessageBox::information(this, "Function Executed", 
            QString("Executing takeoff to altitude: %1 meters").arg(altitude));
    } 
    else if (functionName == "land") {
        QMessageBox::information(this, "Function Executed", "Executing land command");
    } 
    else if (functionName == "turn") {
        QString direction = arguments["direction"].toString();
        double degrees = arguments["degrees"].toDouble();
        QMessageBox::information(this, "Function Executed", 
            QString("Executing turn %1 %2 degrees").arg(direction).arg(degrees));
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