#ifndef LEFTSIDEBAR_H
#define LEFTSIDEBAR_H

#include <QToolBar>
#include <QAction>
#include <QStackedWidget>
#include <QDockWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>

class LeftSidebar : public QObject {
    Q_OBJECT
public:
    explicit LeftSidebar(QWidget* parent = nullptr);
    
    QToolBar* getToolBar() const { return leftToolBar; }
    QDockWidget* getDockWidget() const { return leftPanelDock; }
    QStackedWidget* getStackedWidget() const { return leftStackedWidget; }
    
    int getCurrentPanelIndex() const { return currentPanelIndex; }

signals:
    void panelChanged(int index);
    void missionAssigned(const QString& missionType, const QString& vehicle, const QString& prompt);

public slots:
    void handleButtonClick(int index);
    void setTitle(const QString& title);
    void handleAssignTask();
    void handleChatGPTResponse(int missionId, const QString& response, const QString& functions);
    void handleApiError(const QString& errorMessage);

private:
    void createToolBar(QWidget* parent);
    void createDockWidget(QWidget* parent);
    void createPanels();
    void setupConnections();
    
    QToolBar* leftToolBar;
    QDockWidget* leftPanelDock;
    QStackedWidget* leftStackedWidget;
    
    // Panel actions
    QAction* missionAction;
    QAction* configAction;
    QAction* simulationAction;
    QAction* settingsAction;
    
    // Mission panel controls
    QComboBox* missionTypeCombo;
    QComboBox* vehicleCombo;
    QTextEdit* promptTextEdit;
    QPushButton* assignTaskButton;
    
    int currentPanelIndex;
};

#endif // LEFTSIDEBAR_H 