#ifndef MISSIONCONTROL_H
#define MISSIONCONTROL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QFrame>
#include <QFont>
#include <QMessageBox>
#include "../../../include/api/ChatGPTClient.h"

class MissionControl : public QWidget {
    Q_OBJECT
public:
    explicit MissionControl(QWidget* parent = nullptr);
    
signals:
    void missionAssigned(const QString& missionType, const QString& vehicle, const QString& prompt);

public slots:
    void handleAssignTask();
    void handleChatGPTResponse(int missionId, const QString& response, const QString& functions);
    void handleApiError(const QString& errorMessage);

private:
    void setupUI();
    void setupConnections();
    
    // Mission panel controls
    QComboBox* missionTypeCombo;
    QComboBox* vehicleCombo;
    QTextEdit* promptTextEdit;
    QPushButton* assignTaskButton;
};

#endif // MISSIONCONTROL_H
