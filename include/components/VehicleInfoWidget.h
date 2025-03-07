#ifndef VEHICLEINFOWIDGET_H
#define VEHICLEINFOWIDGET_H

#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class VehicleInfoWidget : public QFrame {
    Q_OBJECT
public:
    VehicleInfoWidget(const QString& name, const QString& type, const QString& status, QWidget* parent = nullptr);
    void setExpanded(bool expanded);
    
private:
    QVBoxLayout* mainLayout;
    QWidget* detailsWidget;
    QLabel* statusLabel;
    QPushButton* expandButton;
    bool isExpanded = false;
};

#endif // VEHICLEINFOWIDGET_H 