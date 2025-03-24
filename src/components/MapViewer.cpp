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
    }

    const QString mapboxToken = "pk.eyJ1Ijoibmlja3lqMTIxIiwiYSI6ImNtN3N3eHFtcTB1MTkya3M4Mnc0dmQxanAifQ.gLJZYJe_zH9b9yxFxQZm6g";
    
    QString html = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset='utf-8'>
            <title>3D Path Visualization</title>
            <script src='https://api.mapbox.com/mapbox-gl-js/v2.15.0/mapbox-gl.js'></script>
            <link href='https://api.mapbox.com/mapbox-gl-js/v2.15.0/mapbox-gl.css' rel='stylesheet' />
            <!-- Add Mapbox Draw plugin -->
            <script src='https://api.mapbox.com/mapbox-gl-js/plugins/mapbox-gl-draw/v1.4.0/mapbox-gl-draw.js'></script>
            <link rel='stylesheet' href='https://api.mapbox.com/mapbox-gl-js/plugins/mapbox-gl-draw/v1.4.0/mapbox-gl-draw.css' type='text/css' />
            <style>
                body { margin: 0; padding: 0; }
                #map { position: absolute; top: 0; bottom: 0; width: 100%; }
                .mapboxgl-ctrl-group { background: #252525; }
                .mapboxgl-ctrl-group button { color: #00a6ff; }
                .mapboxgl-ctrl-group button:hover { background-color: #333; }
                .custom-controls {
                    position: absolute;
                    top: 10px;
                    right: 10px;
                    z-index: 1;
                    background-color: #252525;
                    border-radius: 4px;
                    padding: 5px;
                    display: flex;
                    flex-direction: column;
                }
                .control-button {
                    background-color: #252525;
                    color: #00a6ff;
                    border: 1px solid #00a6ff;
                    border-radius: 4px;
                    padding: 8px 12px;
                    margin: 5px;
                    cursor: pointer;
                    font-weight: bold;
                    transition: all 0.3s;
                }
                .control-button:hover {
                    background-color: #00a6ff;
                    color: #ffffff;
                }
            </style>
        </head>
        <body>
            <div id='map'></div>
            <div class='custom-controls'>
                <button id='zoom-in' class='control-button'>Zoom In</button>
                <button id='zoom-out' class='control-button'>Zoom Out</button>
                <button id='reset-north' class='control-button'>Reset North</button>
                <button id='save-geometry' class='control-button'>Save Geometry</button>
            </div>
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
                
                // Initialize the draw control
                const draw = new MapboxDraw({
                    displayControlsDefault: false,
                    controls: {
                        point: true,
                        line_string: true,
                        polygon: true,
                        trash: true
                    },
                    styles: [
                        // Line style
                        {
                            'id': 'gl-draw-line',
                            'type': 'line',
                            'filter': ['all', ['==', '$type', 'LineString'], ['!=', 'mode', 'static']],
                            'layout': {
                                'line-cap': 'round',
                                'line-join': 'round'
                            },
                            'paint': {
                                'line-color': '#00a6ff',
                                'line-width': 3
                            }
                        },
                        // Point style
                        {
                            'id': 'gl-draw-point',
                            'type': 'circle',
                            'filter': ['all', ['==', '$type', 'Point'], ['!=', 'mode', 'static']],
                            'paint': {
                                'circle-radius': 6,
                                'circle-color': '#00a6ff'
                            }
                        },
                        // Polygon style
                        {
                            'id': 'gl-draw-polygon',
                            'type': 'fill',
                            'filter': ['all', ['==', '$type', 'Polygon'], ['!=', 'mode', 'static']],
                            'paint': {
                                'fill-color': '#00a6ff',
                                'fill-outline-color': '#00a6ff',
                                'fill-opacity': 0.3
                            }
                        }
                    ]
                });
                
                // Add the draw control to the map
                map.addControl(draw, 'top-left');
                
                // Add navigation control
                map.addControl(new mapboxgl.NavigationControl(), 'top-left');
                
                // Add fullscreen control
                map.addControl(new mapboxgl.FullscreenControl(), 'top-left');

                map.on('style.load', () => {
                    // Add terrain source
                    map.addSource('mapbox-dem', {
                        'type': 'raster-dem',
                        'url': 'mapbox://mapbox.mapbox-terrain-dem-v1',
                        'tileSize': 512,
                        'maxzoom': 14
                    });
                    
                    // Add terrain and sky
                    map.setTerrain({ 'source': 'mapbox-dem', 'exaggeration': 1.5 });
                    map.addLayer({
                        'id': 'sky',
                        'type': 'sky',
                        'paint': {
                            'sky-type': 'atmosphere',
                            'sky-atmosphere-sun': [0.0, 90.0],
                            'sky-atmosphere-sun-intensity': 15
                        }
                    });

                    // Add the drone path source
                    map.addSource('drone-path', {
                        'type': 'geojson',
                        'data': geojsonData
                    });

                    // Add the path layer with glow effect
                    map.addLayer({
                        'id': 'drone-path-glow',
                        'type': 'line',
                        'source': 'drone-path',
                        'paint': {
                            'line-color': '#ff0000',
                            'line-width': 12,
                            'line-opacity': 0.3,
                            'line-blur': 3
                        }
                    });

                    // Add the main path layer
                    map.addLayer({
                        'id': 'drone-path',
                        'type': 'line',
                        'source': 'drone-path',
                        'paint': {
                            'line-color': '#ff0000',
                            'line-width': 4,
                            'line-opacity': 0.8
                        }
                    });

                    // Add altitude markers if coordinates have altitude (z) values
                    if (geojsonData.features && geojsonData.features.length > 0) {
                        const coordinates = geojsonData.features[0].geometry.coordinates;
                        coordinates.forEach((coord, index) => {
                            const height = coord.length > 2 ? coord[2] : 0;
                            if (height > 0) {
                                const el = document.createElement('div');
                                el.className = 'altitude-marker';
                                el.style.height = Math.max(8, height / 10) + 'px';
                                el.style.backgroundColor = '#ff0000';
                                el.style.width = '4px';
                                el.style.borderRadius = '2px';
                                
                                new mapboxgl.Marker({
                                    element: el,
                                    anchor: 'bottom'
                                })
                                .setLngLat([coord[0], coord[1]])
                                .setPopup(new mapboxgl.Popup({
                                    closeButton: false
                                }).setHTML(`Altitude: ${height}m`))
                                .addTo(map);
                            }
                        });
                    }

                    // Fit to bounds if coordinates exist
                    if (geojsonData.features && geojsonData.features.length > 0) {
                        const coordinates = geojsonData.features[0].geometry.coordinates;
                        if (coordinates && coordinates.length > 0) {
                            const bounds = coordinates.reduce((bounds, coord) => {
                                return bounds.extend([coord[0], coord[1]]);
                            }, new mapboxgl.LngLatBounds(coordinates[0], coordinates[0]));

                            map.fitBounds(bounds, {
                                padding: 50,
                                pitch: 60
                            });
                        }
                    }
                });

                // Function to update drone path
                window.updateDronePath = function(newGeojsonData) {
                    if (map.getSource('drone-path')) {
                        map.getSource('drone-path').setData(newGeojsonData);
                        
                        // Update bounds
                        if (newGeojsonData.features && newGeojsonData.features.length > 0) {
                            const coordinates = newGeojsonData.features[0].geometry.coordinates;
                            if (coordinates && coordinates.length > 0) {
                                const bounds = coordinates.reduce((bounds, coord) => {
                                    return bounds.extend([coord[0], coord[1]]);
                                }, new mapboxgl.LngLatBounds(coordinates[0], coordinates[0]));

                                map.fitBounds(bounds, {
                                    padding: 50,
                                    pitch: 60
                                });
                            }
                        }
                    }
                };
                
                // Custom control event listeners
                document.getElementById('zoom-in').addEventListener('click', () => {
                    map.zoomIn();
                });
                
                document.getElementById('zoom-out').addEventListener('click', () => {
                    map.zoomOut();
                });
                
                document.getElementById('reset-north').addEventListener('click', () => {
                    map.easeTo({ bearing: 0 });
                });
                
                document.getElementById('save-geometry').addEventListener('click', () => {
                    const data = draw.getAll();
                    if (data.features.length > 0) {
                        // Send data to Qt
                        window.qt.saveGeometryData(JSON.stringify(data));
                    } else {
                        alert('No geometries to save');
                    }
                });
                
                // Listen for drawing events
                map.on('draw.create', updateGeometry);
                map.on('draw.update', updateGeometry);
                map.on('draw.delete', updateGeometry);
                
                function updateGeometry() {
                    const data = draw.getAll();
                    if (data.features.length > 0) {
                        // Send data to Qt in real-time
                        window.qt.updateGeometryData(JSON.stringify(data));
                    }
                }

                map.on('error', (e) => {
                    console.error('Map error:', e.error);
                });
            </script>
        </body>
        </html>
    )";
    
    // Create a custom web page with a bridge to receive JavaScript calls
    DebugWebEnginePage* page = new DebugWebEnginePage(QWebEngineProfile::defaultProfile(), this);
    m_webView->setPage(page);
    
    // Set up the bridge for JavaScript communication
    page->setWebChannel(new QWebChannel(page));
    page->webChannel()->registerObject(QStringLiteral("qt"), this);
    
    m_webView->setHtml(html.arg(mapboxToken, geojsonStr));
    
    // Start a timer to check for file changes
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MapViewer::checkForFileChanges);
    timer->start(1000); // Check every second
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

void MapViewer::saveGeometryData(const QString& geometryData)
{
    // Get application path
    QString appPath = QCoreApplication::applicationDirPath();
    QString geojsonDir = appPath + "/drone_geojson";
    
    // Ensure directory exists
    QDir dir(geojsonDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Save to geomatics.geojson file
    QString filename = geojsonDir + "/geomatics.geojson";
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << geometryData;
        file.close();
        qDebug() << "Geometry data saved to" << filename;
    } else {
        qDebug() << "Failed to save geometry data to file:" << file.errorString();
    }
}

void MapViewer::updateGeometryData(const QString& geometryData)
{
    // Real-time update of geometry data
    // This method is called whenever the user draws or edits geometry
    
    // Get application path
    QString appPath = QCoreApplication::applicationDirPath();
    QString geojsonDir = appPath + "/drone_geojson";
    
    // Ensure directory exists
    QDir dir(geojsonDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Save to geomatics.geojson file
    QString filename = geojsonDir + "/geomatics.geojson";
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << geometryData;
        file.close();
    }
}

void MapViewer::checkForFileChanges()
{
    // Get application path
    QString appPath = QCoreApplication::applicationDirPath();
    QString geojsonDir = appPath + "/drone_geojson";
    
    // Check for Atlas_path.geojson file changes
    QString filename = geojsonDir + "/Atlas_path.geojson";
    QFileInfo fileInfo(filename);
    
    if (fileInfo.exists() && fileInfo.isFile()) {
        QDateTime lastModified = fileInfo.lastModified();
        
        // Check if file has been modified since last check
        if (!m_lastFileModified.isValid() || lastModified > m_lastFileModified) {
            // File has been modified, reload it
            QFile file(filename);
            if (file.open(QIODevice::ReadOnly)) {
                QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
                QString geojsonStr = doc.toJson(QJsonDocument::Compact);
                file.close();
                
                // Calculate hash to avoid unnecessary updates
                QString newHash = QString(QCryptographicHash::hash(geojsonStr.toUtf8(), QCryptographicHash::Md5).toHex());
                
                if (newHash != m_lastGeojsonHash) {
                    // Update the map with new GeoJSON data
                    m_webView->page()->runJavaScript(QString("updateDronePath(%1);").arg(geojsonStr));
                    m_lastGeojsonHash = newHash;
                }
            }
            
            // Update last modified time
            m_lastFileModified = lastModified;
        }
    }
}

MapViewer::~MapViewer()
{
}