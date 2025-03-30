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
#include <QJsonArray>  // Added include for QJsonArray
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
#include <QMessageBox>

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
    void startDroneAnimation(); 
    void confirmDroneTask(const QString& missionType, const QString& vehicle, const QString& prompt); 
    
signals:
    void geometricShapeSaved(const QString& shapeName);
    void droneAnimationCompleted(); 
    
private slots:
    void updateDronePosition(); 
    
private:
    QStackedWidget* m_stackedWidget;
    QWebEngineView* m_webView;
    QString m_lastGeojsonHash;
    QString m_lastGeojsonPath;
    QDateTime m_lastFileModified;
    QString m_activeDroneName = "Atlas"; 
    QMap<QString, QString> m_dronePathColors; 
    QTimer* m_fileCheckTimer; 
    QDateTime m_lastShapesFileModified; 
    QDateTime m_lastDronePathsFileModified; 
    
    // New properties for drone animation
    QTimer* m_animationTimer;
    QJsonArray m_currentPath;
    int m_currentPathIndex;
    double m_animationSpeed; 
    bool m_isAnimating;
    
    void setupUI();
    void loadMap();

};

#endif // MAPVIEWER_H