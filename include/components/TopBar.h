#ifndef TOPBAR_H
#define TOPBAR_H

#include <QToolBar>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>

class TopBar : public QToolBar {
    Q_OBJECT
public:
    explicit TopBar(QWidget* parent = nullptr);
    ~TopBar();

private slots:
    void updateDateTime();

private:
    QLabel* logoLabel;
    QLineEdit* searchBox;
    QLabel* userIcon;
    QLabel* dateTimeLabel;
    QLabel* statusLabel;
    QTimer* dateTimeTimer;
};

#endif // TOPBAR_H 