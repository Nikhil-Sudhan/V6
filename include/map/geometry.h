#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QObject>
#include <QWebEngineView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDebug>

class Geometry : public QObject
{
    Q_OBJECT
    
public:
    explicit Geometry(QWebEngineView* webView, QObject* parent = nullptr);
    ~Geometry();
    
    void saveGeometryData(const QString& geometryData);
    void updateGeometryData(const QString& geometryData);
    void saveGeometricShape(const QString& shapeData, const QString& shapeName);
    void loadGeometricShapes();
    void deleteGeometricShape(const QString& shapeName);
    void clearAllGeometryOnExit();
    
signals:
    void geometricShapeSaved(const QString& shapeName);
    
private:
    QWebEngineView* m_webView;
    QDateTime m_lastShapesFileModified;
};

#endif // GEOMETRY_H