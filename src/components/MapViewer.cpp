#include "../../include/components/MapViewer.h"
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

MapViewer::MapViewer(QWidget* parent) : QWidget(parent), m_currentMode(MapMode)
{
    setupUI();
    loadMap();
}

void MapViewer::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create stacked widget to hold map and simulation views
    m_stackedWidget = new QStackedWidget(this);
    
    // Create map view
    m_webView = new QWebEngineView(this);
    m_stackedWidget->addWidget(m_webView);
    
    // Create simulation view
    m_simulationView = new SimulationView(this);
    m_stackedWidget->addWidget(m_simulationView);
    
    // Create toggle button
    m_toggleButton = new QPushButton(this);
    m_toggleButton->setText("Switch to Simulation");
    m_toggleButton->setStyleSheet(R"(
        QPushButton {
            background-color: #252525;
            color: #00a6ff;
            border: 1px solid #00a6ff;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
            position: absolute;
            top: 10px;
            right: 10px;
        }
        QPushButton:hover {
            background-color: #00a6ff;
            color: #ffffff;
        }
    )");
    connect(m_toggleButton, &QPushButton::clicked, this, &MapViewer::toggleView);
    
    // Add stacked widget to layout
    mainLayout->addWidget(m_stackedWidget);
    
    // Position toggle button in top-right corner
    m_toggleButton->setParent(this);
    m_toggleButton->move(width() - m_toggleButton->width() - 20, 20);
    m_toggleButton->raise();
    
    // Set initial view
    m_stackedWidget->setCurrentIndex(MapMode);
}

void MapViewer::loadMap()
{
    // Get GeoJSON data
    QString appPath = QCoreApplication::applicationDirPath();
    QString geojsonPath = appPath + "/drone_geojson/Atlas_path.geojson";
    QFile file(geojsonPath);
    QString geojsonStr = "{}";
    
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        geojsonStr = doc.toJson(QJsonDocument::Compact);
        file.close();
        qDebug() << "GeoJSON loaded successfully";
    } else {
        qDebug() << "Failed to open file:" << geojsonPath;
    }

    const QString mapboxToken = "pk.eyJ1Ijoibmlja3lqMTIxIiwiYSI6ImNtN3N3eHFtcTB1MTkya3M4Mnc0dmQxanAifQ.gLJZYJe_zH9b9yxFxQZm6g";
    
    QString html = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset='utf-8'>
            <title>3D Path Visualization</title>
            <script src='https://unpkg.com/mapbox-gl@2.15.0/dist/mapbox-gl.js'></script>
            <link href='https://unpkg.com/mapbox-gl@2.15.0/dist/mapbox-gl.css' rel='stylesheet' />
            <script src='https://cdn.jsdelivr.net/npm/threebox-plugin@2.2.7/dist/threebox.min.js'></script>
            <style>
                body { margin: 0; padding: 0; }
                #map { position: absolute; top: 0; bottom: 0; width: 100%; }
            </style>
        </head>
        <body>
            <div id='map'></div>
            <script>
                const MAPBOX_TOKEN = '%1';
                const geojsonData = %2;

                mapboxgl.accessToken = MAPBOX_TOKEN;
                
                const map = new mapboxgl.Map({
                    container: 'map',
                    style: 'mapbox://styles/mapbox/dark-v11',
                    center: [77.9806, 10.3637],
                    zoom: 14,
                    pitch: 60,
                    bearing: 0,
                    antialias: true
                });

                map.on('style.load', function() {
                    // Initialize Threebox
                    const tb = new Threebox(
                        map,
                        map.getCanvas().getContext('webgl'),
                        {
                            defaultLights: true,
                            enableSelectingObjects: true,
                            enableTooltips: true,
                            enableDraggingObjects: true,
                            terrain: true
                        }
                    );
                    map.addLayer({
                        id: 'custom-threebox-terrain',
                        type: 'custom',
                        renderingMode: '3d',
                        onAdd: function() {
                            // Get coordinates
                            const coordinates = geojsonData.features[0].geometry.coordinates;
                            
                            // Create line geometry with thicker line
                            const material = new THREE.LineBasicMaterial({
                                color: 0xff0000,
                                linewidth: 10
                            });

                            const points = coordinates.map(coord => {
                                return tb.utils.projectToWorld([coord[0], coord[1], coord[2] * 5]);
                            });

                            const geometry = new THREE.BufferGeometry().setFromPoints(points);
                            const line = new THREE.Line(geometry, material);
                            
                            // Add to scene
                            tb.add(line);

                            // Add markers with popups
                            coordinates.forEach(coord => {
                                new mapboxgl.Marker({ 
                                    color: '#ff0000',
                                    scale: 0.5 
                                })
                                .setLngLat([coord[0], coord[1]])
                                .setPopup(new mapboxgl.Popup().setHTML(`Elevation: ${coord[2]}m`))
                                .addTo(map);
                            });
                        },
                        render: function() {
                            tb.update();
                        }
                    });

                    // Add terrain
                    map.addSource('mapbox-dem', {
                        'type': 'raster-dem',
                        'url': 'mapbox://mapbox.mapbox-terrain-dem-v1',
                        'tileSize': 512,
                        'maxzoom': 14
                    });
                    map.setTerrain({ 'source': 'mapbox-dem', 'exaggeration': 1.5 });

                    // Fit to bounds
                    const coordinates = geojsonData.features[0].geometry.coordinates;
                    const bounds = coordinates.reduce((bounds, coord) => {
                        return bounds.extend([coord[0], coord[1]]);
                    }, new mapboxgl.LngLatBounds(coordinates[0], coordinates[0]));

                    map.fitBounds(bounds, {
                        padding: 50
                    });
                });

                map.on('error', function(e) {
                    console.error('Map error:', e.error);
                });
            </script>
        </body>
        </html>
    )";
    
    m_webView->setHtml(html.arg(mapboxToken, geojsonStr));
}

void MapViewer::toggleView()
{
    if (m_currentMode == MapMode) {
        m_stackedWidget->setCurrentIndex(SimulationMode);
        m_toggleButton->setText("Switch to Map");
        m_currentMode = SimulationMode;
    } else {
        m_stackedWidget->setCurrentIndex(MapMode);
        m_toggleButton->setText("Switch to Simulation");
        m_currentMode = MapMode;
    }
}

void MapViewer::setDronePositions(const QVector<QVector3D>& positions)
{
    // Update positions in simulation view
    m_simulationView->setDronePositions(positions);
    
    // Update positions in map view
    QJsonArray droneArray;
    for (const QVector3D& pos : positions) {
        QJsonObject drone;
        drone["x"] = pos.x();
        drone["y"] = pos.y();
        drone["z"] = pos.z();
        droneArray.append(drone);
    }
    
    QJsonDocument doc(droneArray);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    
    QString script = QString("updateDronePositions(%1);").arg(jsonString);
    m_webView->page()->runJavaScript(script);
}

void MapViewer::updateDronePath(const QJsonObject& geojsonData)
{
    QJsonDocument doc(geojsonData);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    
    QString script = QString("updateDronePath(%1);").arg(jsonString);
    m_webView->page()->runJavaScript(script);
}

MapViewer::~MapViewer()
{
} 