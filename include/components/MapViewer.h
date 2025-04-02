#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QStackedWidget>
#include <QPushButton>
#include <QVector3D>
#include <QJsonObject>
#include <QJsonArray>
#include <QWebChannel>
#include <QTimer>

// Forward declarations
class MapFunctions;
class Mapbox;
class Geometry;

// Custom WebEnginePage for debugging
class DebugWebEnginePage : public QWebEnginePage {
    Q_OBJECT
public:
    DebugWebEnginePage(QWebEngineProfile *profile, QObject *parent = nullptr);
    
protected:
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, 
                                 int lineNumber, const QString &sourceID) override;
};

class MapViewer : public QWidget {
    Q_OBJECT
public:
    explicit MapViewer(QWidget* parent = nullptr);
    ~MapViewer();
    
public slots:
    // Proxy slots that forward to MapFunctions
    void setDronePositions(const QVector<QVector3D>& positions);
    void updateDronePath(const QJsonObject& geojsonData);
    void saveGeometryData(const QString& geometryData);
    void updateGeometryData(const QString& geometryData);
    void checkForFileChanges();
    void setActiveDrone(const QString& droneName);
    void saveGeometricShape(const QString& shapeData, const QString& shapeName);
    void loadGeometricShapes();
    void deleteGeometricShape(const QString& shapeName);
    void clearDronePathsOnExit();
    void confirmDroneTask(const QString& missionType, const QString& vehicle, const QString& prompt); 
    
signals:
    void geometricShapeSaved(const QString& shapeName);
    void droneAnimationCompleted(); 
    
private:
    QStackedWidget* m_stackedWidget;
    QWebEngineView* m_webView;
    QString m_lastGeojsonPath;
    QTimer* m_fileCheckTimer; 
    
    // Map functions handler
    MapFunctions* m_mapFunctions;
    
    // Mapbox handler
    Mapbox* m_mapbox;
    
    // Geometry handler
    Geometry* m_geometry;
};

#endif // MAPVIEWER_H