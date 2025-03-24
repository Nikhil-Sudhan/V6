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

class SimulationView;

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
    
    enum ViewMode {
        MapMode,
        SimulationMode
    };
    
public slots:
    void toggleView();
    void setDronePositions(const QVector<QVector3D>& positions);
    void updateDronePath(const QJsonObject& geojsonData);
    void saveGeometryData(const QString& geometryData);
    void updateGeometryData(const QString& geometryData);
    void checkForFileChanges();
    
private:
    QStackedWidget* m_stackedWidget;
    QWebEngineView* m_webView;
    SimulationView* m_simulationView;
    QPushButton* m_toggleButton;
    ViewMode m_currentMode;
    QString m_lastGeojsonHash;
    QDateTime m_lastFileModified;
    
    void setupUI();
    void loadMap();
};

#endif // MAPVIEWER_H