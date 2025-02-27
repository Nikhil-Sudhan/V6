#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QTreeView>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QToolBar>
#include <QStackedWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleSidebarButton(int index);

private:
    void createMenuBar();
    void createSideToolBar();
    void createStatusBar();
    void setupMainArea();
    void applyStyles();
    void createSidePanels();

private:
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QTextEdit *editor;
    QToolBar *sideToolBar;
    QDockWidget *sidePanelDock;
    QStackedWidget *stackedWidget;
    QStatusBar *bottomBar;
    QAction *missionAction;
    QAction *configAction;
    QAction *simulationAction;
    QAction *settingsAction;
    int currentPanelIndex;
};

#endif // MAINWINDOW_H 