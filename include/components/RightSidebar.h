#ifndef RIGHTSIDEBAR_H
#define RIGHTSIDEBAR_H

#include <QToolBar>
#include <QAction>
#include <QDockWidget>

class RightSidebar : public QObject {
    Q_OBJECT
public:
    explicit RightSidebar(QWidget* parent = nullptr);
    
    QToolBar* getToolBar() const { return rightToolBar; }
    QDockWidget* getDockWidget() const { return rightPanelDock; }
    
    bool isPanelVisible() const { return rightPanelDock->isVisible(); }

signals:
    void visibilityChanged(bool visible);

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