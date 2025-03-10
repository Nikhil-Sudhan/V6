#ifndef DRONEFUNCTIONS_H
#define DRONEFUNCTIONS_H

#include <QString>

class DroneFunctions {
public:
    static bool takeoff(double x, double y, double z, const QString& droneName);
    static bool move(double x, double y, double z, const QString& droneName);
    static bool land(double x, double y, double z, const QString& droneName);
    static bool arm(const QString& droneName);
    static bool disarm(const QString& droneName);
};

#endif // DRONEFUNCTIONS_H 