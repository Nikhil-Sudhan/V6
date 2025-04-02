#include "../../include/map/mapbox.h"

Mapbox::Mapbox(QWebEngineView* webView, QObject* parent)
    : QObject(parent)
    , m_webView(webView)
    , m_mapboxToken("pk.eyJ1Ijoibmlja3lqMTIxIiwiYSI6ImNtN3N3eHFtcTB1MTkya3M4Mnc0dmQxanAifQ.gLJZYJe_zH9b9yxFxQZm6g")
{
}

Mapbox::~Mapbox()
{
}

void Mapbox::loadMap()
{
    // Get GeoJSON data
    QString geojsonPath = QDir::currentPath() + "/drone_geojson/Atlas_path.geojson";
    m_lastGeojsonPath = geojsonPath;
    QFile file(geojsonPath);
    QString geojsonStr = "{}";
    
    // Create directory if it doesn't exist
    QDir dir(QDir::currentPath() + "/drone_geojson");
    if (!dir.exists()) {
        dir.mkpath(".");
        qDebug() << "Created drone_geojson directory";
    }
    
    // Create empty GeoJSON file if it doesn't exist
    if (!file.exists()) {
        QJsonObject emptyGeoJson;
        emptyGeoJson["type"] = "FeatureCollection";
        emptyGeoJson["features"] = QJsonArray();
        
        QJsonDocument doc(emptyGeoJson);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
            qDebug() << "Created empty GeoJSON file:" << geojsonPath;
        }
    }
    
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        geojsonStr = doc.toJson(QJsonDocument::Compact);
        file.close();
    } else {
        qDebug() << "Error opening GeoJSON file:" << file.errorString();
    }

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
                
                /* Ensure control buttons are visible */
                .mapboxgl-ctrl-top-right {
                    top: 10px;
                    right: 10px;
                }
                
                .mapboxgl-ctrl button {
                    width: 30px;
                    height: 30px;
                }
                
                /* Debug panel for model loading */
                #debug-panel {
                    position: absolute;
                    bottom: 10px;
                    left: 10px;
                    background: rgba(0,0,0,0.7);
                    color: white;
                    padding: 10px;
                    border-radius: 4px;
                    font-family: monospace;
                    font-size: 12px;
                    max-width: 300px;
                    max-height: 150px;
                    overflow: auto;
                    z-index: 999;
                }
            </style>
        </head>
        <body>
            <div id='map'></div>
            <div id='debug-panel'></div>

            <script>
                // Debug logging function
                function debugLog(message) {
                    console.log(message);
                    const debugPanel = document.getElementById('debug-panel');
                    if (debugPanel) {
                        const logLine = document.createElement('div');
                        logLine.textContent = message;
                        debugPanel.appendChild(logLine);
                        debugPanel.scrollTop = debugPanel.scrollHeight;
                        
                        // Limit number of log lines
                        while (debugPanel.children.length > 10) {
                            debugPanel.removeChild(debugPanel.firstChild);
                        }
                    }
                }
                
                // Initialize the Qt web channel
                debugLog("Initializing Qt web channel...");
                var qt_object;
                new QWebChannel(qt.webChannelTransport, function(channel) {
                    qt_object = channel.objects.qt_object;
                    debugLog("Qt web channel initialized");
                });
                
                const MAPBOX_TOKEN = '%1';
                const geojsonData = %2;
                
                debugLog("Mapbox token length: " + MAPBOX_TOKEN.length);
                
                mapboxgl.accessToken = MAPBOX_TOKEN;
                
                // Initialize the map
                debugLog("Initializing map...");
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
                    debugLog("Map loaded successfully");
                    
                    // Add Mapbox Draw control
                    debugLog("Adding draw control...");
                    window.draw = new MapboxDraw({
                        displayControlsDefault: false,
                        controls: {
                            polygon: true,
                            line_string: true,
                            point: true,
                            circle: true,
                            rectangle: true,
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
                                    'line-width': 4
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
                                    'line-width': 4
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
                                    'line-width': 6
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
                                    'line-width': 4
                                }
                            },
                            {
                                'id': 'gl-draw-point-inactive',
                                'type': 'circle',
                                'filter': ['all', ['==', 'active', 'false'], ['==', '$type', 'Point']],
                                'paint': {
                                    'circle-radius': 8,
                                    'circle-color': '#3388ff'
                                }
                            },
                            {
                                'id': 'gl-draw-point-active',
                                'type': 'circle',
                                'filter': ['all', ['==', 'active', 'true'], ['==', '$type', 'Point']],
                                'paint': {
                                    'circle-radius': 10,
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
                                    'line-width': 3
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
                            }
                        ]
                    });
                    
                    try {
                        map.addControl(window.draw, 'top-right');
                        debugLog("Draw control added successfully");
                    } catch (e) {
                        debugLog("Error adding draw control: " + e.message);
                    }
                    
                    // Add navigation control
                    try {
                        map.addControl(new mapboxgl.NavigationControl(), 'top-right');
                        debugLog("Navigation control added successfully");
                    } catch (e) {
                        debugLog("Error adding navigation control: " + e.message);
                    }
                    
                    // Add custom drone path source and layer
                    try {
                        map.addSource('drone-path', {
                            type: 'geojson',
                            data: geojsonData
                        });
                        debugLog("Drone path source added");
                        
                        // Add a line layer for the drone path
                        map.addLayer({
                            id: 'drone-path-line',
                            type: 'line',
                            source: 'drone-path',
                            layout: {
                                'line-join': 'round',
                                'line-cap': 'round'
                            },
                            paint: {
                                'line-color': ['get', 'color'],
                                'line-width': 6,
                                'line-opacity': 0.8
                            }
                        });
                        
                        // Add a point layer for the drone path vertices
                        map.addLayer({
                            id: 'drone-path-points',
                            type: 'circle',
                            source: 'drone-path',
                            paint: {
                                'circle-radius': 5,
                                'circle-color': ['get', 'color'],
                                'circle-opacity': 0.8
                            }
                        });
                        debugLog("Drone path layers added");
                    } catch (e) {
                        debugLog("Error adding drone path source/layers: " + e.message);
                    }
                    
                    // Add a source for drone starting points
                    try {
                        map.addSource('drone-start-points', {
                            type: 'geojson',
                            data: {
                                type: 'FeatureCollection',
                                features: []
                            }
                        });
                        debugLog("Drone start points source added");
                    } catch (e) {
                        debugLog("Error adding drone start points source: " + e.message);
                    }
                    
                    // Load the drone icon image (using a PNG instead of SVG for better compatibility)
                    const loadDroneIcon = () => {
                        debugLog("Loading drone icon...");
                        map.loadImage('https://cdn-icons-png.freepik.com/512/2541/2541514.png', (error, image) => {
                            if (error) {
                                debugLog("Error loading primary drone icon: " + error.message);
                                // Try fallback icon if the first one fails
                                map.loadImage('https://cdn.mapbox.com/mapbox-gl-js/assets/custom_marker.png', (err2, img2) => {
                                    if (err2) {
                                        debugLog("Error loading fallback icon: " + err2.message);
                                        // Try local fallback
                                        map.loadImage('qrc:/icons/mission.png', (err3, img3) => {
                                            if (err3) {
                                                debugLog("All icon loading attempts failed");
                                                return;
                                            }
                                            if (!map.hasImage('drone-icon')) {
                                                map.addImage('drone-icon', img3);
                                                debugLog("Local fallback drone icon added");
                                                updateDroneIconLayer();
                                            }
                                        });
                                        return;
                                    }
                                    if (!map.hasImage('drone-icon')) {
                                        map.addImage('drone-icon', img2);
                                        debugLog("Fallback drone icon added");
                                        updateDroneIconLayer();
                                    }
                                });
                                return;
                            }
                            
                            if (!map.hasImage('drone-icon')) {
                                map.addImage('drone-icon', image);
                                debugLog("Primary drone icon added successfully");
                                updateDroneIconLayer();
                            }
                        });
                    };
                    
                    // Load the drone icon
                    loadDroneIcon();
                    
                    // Function to update drone path start points
                    function updateDroneStartPoints() {
                        debugLog("Updating drone start points...");
                        // Get drone path data
                        const dronePathSource = map.getSource('drone-path');
                        if (!dronePathSource || !dronePathSource._data) {
                            debugLog("No drone path data available");
                            return;
                        }
                        
                        const features = dronePathSource._data.features;
                        const startPoints = {
                            type: 'FeatureCollection',
                            features: []
                        };
                        
                        // Process each drone path to extract start points and calculate bearing
                        features.forEach(feature => {
                            if (feature.geometry && feature.geometry.type === 'LineString' && 
                                feature.geometry.coordinates && feature.geometry.coordinates.length > 0) {
                                
                                const coords = feature.geometry.coordinates;
                                const startCoord = coords[0];
                                
                                // Calculate bearing if we have at least 2 points
                                let bearing = 0;
                                if (coords.length > 1) {
                                    const p1 = coords[0];
                                    const p2 = coords[1];
                                    
                                    // Calculate bearing between first two points
                                    const y = Math.sin(p2[0] - p1[0]) * Math.cos(p2[1]);
                                    const x = Math.cos(p1[1]) * Math.sin(p2[1]) -
                                            Math.sin(p1[1]) * Math.cos(p2[1]) * Math.cos(p2[0] - p1[0]);
                                    bearing = (Math.atan2(y, x) * 180 / Math.PI + 360) % 360;
                                }
                                
                                // Create a point feature for the start coordinate
                                startPoints.features.push({
                                    type: 'Feature',
                                    geometry: {
                                        type: 'Point',
                                        coordinates: startCoord
                                    },
                                    properties: {
                                        droneId: feature.properties.droneId || 'unknown',
                                        color: feature.properties.color || '#FF0000',
                                        bearing: bearing,
                                        featureType: 'droneStart'
                                    }
                                });
                            }
                        });
                        
                        // Update the source with the start points
                        const startPointsSource = map.getSource('drone-start-points');
                        if (startPointsSource) {
                            startPointsSource.setData(startPoints);
                            debugLog(`Updated drone start points: ${startPoints.features.length} points`);
                        }
                    }
                    
                    // Function to add the drone icon layer
                    function updateDroneIconLayer() {
                        debugLog("Updating drone icon layer...");
                        if (!map.getLayer('drone-icons') && map.hasImage('drone-icon')) {
                            try {
                                map.addLayer({
                                    id: 'drone-icons',
                                    type: 'symbol',
                                    source: 'drone-start-points',
                                    layout: {
                                        'icon-image': 'drone-icon',
                                        'icon-size': 0.5,
                                        'icon-rotate': ['get', 'bearing'],
                                        'icon-rotation-alignment': 'map',
                                        'icon-allow-overlap': true,
                                        'icon-ignore-placement': true
                                    },
                                    paint: {
                                        'icon-opacity': 1.0
                                    }
                                });
                                debugLog("Drone icon layer added successfully");
                                // Update drone start points after adding the layer
                                updateDroneStartPoints();
                            } catch (e) {
                                debugLog("Error adding drone icon layer: " + e.message);
                            }
                        } else {
                            if (!map.hasImage('drone-icon')) {
                                debugLog("Cannot add drone icon layer: icon not loaded");
                            } else if (map.getLayer('drone-icons')) {
                                debugLog("Drone icon layer already exists");
                            }
                        }
                    }
                    
                    // Watch for changes to the drone path source and update start points
                    map.on('sourcedata', function(e) {
                        if (e.sourceId === 'drone-path' && e.isSourceLoaded) {
                            updateDroneStartPoints();
                        }
                    });
                    
                    // Listen for draw.create events
                    map.on('draw.create', function(e) {
                        debugLog("Draw create event triggered");
                        const data = window.draw.getAll();
                        if (data.features.length > 0) {
                            const lastFeature = e.features[0];
                            // Send the feature to Qt
                            const geoJson = {
                                type: 'FeatureCollection',
                                features: [lastFeature]
                            };
                            
                            // Prompt for a name for the shape
                            const shapeName = prompt("Enter a name for this shape:", "Shape " + Date.now());
                            if (shapeName) {
                                // Send to Qt
                                debugLog("Saving geometric shape: " + shapeName);
                                if (qt_object && qt_object.saveGeometricShape) {
                                    qt_object.saveGeometricShape(JSON.stringify(geoJson), shapeName);
                                } else {
                                    debugLog("Error: qt_object or saveGeometricShape method not available");
                                }
                            }
                        }
                    });
                    
                    // Function to update drone positions
                    window.updateDronePositions = function(positions) {
                        debugLog("Updating drone positions...");
                        const features = [];
                        
                        for (const pos of positions) {
                            features.push({
                                type: 'Feature',
                                geometry: {
                                    type: 'Point',
                                    coordinates: [pos.x, pos.y]
                                },
                                properties: {
                                    altitude: pos.z
                                }
                            });
                        }
                        
                        const geojson = {
                            type: 'FeatureCollection',
                            features: features
                        };
                        
                        const source = map.getSource('drone-position');
                        if (source) {
                            source.setData(geojson);
                            debugLog("Drone positions updated");
                        } else {
                            debugLog("Error: drone-position source not found");
                        }
                    };
                    
                    // Function to update drone path
                    window.updateDronePath = function(geojsonData) {
                        debugLog("Updating drone path...");
                        const source = map.getSource('drone-path');
                        if (source) {
                            source.setData(geojsonData);
                            debugLog("Drone path updated");
                        } else {
                            debugLog("Error: drone-path source not found");
                        }
                    };
                    
                    // Function to update geometric shapes
                    window.updateGeometricShapes = function(geojsonData) {
                        debugLog("Updating geometric shapes...");
                        const source = map.getSource('geometric-shapes');
                        if (source) {
                            source.setData(geojsonData);
                            debugLog("Geometric shapes updated");
                        } else {
                            debugLog("Error: geometric-shapes source not found");
                        }
                    };
                    
                    // Function to move drone along a path
                    window.moveDroneAlongPath = function(coordinates, currentIndex) {
                        if (currentIndex < coordinates.length) {
                            const position = coordinates[currentIndex];
                            
                            const geojson = {
                                type: 'FeatureCollection',
                                features: [{
                                    type: 'Feature',
                                    geometry: {
                                        type: 'Point',
                                        coordinates: position
                                    },
                                    properties: {}
                                }]
                            };
                            
                            const source = map.getSource('drone-position');
                            if (source) {
                                source.setData(geojson);
                                
                                // Center the map on the drone position
                                map.flyTo({
                                    center: position,
                                    speed: 0.5,
                                    curve: 1,
                                    essential: true
                                });
                                
                                debugLog("Drone moved to position: " + position);
                            } else {
                                debugLog("Error: drone-position source not found");
                            }
                        }
                    };
                    
                    // Add a source for geometric shapes
                    try {
                        map.addSource('geometric-shapes', {
                            type: 'geojson',
                            data: {
                                type: 'FeatureCollection',
                                features: []
                            }
                        });
                        
                        // Add a layer for geometric shapes - fill
                        map.addLayer({
                            id: 'geometric-shapes-fill',
                            type: 'fill',
                            source: 'geometric-shapes',
                            paint: {
                                'fill-color': '#00ffff',
                                'fill-opacity': 0.4
                            }
                        });
                        
                        // Add a layer for geometric shapes - outline
                        map.addLayer({
                            id: 'geometric-shapes-outline',
                            type: 'line',
                            source: 'geometric-shapes',
                            paint: {
                                'line-color': '#00ffff',
                                'line-width': 2
                            }
                        });
                        
                        // Add a layer for geometric shapes - points
                        map.addLayer({
                            id: 'geometric-shapes-points',
                            type: 'circle',
                            source: 'geometric-shapes',
                            paint: {
                                'circle-radius': 5,
                                'circle-color': '#00ffff',
                                'circle-opacity': 0.8
                            }
                        });
                        
                        debugLog("Geometric shapes source and layers added");
                    } catch (e) {
                        debugLog("Error adding geometric shapes source/layers: " + e.message);
                    }
                    
                    // Add a source for drone position
                    try {
                        map.addSource('drone-position', {
                            type: 'geojson',
                            data: {
                                type: 'FeatureCollection',
                                features: []
                            }
                        });
                        
                        map.addLayer({
                            id: 'drone-position-point',
                            type: 'circle',
                            source: 'drone-position',
                            paint: {
                                'circle-radius': 8,
                                'circle-color': '#ff0000',
                                'circle-opacity': 0.8
                            }
                        });
                        
                        debugLog("Drone position source and layer added");
                    } catch (e) {
                        debugLog("Error adding drone position source/layer: " + e.message);
                    }
                    
                    // Add lighting for 3D models
                    map.addLight('main-light', {
                        color: '#FFFFFF',
                        intensity: 1.0,
                        position: [1, 0, 1]
                    });
                    
                    map.addLight('ambient-light', {
                        color: '#FFFFFF',
                        intensity: 0.5
                    });
                    
                    debugLog("Map setup complete");
                });
            </script>
        </body>
        </html>
    )";
    
    // Replace the token and geojson data
    html = html.arg(m_mapboxToken).arg(geojsonStr);
    
    // Load the HTML into the web view
    m_webView->setHtml(html);
    
    // Connect to Qt web channel
    QWebChannel* channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("qt_object"), this);
    m_webView->page()->setWebChannel(channel);
}

void Mapbox::saveGeometricShape(const QString& geoJson, const QString& shapeName)
{
    qDebug() << "Saving geometric shape:" << shapeName;
    
    // Create directory if it doesn't exist
    QDir dir(QDir::currentPath() + "/drone_geojson");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Save the shape to a file
    QString filePath = QDir::currentPath() + "/drone_geojson/" + shapeName + ".geojson";
    QFile file(filePath);
    
    if (file.open(QIODevice::WriteOnly)) {
        file.write(geoJson.toUtf8());
        file.close();
        qDebug() << "Geometric shape saved to:" << filePath;
        
        // Emit signal that shape was saved
        emit geometricShapeSaved(shapeName);
    } else {
        qDebug() << "Error saving geometric shape:" << file.errorString();
    }
}

void Mapbox::updateDronePath(const QString& geoJson)
{
    qDebug() << "Updating drone path";
    
    // Save the updated path to the file
    if (!m_lastGeojsonPath.isEmpty()) {
        QFile file(m_lastGeojsonPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(geoJson.toUtf8());
            file.close();
            qDebug() << "Drone path updated in file:" << m_lastGeojsonPath;
            
            // Emit signal that path was updated
            emit dronePathUpdated();
        } else {
            qDebug() << "Error updating drone path:" << file.errorString();
        }
    }
    
    // Execute JavaScript to update the path on the map
    QString js = QString("if (window.updateDronePath) { window.updateDronePath(%1); }").arg(geoJson);
    m_webView->page()->runJavaScript(js, [](const QVariant &result) {
        // Handle result if needed
    });
}

void Mapbox::updateGeometricShapes(const QString& geoJson)
{
    qDebug() << "Updating geometric shapes";
    
    // Execute JavaScript to update the shapes on the map
    QString js = QString("if (window.updateGeometricShapes) { window.updateGeometricShapes(%1); }").arg(geoJson);
    m_webView->page()->runJavaScript(js, [](const QVariant &result) {
        // Handle result if needed
    });
}

void Mapbox::moveDroneAlongPath(const QJsonArray& coordinates, int currentIndex)
{
    qDebug() << "Moving drone along path, index:" << currentIndex;
    
    // Convert QJsonArray to string representation
    QJsonDocument doc(coordinates);
    QString coordsStr = doc.toJson(QJsonDocument::Compact);
    
    // Execute JavaScript to move the drone
    QString js = QString("if (window.moveDroneAlongPath) { window.moveDroneAlongPath(%1, %2); }")
                    .arg(coordsStr)
                    .arg(currentIndex);
    
    m_webView->page()->runJavaScript(js, [](const QVariant &result) {
        // Handle result if needed
    });
}