#ifndef LEFTSIDEBAR_H
#define LEFTSIDEBAR_H

#include <QToolBar>
#include <QAction>
#include <QStackedWidget>
#include <QDockWidget>
#include <QObject>

// Forward declarations
class MissionControl;
class VehicleConfiguration;
class Settings;

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
    QAction* settingsAction;
    
    // Panel widgets
    MissionControl* missionControlPanel;
    VehicleConfiguration* vehicleConfigPanel;
    Settings* settingsPanel;
    
    int currentPanelIndex;
};

#endif // LEFTSIDEBAR_H