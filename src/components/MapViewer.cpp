#include "../../include/components/MapViewer.h"
#include "../../include/components/mapfunctions.h"
#include "../../include/components/mapbox.h"
#include "../../include/simulation/SimulationView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QDir>

// Custom WebEnginePage for debugging
DebugWebEnginePage::DebugWebEnginePage(QWebEngineProfile *profile, QObject *parent)
    : QWebEnginePage(profile, parent)
{
}

void DebugWebEnginePage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, 
                                                int lineNumber, const QString &sourceID) {
    QString levelStr;
    switch (level) {
        case InfoMessageLevel: levelStr = "INFO"; break;
        case WarningMessageLevel: levelStr = "WARNING"; break;
        case ErrorMessageLevel: levelStr = "ERROR"; break;
    }
    
    qDebug() << "JS:" << levelStr << message << "at line" << lineNumber << "in" << sourceID;
}

MapViewer::MapViewer(QWidget* parent) : QWidget(parent)
{
    // Create the UI components
    m_stackedWidget = new QStackedWidget(this);
    
    // Create a QWebEngineView with a custom page for debugging
    QWebEngineProfile* profile = new QWebEngineProfile(this);
    DebugWebEnginePage* page = new DebugWebEnginePage(profile, this);
    
    m_webView = new QWebEngineView(this);
    m_webView->setPage(page);
    
    // Create a QWebChannel to communicate with JavaScript
    QWebChannel* channel = new QWebChannel(this);
    channel->registerObject("qt_object", this);
    page->setWebChannel(channel);
    
    // Add the web view to the stacked widget
    m_stackedWidget->addWidget(m_webView);
    
    // Set up the layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_stackedWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Set up the file check timer
    m_fileCheckTimer = new QTimer(this);
    connect(m_fileCheckTimer, &QTimer::timeout, this, &MapViewer::checkForFileChanges);
    m_fileCheckTimer->start(1000); // Check every second
    
    // Create the MapFunctions instance and connect signals
    m_mapFunctions = new MapFunctions(m_webView, this);
    connect(m_mapFunctions, &MapFunctions::geometricShapeSaved, this, &MapViewer::geometricShapeSaved);
    connect(m_mapFunctions, &MapFunctions::droneAnimationCompleted, this, &MapViewer::droneAnimationCompleted);
    
    // Create the Mapbox instance and load the map
    m_mapbox = new Mapbox(m_webView, this);
    m_mapbox->loadMap();
    
    // Initial check for file changes
    QTimer::singleShot(500, this, &MapViewer::checkForFileChanges);
}

void MapViewer::setDronePositions(const QVector<QVector3D>& positions)
{
    // Forward to MapFunctions
    m_mapFunctions->setDronePositions(positions);
}

void MapViewer::updateDronePath(const QJsonObject& geojsonData)
{
    // Forward to MapFunctions
    m_mapFunctions->updateDronePath(geojsonData);
}

void MapViewer::setActiveDrone(const QString& droneName)
{
    // Forward to MapFunctions
    m_mapFunctions->setActiveDrone(droneName);
}

void MapViewer::saveGeometryData(const QString& geometryData)
{
    // Forward to MapFunctions
    m_mapFunctions->saveGeometryData(geometryData);
}

void MapViewer::updateGeometryData(const QString& geometryData)
{
    // Forward to MapFunctions
    m_mapFunctions->updateGeometryData(geometryData);
}

void MapViewer::saveGeometricShape(const QString& shapeData, const QString& shapeName)
{
    // Forward to MapFunctions
    m_mapFunctions->saveGeometricShape(shapeData, shapeName);
}

void MapViewer::loadGeometricShapes()
{
    // Forward to MapFunctions
    m_mapFunctions->loadGeometricShapes();
}

void MapViewer::checkForFileChanges()
{
    // Forward to MapFunctions
    m_mapFunctions->checkForFileChanges();
}

void MapViewer::deleteGeometricShape(const QString& shapeName)
{
    // Forward to MapFunctions
    m_mapFunctions->deleteGeometricShape(shapeName);
}

void MapViewer::clearDronePathsOnExit()
{
    // Forward to MapFunctions
    m_mapFunctions->clearDronePathsOnExit();
}



void MapViewer::startDroneAnimation()
{
    // Forward to MapFunctions
    m_mapFunctions->startDroneAnimation();
}



void MapViewer::confirmDroneTask(const QString& missionType, const QString& vehicle, const QString& prompt)
{
    // Forward to MapFunctions
    m_mapFunctions->confirmDroneTask(missionType, vehicle, prompt);
}

MapViewer::~MapViewer()
{
    // The MapFunctions destructor will handle clearing drone paths
    delete m_mapFunctions;
    delete m_mapbox;
}