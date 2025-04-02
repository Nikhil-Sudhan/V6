#ifndef TASKDETAILS_H
#define TASKDETAILS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QGridLayout>
#include <QMap>
#include <QDateTime>
#include "../../database/DatabaseManager.h"

class TaskDetails : public QWidget {
    Q_OBJECT
public:
    explicit TaskDetails(QWidget* parent = nullptr);
    
    // Load mission list from database
    void loadMissionList();
    
    // Display mission details
    void displayMissionDetails(int missionId);
    
    // Update task list with new mission data
    void updateTaskList(const QString& missionType, const QString& vehicle, const QString& prompt);
    
    // Update asset data
    void updateAssetData(int missionId);

signals:
    void assignTask(const QString& missionType, const QString& vehicle, const QString& prompt);

public slots:
    void showAssignTaskDialog();
    void handleMissionItemClicked(QListWidgetItem* item);
    void toggleTaskDetails();
    void toggleAssetData();
    void handleStartPauseClicked();
    void handleCancelClicked();

private:
    void setupUI();
    void setupConnections();
    void addTaskItem(const QString& taskName, const QString& status, const QString& time);
    
    // Mission list widget
    QListWidget* missionListWidget;
    
    // Mission details widgets
    QLabel* userNameLabel;
    QLabel* missionTitleLabel;
    QLabel* missionTypeLabel;
    QPushButton* startPauseButton;
    QPushButton* cancelButton;
    
    // Task details widgets
    QWidget* taskDetailsWidget;
    QLabel* taskStatusLabel;
    QLabel* taskTimeLabel;
    QLabel* taskAssetLabel;
    QLabel* taskObjectiveLabel;
    QLabel* taskCollapseIcon;
    
    // Asset data widgets
    QWidget* assetDataWidget;
    QLabel* altitudeMSLValue;
    QLabel* altitudeAGLValue;
    QLabel* headingValue;
    QLabel* speedValue;
    QLabel* locationValue;
    QLabel* assetCollapseIcon;
    
    // Task list components
    QVBoxLayout* taskListLayout;
    QList<QWidget*> taskItems;
    
    // Collapsible sections
    bool taskDetailsExpanded;
    bool assetDataExpanded;
    
    // Current mission ID
    int currentMissionId;
    
    // Map to store mission IDs by list item
    QMap<int, int> itemToMissionId;
};

#endif // TASKDETAILS_H
