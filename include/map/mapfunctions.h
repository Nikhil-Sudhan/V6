#ifndef MAPFUNCTIONS_H
#define MAPFUNCTIONS_H

#include <QWidget>
#include <QWebEngineView>
#include <QVector3D>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QCryptographicHash>
#include <QMap>
#include <QRandomGenerator>
#include <QColor>
#include <QMessageBox>
#include <QTimer>
#include <QTextStream>

class MapFunctions : public QObject {
    Q_OBJECT
public:
    explicit MapFunctions(QWebEngineView* webView, QObject* parent = nullptr);
    ~MapFunctions();
    
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
    void confirmDroneTask(const QString& missionType, const QString& vehicle, const QString& prompt); 
    
signals:
    void geometricShapeSaved(const QString& shapeName);
    void droneAnimationCompleted(); 
    
private slots:
    void updateDronePosition(); 
    
private:
    QWebEngineView* m_webView;
    QString m_lastGeojsonHash;
    QString m_lastGeojsonPath;
    QDateTime m_lastFileModified;
    QString m_activeDroneName = "Atlas"; 
    QMap<QString, QString> m_dronePathColors; 
    QDateTime m_lastShapesFileModified; 
    QDateTime m_lastDronePathsFileModified; 
    
    // Properties for drone animation
    QTimer* m_animationTimer;
    QJsonArray m_currentPath;
    int m_currentPathIndex;
    double m_animationSpeed; 
    bool m_isAnimating;
};

#endif // MAPFUNCTIONS_H
