#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QStackedWidget>

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
    void handleAssignTask();
    void handleLeftPanelChanged(int index);

private:
    void setupMainArea();
    void applyStyles();

private:
    // Main widgets
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    
    // Components
    TopBar *topBar;
    LeftSidebar *leftSidebar;
    RightSidebar *rightSidebar;
    MapViewer *mapViewer;
};

#endif // MAINWINDOW_H 