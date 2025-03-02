#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QToolBar>
#include <QStackedWidget>
#include <QDockWidget>
#include <QLineEdit>
#include <QTimer>
#include <QDateTime>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleLeftSidebarButton(int index);
    void handleRightSidebarButton();
    void updateDateTime();

private:
    void createTopBar();
    void createLeftSideBar();
    void createRightSideBar();
    void createLeftPanels();
    void createRightPanel();
    void setupMainArea();
    void applyStyles();

private:
    // Main widgets
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;

    // Top bar widgets
    QToolBar *topBar;
    QLabel *logoLabel;
    QLineEdit *searchBox;
    QLabel *userIcon;
    QLabel *dateTimeLabel;
    QLabel *statusLabel;
    QTimer *dateTimeTimer;

    // Left sidebar
    QToolBar *leftToolBar;
    QDockWidget *leftPanelDock;
    QStackedWidget *leftStackedWidget;
    QAction *missionAction;
    QAction *configAction;
    QAction *simulationAction;
    QAction *settingsAction;
    int currentLeftPanelIndex;

    // Right sidebar
    QToolBar *rightToolBar;
    QDockWidget *rightPanelDock;
    QAction *taskDetailsAction;
    bool isRightPanelVisible;
};

#endif // MAINWINDOW_H