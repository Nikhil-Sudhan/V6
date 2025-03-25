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

MapViewer::MapViewer(QWidget* parent) : QWidget(parent)
{
    setupUI();
    loadMap();
    
    // Initial check for file changes
    QTimer::singleShot(500, this, &MapViewer::checkForFileChanges);
}

void MapViewer::setupUI()
{
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
    connect(m_fileCheckTimer, &QTimer::timeout, this, &MapViewer::loadGeometricShapes);
    m_fileCheckTimer->start(1000); // Check every second
    
    // Load the map
    loadMap();
}

void MapViewer::loadMap()
{
    // Get GeoJSON data
    QString geojsonPath = QDir::currentPath() + "/drone_geojson/Atlas_path.geojson";
    m_lastGeojsonPath = geojsonPath;
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
            <script src="qrc:///qtwebchannel/qwebchannel.js"></script>
            <style>
                body { margin: 0; padding: 0; }
                #map { position: absolute; top: 0; bottom: 0; width: 100%; }
                .mapboxgl-ctrl-group { background: #252525; }
                .mapboxgl-ctrl-group button { color: #00a6ff; }
                .mapboxgl-ctrl-group button:hover { background-color: #333; }
            </style>
        </head>
        <body>
            <div id='map'></div>
            <script>
                // Initialize the Qt web channel
                var qt_object;
                new QWebChannel(qt.webChannelTransport, function(channel) {
                    qt_object = channel.objects.qt_object;
                });
                
                const MAPBOX_TOKEN = '%1';
                const geojsonData = %2;

                mapboxgl.accessToken = MAPBOX_TOKEN;
                
                // Initialize the map
                const map = new mapboxgl.Map({
                    container: 'map',
                    style: 'mapbox://styles/mapbox/dark-v11',
                    center: [77.9806, 10.3637],
                    zoom: 14,
                    pitch: 60,
                    bearing: 0,
                    antialias: true
                });
                
                // Wait for map to load before adding sources and controls
                map.on('load', function() {
                    // Add Mapbox Draw control
                    window.draw = new MapboxDraw({
                        displayControlsDefault: false,
                        controls: {
                            polygon: true,
                            line_string: true,
                            point: true,
                            trash: true
                        },
                        styles: [
                            // Default styles
                            {
                                'id': 'gl-draw-polygon-fill-inactive',
                                'type': 'fill',
                                'filter': ['all', ['==', 'active', 'false'], ['==', '$type', 'Polygon']],
                                'paint': {
                                    'fill-color': '#3388ff',
                                    'fill-outline-color': '#3388ff',
                                    'fill-opacity': 0.4
                                }
                            },
                            {
                                'id': 'gl-draw-polygon-fill-active',
                                'type': 'fill',
                                'filter': ['all', ['==', 'active', 'true'], ['==', '$type', 'Polygon']],
                                'paint': {
                                    'fill-color': '#fbb03b',
                                    'fill-outline-color': '#fbb03b',
                                    'fill-opacity': 0.6
                                }
                            },
                            {
                                'id': 'gl-draw-polygon-stroke-inactive',
                                'type': 'line',
                                'filter': ['all', ['==', 'active', 'false'], ['==', '$type', 'Polygon']],
                                'layout': {
                                    'line-cap': 'round',
                                    'line-join': 'round'
                                },
                                'paint': {
                                    'line-color': '#3388ff',
                                    'line-width': 2
                                }
                            },
                            {
                                'id': 'gl-draw-polygon-stroke-active',
                                'type': 'line',
                                'filter': ['all', ['==', 'active', 'true'], ['==', '$type', 'Polygon']],
                                'layout': {
                                    'line-cap': 'round',
                                    'line-join': 'round'
                                },
                                'paint': {
                                    'line-color': '#fbb03b',
                                    'line-dasharray': [0.2, 2],
                                    'line-width': 2
                                }
                            },
                            {
                                'id': 'gl-draw-line-inactive',
                                'type': 'line',
                                'filter': ['all', ['==', 'active', 'false'], ['==', '$type', 'LineString']],
                                'layout': {
                                    'line-cap': 'round',
                                    'line-join': 'round'
                                },
                                'paint': {
                                    'line-color': '#3388ff',
                                    'line-width': 2
                                }
                            },
                            {
                                'id': 'gl-draw-line-active',
                                'type': 'line',
                                'filter': ['all', ['==', 'active', 'true'], ['==', '$type', 'LineString']],
                                'layout': {
                                    'line-cap': 'round',
                                    'line-join': 'round'
                                },
                                'paint': {
                                    'line-color': '#fbb03b',
                                    'line-dasharray': [0.2, 2],
                                    'line-width': 2
                                }
                            },
                            {
                                'id': 'gl-draw-point-inactive',
                                'type': 'circle',
                                'filter': ['all', ['==', 'active', 'false'], ['==', '$type', 'Point']],
                                'paint': {
                                    'circle-radius': 5,
                                    'circle-color': '#3388ff'
                                }
                            },
                            {
                                'id': 'gl-draw-point-active',
                                'type': 'circle',
                                'filter': ['all', ['==', 'active', 'true'], ['==', '$type', 'Point']],
                                'paint': {
                                    'circle-radius': 7,
                                    'circle-color': '#fbb03b'
                                }
                            },
                            {
                                'id': 'gl-draw-polygon-fill-static',
                                'type': 'fill',
                                'filter': ['all', ['==', 'mode', 'static'], ['==', '$type', 'Polygon']],
                                'paint': {
                                    'fill-color': '#3388ff',
                                    'fill-outline-color': '#3388ff',
                                    'fill-opacity': 0.4
                                }
                            },
                            {
                                'id': 'gl-draw-polygon-stroke-static',
                                'type': 'line',
                                'filter': ['all', ['==', 'mode', 'static'], ['==', '$type', 'Polygon']],
                                'layout': {
                                    'line-cap': 'round',
                                    'line-join': 'round'
                                },
                                'paint': {
                                    'line-color': '#3388ff',
                                    'line-width': 2
                                }
                            },
                            {
                                'id': 'gl-draw-line-static',
                                'type': 'line',
                                'filter': ['all', ['==', 'mode', 'static'], ['==', '$type', 'LineString']],
                                'layout': {
                                    'line-cap': 'round',
                                    'line-join': 'round'
                                },
                                'paint': {
                                    'line-color': '#3388ff',
                                    'line-width': 2
                                }
                            },
                            {
                                'id': 'gl-draw-point-static',
                                'type': 'circle',
                                'filter': ['all', ['==', 'mode', 'static'], ['==', '$type', 'Point']],
                                'paint': {
                                    'circle-radius': 5,
                                    'circle-color': '#3388ff'
                                }
                            },
                            // Label styles for shapes
                            {
                                'id': 'gl-draw-shape-labels',
                                'type': 'symbol',
                                'filter': ['has', 'name'],
                                'layout': {
                                    'text-field': ['get', 'name'],
                                    'text-font': ['Open Sans Regular'],
                                    'text-size': 12,
                                    'text-anchor': 'center',
                                    'text-justify': 'center',
                                    'text-allow-overlap': true
                                },
                                'paint': {
                                    'text-color': '#000000',
                                    'text-halo-color': '#ffffff',
                                    'text-halo-width': 2
                                }
                            }
                        ]
                    });
                    
                    map.addControl(window.draw, 'top-right');

                    // Function to update geometric shapes
                    window.updateGeometricShapes = function(shapesData) {
                        // First remove any existing shapes
                        if (window.draw) {
                            window.draw.deleteAll();
                            
                            // Add the shapes from the GeoJSON data
                            if (shapesData && shapesData.features && shapesData.features.length > 0) {
                                window.draw.add(shapesData);
                            }
                        }
                    };
                    
                    // Handle draw.create event
                    map.on('draw.create', function(e) {
                        // Prompt for shape name
                        const shapeName = prompt('Enter a name for this shape:', 'Shape ' + new Date().toLocaleTimeString());
                        
                        if (shapeName) {
                            // Create a GeoJSON with the new shape
                            const shapeData = {
                                type: 'FeatureCollection',
                                features: e.features
                            };
                            
                            // Send the shape data to Qt
                            qt_object.saveGeometricShape(JSON.stringify(shapeData), shapeName);
                        }
                    });
                    
                    // Handle draw.update event
                    map.on('draw.update', function(e) {
                        // Get the updated features
                        const shapeData = {
                            type: 'FeatureCollection',
                            features: e.features
                        };
                        
                        // Get the name from the first feature's properties
                        let shapeName = 'Updated Shape';
                        if (e.features[0].properties && e.features[0].properties.name) {
                            shapeName = e.features[0].properties.name;
                        }
                        
                        // Send the updated shape data to Qt
                        qt_object.saveGeometricShape(JSON.stringify(shapeData), shapeName);
                    });

                    // Add terrain source
                    map.addSource('mapbox-dem', {
                        'type': 'raster-dem',
                        'url': 'mapbox://mapbox.mapbox-terrain-dem-v1',
                        'tileSize': 512,
                        'maxzoom': 14
                    });
                    
                    // Add 3D terrain
                    map.setTerrain({ 'source': 'mapbox-dem', 'exaggeration': 1.5 });
                    
                    // Add drone path source and layer
                    map.addSource('drone-path', {
                        'type': 'geojson',
                        'data': geojsonData
                    });
                    
                    // Add drone path layer
                    map.addLayer({
                        'id': 'drone-path-layer',
                        'type': 'line',
                        'source': 'drone-path',
                        'layout': {
                            'line-join': 'round',
                            'line-cap': 'round'
                        },
                        'paint': {
                            'line-color': ['get', 'color'],
                            'line-width': 4,
                            'line-opacity': 0.8
                        }
                    });
                    
                    // Add drone points layer
                    map.addLayer({
                        'id': 'drone-points',
                        'type': 'circle',
                        'source': 'drone-path',
                        'paint': {
                            'circle-radius': 5,
                            'circle-color': ['get', 'color'],
                            'circle-opacity': 0.7,
                            'circle-stroke-width': 2,
                            'circle-stroke-color': '#ffffff'
                        }
                    });
                    
                    // Add drone labels layer
                    map.addLayer({
                        'id': 'drone-labels',
                        'type': 'symbol',
                        'source': 'drone-path',
                        'layout': {
                            'text-field': ['get', 'name'],
                            'text-size': 12,
                            'text-offset': [0, 1.5],
                            'text-anchor': 'top'
                        },
                        'paint': {
                            'text-color': '#ffffff',
                            'text-halo-color': '#000000',
                            'text-halo-width': 1
                        }
                    });
                    
                    // Function to update drone path data
                    window.updateDronePath = function(newData) {
                        map.getSource('drone-path').setData(newData);
                    };
                    
                    // Signal to Qt that the map is ready
                    if (qt_object) {
                        qt_object.loadGeometricShapes();
                    }
                });
            </script>
        </body>
        </html>
    )";
    
    m_webView->setHtml(html.arg(mapboxToken, geojsonStr));
}

void MapViewer::setDronePositions(const QVector<QVector3D>& positions)
{
    // Update positions in simulation view
    // m_simulationView->setDronePositions(positions);
    
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
    // Process the features to ensure active drone is highlighted
    QJsonObject processedData = geojsonData;
    
    if (processedData.contains("features") && processedData["features"].isArray()) {
        QJsonArray features = processedData["features"].toArray();
        
        // Process each feature to ensure proper properties
        for (int i = 0; i < features.size(); ++i) {
            QJsonObject feature = features[i].toObject();
            
            if (feature.contains("properties") && feature["properties"].isObject()) {
                QJsonObject props = feature["properties"].toObject();
                
                // Check if this is a drone path
                if (props.contains("name")) {
                    QString droneName = props["name"].toString();
                    
                    // Set active flag based on current active drone
                    props["active"] = (droneName == m_activeDroneName);
                    
                    // Ensure color property exists
                    if (!props.contains("color")) {
                        // Generate a consistent color based on drone name if not already defined
                        QStringList colors = {"#ff0000", "#00ff00", "#0000ff", "#ffff00", "#ff00ff", "#00ffff"};
                        int colorIndex = qHash(droneName) % colors.size();
                        props["color"] = colors[colorIndex];
                    }
                    
                    // Store color in our map for consistency
                    m_dronePathColors[droneName] = props["color"].toString();
                    
                    feature["properties"] = props;
                    features[i] = feature;
                }
            }
        }
        
        processedData["features"] = features;
    }
    
    // Convert to JSON string
    QJsonDocument doc(processedData);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    
    // Calculate hash to avoid unnecessary updates
    QString newHash = QString(QCryptographicHash::hash(jsonString.toUtf8(), QCryptographicHash::Md5).toHex());
    
    // Only update if content has changed
    if (newHash != m_lastGeojsonHash) {
        QString script = QString("updateDronePath(%1);").arg(jsonString);
        m_webView->page()->runJavaScript(script, [this](const QVariant &result) {
            qDebug() << "Map updated with drone paths";
        });
        
        m_lastGeojsonHash = newHash;
    }
}

void MapViewer::setActiveDrone(const QString& droneName)
{
    qDebug() << "Setting active drone to:" << droneName;
    m_activeDroneName = droneName;
    
    // Initialize drone path colors if not already set
    if (m_dronePathColors.isEmpty()) {
        // Define a set of distinct colors for different drones
        m_dronePathColors["Atlas"] = "#FF5733";    // Bright red/orange
        m_dronePathColors["Bolt"] = "#33A8FF";     // Bright blue
        m_dronePathColors["Barbarian"] = "#33FF57"; // Bright green
        m_dronePathColors["Phantom"] = "#A833FF";  // Purple
        m_dronePathColors["Shadow"] = "#FFD700";   // Gold
        m_dronePathColors["Hawk"] = "#FF33A8";     // Pink
        m_dronePathColors["Eagle"] = "#00FFFF";    // Cyan
        m_dronePathColors["Falcon"] = "#FF8C00";   // Dark orange
    }
    
    // If this drone doesn't have a color yet, assign a random one
    if (!m_dronePathColors.contains(droneName)) {
        // Generate a random color if not in predefined list
        QColor randomColor = QColor::fromHsv(
            QRandomGenerator::global()->bounded(360),  // Hue (0-359)
            QRandomGenerator::global()->bounded(155, 255),  // Saturation (155-255 for vibrant colors)
            QRandomGenerator::global()->bounded(200, 255)   // Value (200-255 for brightness)
        );
        m_dronePathColors[droneName] = randomColor.name();
        qDebug() << "Assigned random color to drone:" << droneName << "-" << randomColor.name();
    }
    
    // Trigger a check for file changes to update the display
    checkForFileChanges();
}

void MapViewer::saveGeometryData(const QString& geometryData)
{
    // Get application path
    QString geojsonDir = QDir::currentPath() + "/drone_geojson";
    
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
        qInfo() << "Geometry data saved to:" << QDir::toNativeSeparators(filename);
    } else {
        qWarning() << "Failed to save geometry data to file:" << file.errorString();
    }
}

void MapViewer::updateGeometryData(const QString& geometryData)
{
    // This is the same as saveGeometryData now, just save the data
    saveGeometryData(geometryData);
}

void MapViewer::saveGeometricShape(const QString& shapeData, const QString& shapeName)
{
    // Parse the shape data
    QJsonDocument shapeDoc = QJsonDocument::fromJson(shapeData.toUtf8());
    if (shapeDoc.isNull() || !shapeDoc.isObject()) {
        qDebug() << "Invalid shape data format";
        return;
    }
    
    QJsonObject shapeObj = shapeDoc.object();
    
    // Get current path
    QString geojsonDir = QDir::currentPath() + "/drone_geojson";
    
    // Ensure directory exists
    QDir dir(geojsonDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Path to the geometric shapes file
    QString shapesFilename = geojsonDir + "/geometric_shapes.geojson";
    
    // Create or load the shapes FeatureCollection file
    QJsonObject shapesGeoJson;
    QFile shapesFile(shapesFilename);
    
    if (shapesFile.exists() && shapesFile.open(QIODevice::ReadOnly)) {
        // File exists, read and parse it
        QJsonDocument existingDoc = QJsonDocument::fromJson(shapesFile.readAll());
        shapesFile.close();
        
        if (!existingDoc.isNull() && existingDoc.isObject()) {
            shapesGeoJson = existingDoc.object();
        } else {
            // Invalid existing file, create new structure
            shapesGeoJson["type"] = "FeatureCollection";
            shapesGeoJson["features"] = QJsonArray();
        }
    } else {
        // File doesn't exist or couldn't be opened, create new structure
        shapesGeoJson["type"] = "FeatureCollection";
        shapesGeoJson["features"] = QJsonArray();
    }
    
    // Get the features array
    QJsonArray features;
    if (shapesGeoJson.contains("features") && shapesGeoJson["features"].isArray()) {
        features = shapesGeoJson["features"].toArray();
    }
    
    // Process the shape data to add name to properties
    if (shapeObj.contains("features") && shapeObj["features"].isArray()) {
        QJsonArray shapeFeatures = shapeObj["features"].toArray();
        
        for (int i = 0; i < shapeFeatures.size(); ++i) {
            QJsonObject feature = shapeFeatures[i].toObject();
            
            // Add name to properties
            if (feature.contains("properties") && feature["properties"].isObject()) {
                QJsonObject props = feature["properties"].toObject();
                props["name"] = shapeName;
                feature["properties"] = props;
            } else {
                QJsonObject props;
                props["name"] = shapeName;
                feature["properties"] = props;
            }
            
            // Add the feature to the collection
            features.append(feature);
        }
    }
    
    // Update the features array
    shapesGeoJson["features"] = features;
    
    // Save the updated FeatureCollection to the file
    if (shapesFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&shapesFile);
        QJsonDocument shapesDoc(shapesGeoJson);
        out << shapesDoc.toJson(QJsonDocument::Indented);
        shapesFile.close();
        qDebug() << "Saved geometric shape:" << shapeName << "to file:" << shapesFilename;
        
        // Emit signal that shape was saved
        emit geometricShapeSaved(shapeName);
        
        // Update the map with the shapes
        loadGeometricShapes();
    } else {
        qDebug() << "Failed to save geometric shapes file:" << shapesFilename << "-" << shapesFile.errorString();
    }
}

void MapViewer::loadGeometricShapes()
{
    // Get current path
    QString geojsonDir = QDir::currentPath() + "/drone_geojson";
    
    // Ensure directory exists
    QDir dir(geojsonDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Path to the geometric shapes file
    QString shapesFilename = geojsonDir + "/geometric_shapes.geojson";
    QFileInfo fileInfo(shapesFilename);
    
    // Create empty file if it doesn't exist
    if (!fileInfo.exists()) {
        QFile file(shapesFilename);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QJsonObject emptyGeoJson;
            emptyGeoJson["type"] = "FeatureCollection";
            emptyGeoJson["features"] = QJsonArray();
            
            QJsonDocument doc(emptyGeoJson);
            file.write(doc.toJson());
            file.close();
            qDebug() << "Created empty geometric shapes file:" << shapesFilename;
        }
    }
    
    // Now load the file (which should exist)
    QDateTime lastModified = fileInfo.lastModified();
    
    // Check if file has been modified since last check
    if (!m_lastShapesFileModified.isValid() || lastModified > m_lastShapesFileModified) {
        // File has been modified, reload it
        QFile file(shapesFilename);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            QJsonObject shapesObj = doc.object();
            file.close();
            
            // Debug output
            qDebug() << "Loading geometric shapes from file:" << shapesFilename;
            
            // Update the map with the shapes
            QString shapesStr = QJsonDocument(shapesObj).toJson(QJsonDocument::Compact);
            QString script = QString("updateGeometricShapes(%1);").arg(shapesStr);
            m_webView->page()->runJavaScript(script, [this](const QVariant &result) {
                qDebug() << "Map updated with geometric shapes";
            });
            
            // Update last modified time
            m_lastShapesFileModified = lastModified;
        } else {
            qDebug() << "Failed to open geometric shapes file:" << shapesFilename;
        }
    }
}

void MapViewer::checkForFileChanges()
{
    // Get current path
    QString geojsonDir = QDir::currentPath() + "/drone_geojson";
    
    // Ensure directory exists
    QDir dir(geojsonDir);
    if (!dir.exists()) {
        dir.mkpath(".");
        return; // No files to check yet
    }
    
    // Path to the main drone paths file
    QString mainDronePathsFile = geojsonDir + "/all_drone_paths.geojson";
    QFileInfo mainFileInfo(mainDronePathsFile);
    
    // Check if the main file exists and has been modified
    if (mainFileInfo.exists() && mainFileInfo.isFile()) {
        QDateTime lastModified = mainFileInfo.lastModified();
        
        // Check if file has been modified since last check
        if (!m_lastDronePathsFileModified.isValid() || lastModified > m_lastDronePathsFileModified) {
            // File has been modified, reload it
            QFile file(mainDronePathsFile);
            if (file.open(QIODevice::ReadOnly)) {
                QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
                QJsonObject geoJson = doc.object();
                file.close();
                
                // Debug output
                qDebug() << "Loading drone paths from file:" << mainDronePathsFile;
                
                // Update the map with the drone paths
                updateDronePath(geoJson);
                
                // Update last modified time
                m_lastDronePathsFileModified = lastModified;
            } else {
                qDebug() << "Failed to open drone paths file:" << mainDronePathsFile << "-" << file.errorString();
            }
        }
    } else if (!mainFileInfo.exists()) {
        // Create an empty GeoJSON file if it doesn't exist
        QFile file(mainDronePathsFile);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QJsonObject emptyGeoJson;
            emptyGeoJson["type"] = "FeatureCollection";
            emptyGeoJson["features"] = QJsonArray();
            
            QJsonDocument doc(emptyGeoJson);
            file.write(doc.toJson());
            file.close();
            qDebug() << "Created empty drone paths file:" << mainDronePathsFile;
            
            // Set initial last modified time
            m_lastDronePathsFileModified = QFileInfo(mainDronePathsFile).lastModified();
        }
    }
    
    // Also check for geometric shapes file changes
    loadGeometricShapes();
}

MapViewer::~MapViewer()
{
}