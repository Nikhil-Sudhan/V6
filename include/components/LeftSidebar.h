#ifndef LEFTSIDEBAR_H
#define LEFTSIDEBAR_H

#include <QToolBar>
#include <QAction>
#include <QStackedWidget>
#include <QDockWidget>
#include <QLabel>
#include <QHBoxLayout>

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

public slots:
    void handleButtonClick(int index);
    void setTitle(const QString& title);

private:
    void createToolBar(QWidget* parent);
    void createDockWidget(QWidget* parent);
    void createPanels();
    
    QToolBar* leftToolBar;
    QDockWidget* leftPanelDock;
    QStackedWidget* leftStackedWidget;
    
    // Panel actions
    QAction* missionAction;
    QAction* configAction;
    QAction* simulationAction;
    QAction* settingsAction;
    
    int currentPanelIndex;
};

#endif // LEFTSIDEBAR_H 