#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

class Settings : public QWidget {
    Q_OBJECT
public:
    explicit Settings(QWidget* parent = nullptr);
    
private:
    void setupUI();
};

#endif // SETTINGS_H
