#ifndef RESPONSEDIALOG_H
#define RESPONSEDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ResponseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResponseDialog(QWidget* parent = nullptr);
    ~ResponseDialog();
    
    void setResponse(const QString& response, const QString& unused);

private slots:
    void closeDialog();

private:
    QTextEdit* responseTextEdit;
    QPushButton* closeButton;
};

#endif // RESPONSEDIALOG_H