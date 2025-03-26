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
#include <QWebChannel>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QCryptographicHash>
#include <QMap> // Added include for QMap
#include <QRandomGenerator>
#include <QColor>

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
    
signals:
    void geometricShapeSaved(const QString& shapeName);
    
private:
    QStackedWidget* m_stackedWidget;
    QWebEngineView* m_webView;
    QString m_lastGeojsonHash;
    QString m_lastGeojsonPath;
    QDateTime m_lastFileModified;
    QString m_activeDroneName = "Atlas"; // Default drone name
    QMap<QString, QString> m_dronePathColors; // Store colors for each drone
    QTimer* m_fileCheckTimer; // Timer to periodically check for file changes
    QDateTime m_lastShapesFileModified; // Last modified time for shapes file
    QDateTime m_lastDronePathsFileModified; // Last modified time for drone paths file
    void setupUI();
    void loadMap();
};

#endif // MAPVIEWER_H