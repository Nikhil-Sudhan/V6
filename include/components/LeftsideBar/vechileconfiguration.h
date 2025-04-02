#ifndef VEHICLECONFIGURATION_H
#define VEHICLECONFIGURATION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>
#include <QFont>
#include "../../../include/components/VehicleInfoWidget.h"

class VehicleConfiguration : public QWidget {
    Q_OBJECT
public:
    explicit VehicleConfiguration(QWidget* parent = nullptr);
    
private:
    void setupUI();
};

#endif // VEHICLECONFIGURATION_H
