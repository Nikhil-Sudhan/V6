#ifndef MAPBOX_H
#define MAPBOX_H

#include <QObject>
#include <QWebEngineView>
#include <QString>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

class Mapbox : public QObject {
    Q_OBJECT
public:
    explicit Mapbox(QWebEngineView* webView, QObject* parent = nullptr);
    ~Mapbox();

    void loadMap();
    QString getMapboxToken() const { return m_mapboxToken; }

private:
    QWebEngineView* m_webView;
    QString m_lastGeojsonPath;
    QString m_mapboxToken;
};

#endif // MAPBOX_H
