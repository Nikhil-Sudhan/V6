#ifndef RESPONSEDIALOG_H
#define RESPONSEDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonObject>

class ResponseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResponseDialog(QWidget* parent = nullptr);
    ~ResponseDialog();
    
    void setResponse(const QString& response, const QString& functionCalls);

private slots:
    void executeFunctions();
    void closeDialog();

private:
    QTextEdit* responseTextEdit;
    QTextEdit* functionsTextEdit;
    QPushButton* executeButton;
    QPushButton* closeButton;
    
    QJsonObject parsedFunctionCalls;
    bool hasFunctions;
};

#endif // RESPONSEDIALOG_H 