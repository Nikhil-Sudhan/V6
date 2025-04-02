#ifndef MAPBOX_H
#define MAPBOX_H

#include <QObject>
#include <QWebEngineView>
#include <QString>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QWebChannel>
#include <QDebug>

class Mapbox : public QObject {
    Q_OBJECT
public:
    explicit Mapbox(QWebEngineView* webView, QObject* parent = nullptr);
    ~Mapbox();

    void loadMap();
    QString getMapboxToken() const { return m_mapboxToken; }

public slots:
    void saveGeometricShape(const QString& geoJson, const QString& shapeName);
    void updateDronePath(const QString& geoJson);
    void updateGeometricShapes(const QString& geoJson);
    void moveDroneAlongPath(const QJsonArray& coordinates, int currentIndex);

signals:
    void geometricShapeSaved(const QString& shapeName);
    void dronePathUpdated();

private:
    QWebEngineView* m_webView;
    QString m_lastGeojsonPath;
    QString m_mapboxToken;
};

#endif // MAPBOX_H
