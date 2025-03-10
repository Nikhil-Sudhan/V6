#ifndef TOPBAR_H
#define TOPBAR_H

#include <QToolBar>
#include <QLabel>
#include <QTimer>
#include <QLineEdit>

class TopBar : public QToolBar {
    Q_OBJECT
public:
    explicit TopBar(QWidget* parent = nullptr);
    ~TopBar() { if (dateTimeTimer) dateTimeTimer->stop(); }

private slots:
    void updateDateTime();

private:
    QLabel* dateTimeLabel;
    QTimer* dateTimeTimer;
    QLineEdit* searchBar;
};

#endif // TOPBAR_H 