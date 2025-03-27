#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>

// Forward declarations
class TopBar;
class LeftSidebar;
class RightSidebar;
class MapViewer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleLeftPanelChanged(int index);
    void handleAssignTask();
    void handleDroneAnimationCompleted();

private:
    void setupMainArea();
    void applyStyles();

    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    
    TopBar* topBar;
    LeftSidebar* leftSidebar;
    RightSidebar* rightSidebar;
    MapViewer* mapViewer;
};

#endif // MAINWINDOW_H 