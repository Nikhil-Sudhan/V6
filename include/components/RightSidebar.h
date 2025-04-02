#ifndef RIGHTSIDEBAR_H
#define RIGHTSIDEBAR_H

#include <QToolBar>
#include <QAction>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QList>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <QMap>

class RightSidebar : public QObject {
    Q_OBJECT
public:
    explicit RightSidebar(QWidget* parent = nullptr);
    
    QToolBar* getToolBar() const { return rightToolBar; }
    QDockWidget* getDockWidget() const { return rightPanelDock; }
    
    bool isPanelVisible() const { return rightPanelDock->isVisible(); }
    
    // Update task list with new mission data
    void updateTaskList(const QString& missionType, const QString& vehicle, const QString& prompt);
    
    // Load mission list from database
    void loadMissionList();
    
    // Display mission details
    void displayMissionDetails(int missionId);
    
    // Show assign task dialog
    void showAssignTaskDialog();

signals:
    void visibilityChanged(bool visible);
    void assignTask(const QString& missionType, const QString& vehicle, const QString& prompt);

public slots:
    void handleButtonClick();

private:
    void createToolBar(QWidget* parent);
    void createDockWidget(QWidget* parent);
    void createPanel();
    
    QToolBar* rightToolBar;
    QDockWidget* rightPanelDock;
    QAction* taskDetailsAction;
};

#endif // RIGHTSIDEBAR_H