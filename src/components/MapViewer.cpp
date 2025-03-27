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
    // Initialize animation properties
    m_animationTimer = new QTimer(this);
    m_currentPathIndex = 0;
    m_animationSpeed = 5.0; // 5 meters per second
    m_isAnimating = false;
    
    // Connect animation timer to update slot
    connect(m_animationTimer, &QTimer::timeout, this, &MapViewer::updateDronePosition);
    
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
            <!-- Add Three.js for 3D model rendering -->
            <script src='https://unpkg.com/three@0.126.0/build/three.min.js'></script>
            <script src='https://unpkg.com/three@0.126.0/examples/js/loaders/GLTFLoader.js'></script>
            <style>
                body { margin: 0; padding: 0; }
                #map { position: absolute; top: 0; bottom: 0; width: 100%; }
                .mapboxgl-ctrl-group { background: #252525; }
                .mapboxgl-ctrl-group button { color: #00a6ff; }
                .mapboxgl-ctrl-group button:hover { background-color: #333; }
                #debug { position: absolute; top: 10px; left: 10px; background: rgba(0,0,0,0.7); color: white; padding: 10px; z-index: 1000; }
            </style>
        </head>
        <body>
            <div id='map'></div>
            <div id='debug'></div>
            <script>
                // Initialize the Qt web channel
                var qt_object;
                new QWebChannel(qt.webChannelTransport, function(channel) {
                    qt_object = channel.objects.qt_object;
                });
                
                const MAPBOX_TOKEN = '%1';
                const geojsonData = %2;
                
                // Debug function
                function debugLog(message) {
                    document.getElementById('debug').innerText = message;
                    console.log(message);
                }

                mapboxgl.accessToken = MAPBOX_TOKEN;
                
                // Variables for 3D drone model
                let droneModel = null;
                let droneModelLoaded = false;
                let customLayer = null;
                
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
                                    'line-width': 6 // Increased line width for inactive line strings
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
                        // Remove existing shapes source if it exists
                        if (map.getSource('geometric-shapes')) {
                            // Remove all layers that use this source
                            if (map.getLayer('geometric-shapes-fill')) {
                                map.removeLayer('geometric-shapes-fill');
                            }
                            if (map.getLayer('geometric-shapes-outline')) {
                                map.removeLayer('geometric-shapes-outline');
                            }
                            if (map.getLayer('geometric-shapes-points')) {
                                map.removeLayer('geometric-shapes-points');
                            }
                            if (map.getLayer('geometric-shapes-lines')) {
                                map.removeLayer('geometric-shapes-lines');
                            }
                            // Remove the source
                            map.removeSource('geometric-shapes');
                        }
                        
                        // Add the shapes source
                        map.addSource('geometric-shapes', {
                            'type': 'geojson',
                            'data': shapesData
                        });
                        
                        // Add a fill layer for polygons (including circles and rectangles)
                        map.addLayer({
                            'id': 'geometric-shapes-fill',
                            'type': 'fill',
                            'source': 'geometric-shapes',
                            'filter': ['==', '$type', 'Polygon'],
                            'paint': {
                                'fill-color': '#3388ff',
                                'fill-opacity': 0.4
                            }
                        });
                        
                        // Add an outline layer for polygons (including circles and rectangles)
                        map.addLayer({
                            'id': 'geometric-shapes-outline',
                            'type': 'line',
                            'source': 'geometric-shapes',
                            'filter': ['==', '$type', 'Polygon'],
                            'paint': {
                                'line-color': '#3388ff',
                                'line-width': 4
                            }
                        });
                        
                        // Add a layer for points
                        map.addLayer({
                            'id': 'geometric-shapes-points',
                            'type': 'circle',
                            'source': 'geometric-shapes',
                            'filter': ['==', '$type', 'Point'],
                            'paint': {
                                'circle-radius': 8,
                                'circle-color': '#3388ff'
                            }
                        });
                        
                        // Add a layer for lines
                        map.addLayer({
                            'id': 'geometric-shapes-lines',
                            'type': 'line',
                            'source': 'geometric-shapes',
                            'filter': ['==', '$type', 'LineString'],
                            'paint': {
                                'line-color': '#3388ff',
                                'line-width': 4
                            }
                        });
                    };
                    
                    // Handle draw.create event
                    map.on('draw.create', function(e) {
                        // Get the drawn features
                        const features = e.features;
                        
                        if (features.length > 0) {
                            // Create a GeoJSON with the new shape
                            const shapeGeoJson = {
                                "type": "FeatureCollection",
                                "features": features
                            };
                            
                            // Generate a unique name for the shape
                            const shapeName = 'shape_' + Date.now();
                            
                            // Convert to string and send to Qt
                            const shapeStr = JSON.stringify(shapeGeoJson);
                            if (qt_object) {
                                qt_object.saveGeometricShape(shapeStr, shapeName);
                            }
                            
                            // Clear the drawing
                            window.draw.deleteAll();
                        }
                    });
                    
                    // Handle draw.delete event
                    map.on('draw.delete', function(e) {
                        // Get the deleted features
                        const features = e.features;
                        
                        if (features.length > 0 && qt_object) {
                            // For each deleted feature, try to find its name and delete it
                            features.forEach(function(feature) {
                                if (feature.properties && feature.properties.name) {
                                    qt_object.deleteGeometricShape(feature.properties.name);
                                }
                            });
                        }
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
                    
                    // Function to load 3D drone model
                    window.loadDroneModel = function() {
                        debugLog("Loading drone model...");
                        
                        // Find the starting point of the active drone path
                        let startingPoint = [77.9695, 10.3624]; // Default starting point
                        
                        // Check if we have valid GeoJSON data with a path
                        if (geojsonData && geojsonData.features && geojsonData.features.length > 0) {
                            for (const feature of geojsonData.features) {
                                if (feature.geometry && feature.geometry.type === 'LineString' && 
                                    feature.geometry.coordinates && feature.geometry.coordinates.length > 0) {
                                    startingPoint = feature.geometry.coordinates[0];
                                    debugLog("Found starting point: " + JSON.stringify(startingPoint));
                                    break;
                                }
                            }
                        }
                        
                        // Create a custom layer for the 3D drone model
                        const modelOrigin = startingPoint;
                        const modelAltitude = 0;
                        const modelRotate = [Math.PI / 2, 0, 0];
                        
                        const modelAsMercatorCoordinate = mapboxgl.MercatorCoordinate.fromLngLat(
                            modelOrigin,
                            modelAltitude
                        );
                        
                        // Create a Three.js scene
                        const scene = new THREE.Scene();
                        const camera = new THREE.Camera();
                        const renderer = new THREE.WebGLRenderer({
                            canvas: map.getCanvas(),
                            context: map.getCanvas().getContext('webgl'),
                            antialias: true
                        });
                        
                        // Add lights to the scene
                        const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
                        directionalLight.position.set(0, -70, 100).normalize();
                        scene.add(directionalLight);
                        
                        const ambientLight = new THREE.AmbientLight(0xffffff, 0.5);
                        scene.add(ambientLight);
                        
                        // Load the drone model
                        const loader = new THREE.GLTFLoader();
                        // Try both model paths
                        const modelUrls = [
                            '/home/sudhan/V6/assets/models/drone.glb',
                            '/home/sudhan/V6/models/drone/drone.glb',
                            'assets/models/drone.glb',
                            'models/drone/drone.glb'
                        ];
                        
                        let modelLoadAttempt = 0;
                        
                        function attemptLoadModel() {
                            if (modelLoadAttempt >= modelUrls.length) {
                                debugLog("Failed to load model after trying all paths");
                                return;
                            }
                            
                            const modelUrl = modelUrls[modelLoadAttempt];
                            debugLog("Attempting to load model from: " + modelUrl);
                            
                            loader.load(modelUrl, 
                                // Success callback
                                function(gltf) {
                                    debugLog("Model loaded successfully from: " + modelUrl);
                                    droneModel = gltf.scene;
                                    
                                    // Scale the model
                                    const modelScale = 5;
                                    droneModel.scale.set(modelScale, modelScale, modelScale);
                                    
                                    scene.add(droneModel);
                                    droneModelLoaded = true;
                                    
                                    // Position the model at the start of the path
                                    droneModel.position.set(
                                        modelAsMercatorCoordinate.x,
                                        modelAsMercatorCoordinate.y,
                                        modelAsMercatorCoordinate.z
                                    );
                                    
                                    // Rotate the model
                                    droneModel.rotation.set(modelRotate[0], modelRotate[1], modelRotate[2]);
                                    
                                    debugLog("Drone model positioned at: " + JSON.stringify(modelOrigin));
                                }, 
                                // Progress callback
                                function(xhr) {
                                    debugLog("Loading model: " + Math.round(xhr.loaded / xhr.total * 100) + "%");
                                }, 
                                // Error callback
                                function(error) {
                                    debugLog("Error loading model from " + modelUrl + ": " + error.message);
                                    modelLoadAttempt++;
                                    attemptLoadModel(); // Try next path
                                }
                            );
                        }
                        
                        attemptLoadModel();
                        
                        // Create a custom layer for the 3D model
                        customLayer = {
                            id: '3d-drone-model',
                            type: 'custom',
                            renderingMode: '3d',
                            onAdd: function(map, gl) {
                                this.camera = camera;
                                this.scene = scene;
                                this.map = map;
                                
                                // Use the Mapbox GL JS map canvas for Three.js
                                this.renderer = renderer;
                                this.renderer.autoClear = false;
                            },
                            render: function(gl, matrix) {
                                if (!droneModelLoaded) return;
                                
                                const rotationX = new THREE.Matrix4().makeRotationAxis(
                                    new THREE.Vector3(1, 0, 0),
                                    modelRotate[0]
                                );
                                const rotationY = new THREE.Matrix4().makeRotationAxis(
                                    new THREE.Vector3(0, 1, 0),
                                    modelRotate[1]
                                );
                                const rotationZ = new THREE.Matrix4().makeRotationAxis(
                                    new THREE.Vector3(0, 0, 1),
                                    modelRotate[2]
                                );
                                
                                const m = new THREE.Matrix4().fromArray(matrix);
                                const l = new THREE.Matrix4()
                                    .makeTranslation(
                                        modelAsMercatorCoordinate.x,
                                        modelAsMercatorCoordinate.y,
                                        modelAsMercatorCoordinate.z
                                    )
                                    .scale(new THREE.Vector3(1, 1, 1))
                                    .multiply(rotationX)
                                    .multiply(rotationY)
                                    .multiply(rotationZ);
                                
                                this.camera.projectionMatrix = m.multiply(l);
                                this.renderer.resetState();
                                this.renderer.render(this.scene, this.camera);
                                this.map.triggerRepaint();
                            }
                        };
                        
                        // Add the custom layer to the map
                        map.addLayer(customLayer);
                    };
                    
                    // Signal to Qt that the map is ready
                    if (qt_object) {
                        qt_object.loadGeometricShapes();
                    }
                    
                    // Load the drone model after a short delay to ensure the map is fully loaded
                    setTimeout(function() {
                        window.loadDroneModel();
                    }, 2000);
                });
            </script>
        </body>
        </html>
    )";
    
    m_webView->setHtml(html.arg(mapboxToken, geojsonStr));
    
    // Load the drone model after the map is loaded
    QTimer::singleShot(2000, this, &MapViewer::loadDroneModel);
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

void MapViewer::deleteGeometricShape(const QString& shapeName)
{
    // Get current path
    QString geojsonDir = QDir::currentPath() + "/drone_geojson";
    
    // Ensure directory exists
    QDir dir(geojsonDir);
    if (!dir.exists()) {
        dir.mkpath(".");
        return; // No files to modify yet
    }
    
    // Path to the geometric shapes file
    QString shapesFilename = geojsonDir + "/geometric_shapes.geojson";
    
    // Load the shapes FeatureCollection file
    QJsonObject shapesGeoJson;
    QFile shapesFile(shapesFilename);
    
    if (shapesFile.exists() && shapesFile.open(QIODevice::ReadOnly)) {
        // File exists, read and parse it
        QJsonDocument existingDoc = QJsonDocument::fromJson(shapesFile.readAll());
        shapesFile.close();
        
        if (!existingDoc.isNull() && existingDoc.isObject()) {
            shapesGeoJson = existingDoc.object();
        } else {
            // Invalid existing file, nothing to delete
            qDebug() << "Invalid geometric shapes file format";
            return;
        }
    } else {
        // File doesn't exist or couldn't be opened, nothing to delete
        qDebug() << "Geometric shapes file does not exist or cannot be opened";
        return;
    }
    
    // Get the features array
    QJsonArray features;
    if (shapesGeoJson.contains("features") && shapesGeoJson["features"].isArray()) {
        features = shapesGeoJson["features"].toArray();
    } else {
        // No features to delete
        return;
    }
    
    // Create a new array without the features with the specified name
    QJsonArray updatedFeatures;
    bool shapeFound = false;
    
    for (int i = 0; i < features.size(); ++i) {
        QJsonObject feature = features[i].toObject();
        
        // Check if this feature has the name we're looking for
        if (feature.contains("properties") && feature["properties"].isObject()) {
            QJsonObject props = feature["properties"].toObject();
            
            if (props.contains("name") && props["name"].toString() == shapeName) {
                // Skip this feature (effectively deleting it)
                shapeFound = true;
                continue;
            }
        }
        
        // Keep this feature
        updatedFeatures.append(feature);
    }
    
    if (!shapeFound) {
        qDebug() << "No shape found with name:" << shapeName;
        return;
    }
    
    // Update the features array
    shapesGeoJson["features"] = updatedFeatures;
    
    // Save the updated FeatureCollection to the file
    if (shapesFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&shapesFile);
        QJsonDocument shapesDoc(shapesGeoJson);
        out << shapesDoc.toJson(QJsonDocument::Indented);
        shapesFile.close();
        qDebug() << "Deleted geometric shape:" << shapeName << "from file:" << shapesFilename;
        
        // Update the map with the shapes
        loadGeometricShapes();
    } else {
        qDebug() << "Failed to save geometric shapes file after deletion:" << shapesFilename << "-" << shapesFile.errorString();
    }
}

void MapViewer::clearDronePathsOnExit()
{
    // Get current path
    QString geojsonDir = QDir::currentPath() + "/drone_geojson";
    
    // Ensure directory exists
    QDir dir(geojsonDir);
    if (!dir.exists()) {
        return; // No directory, nothing to clear
    }
    
    // Path to the main drone paths file
    QString mainDronePathsFile = geojsonDir + "/all_drone_paths.geojson";
    
    // Create an empty GeoJSON structure
    QJsonObject emptyGeoJson;
    emptyGeoJson["type"] = "FeatureCollection";
    emptyGeoJson["features"] = QJsonArray();
    
    // Save the empty structure to the file
    QFile file(mainDronePathsFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonDocument doc(emptyGeoJson);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "Cleared drone paths file on exit:" << mainDronePathsFile;
    } else {
        qDebug() << "Failed to clear drone paths file on exit:" << mainDronePathsFile << "-" << file.errorString();
    }
}

void MapViewer::loadDroneModel()
{
    // Execute JavaScript to load the 3D drone model
    QString script = "if (typeof window.loadDroneModel === 'function') { window.loadDroneModel(); }";
    m_webView->page()->runJavaScript(script, [this](const QVariant &result) {
        qDebug() << "Drone model loading initiated";
    });
}

void MapViewer::startDroneAnimation()
{
    if (m_isAnimating) {
        return; // Already animating
    }
    
    // Get the active drone path
    QFile file(QDir::currentPath() + "/drone_geojson/" + m_activeDroneName + "_path.geojson");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open drone path file for animation";
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    QJsonObject geojsonObj = doc.object();
    if (!geojsonObj.contains("features") || !geojsonObj["features"].isArray()) {
        qDebug() << "Invalid GeoJSON format for animation";
        return;
    }
    
    QJsonArray features = geojsonObj["features"].toArray();
    if (features.isEmpty()) {
        qDebug() << "No features found in GeoJSON for animation";
        return;
    }
    
    // Find the path for the active drone
    for (int i = 0; i < features.size(); ++i) {
        QJsonObject feature = features[i].toObject();
        if (feature.contains("properties") && feature["properties"].isObject()) {
            QJsonObject props = feature["properties"].toObject();
            if (props.contains("name") && props["name"].toString() == m_activeDroneName) {
                if (feature.contains("geometry") && feature["geometry"].isObject()) {
                    QJsonObject geometry = feature["geometry"].toObject();
                    if (geometry.contains("coordinates") && geometry["coordinates"].isArray()) {
                        m_currentPath = geometry["coordinates"].toArray();
                        m_currentPathIndex = 0;
                        m_isAnimating = true;
                        
                        // Start the animation timer
                        m_animationTimer->start(100); // Update every 100ms
                        
                        // Move the drone to the starting position
                        updateDronePosition();
                        return;
                    }
                }
            }
        }
    }
    
    qDebug() << "No valid path found for the active drone";
}

void MapViewer::updateDronePosition()
{
    if (!m_isAnimating || m_currentPath.isEmpty() || m_currentPathIndex >= m_currentPath.size()) {
        m_animationTimer->stop();
        m_isAnimating = false;
        emit droneAnimationCompleted();
        return;
    }
    
    // Get the current coordinates
    QJsonArray coordArray = m_currentPath[m_currentPathIndex].toArray();
    if (coordArray.size() < 2) {
        qDebug() << "Invalid coordinate format";
        m_animationTimer->stop();
        m_isAnimating = false;
        return;
    }
    
    // Extract coordinates
    double lng = coordArray[0].toDouble();
    double lat = coordArray[1].toDouble();
    
    // Create a JavaScript array of coordinates for the path
    QString coordsJson = "[";
    for (int i = 0; i < m_currentPath.size(); ++i) {
        QJsonArray point = m_currentPath[i].toArray();
        if (i > 0) coordsJson += ",";
        coordsJson += "[" + QString::number(point[0].toDouble()) + "," + QString::number(point[1].toDouble()) + "]";
    }
    coordsJson += "]";
    
    // Call JavaScript function to move the drone
    QString script = QString("if (typeof window.moveDroneAlongPath === 'function') { "
                           "window.moveDroneAlongPath(%1, %2); }").arg(coordsJson).arg(m_currentPathIndex);
    m_webView->page()->runJavaScript(script);
    
    // Increment the path index for the next update
    m_currentPathIndex++;
    
    // If we've reached the end of the path, stop the animation
    if (m_currentPathIndex >= m_currentPath.size()) {
        m_animationTimer->stop();
        m_isAnimating = false;
        emit droneAnimationCompleted();
    }
}

void MapViewer::confirmDroneTask(const QString& missionType, const QString& vehicle, const QString& prompt)
{
    // Create a confirmation dialog
    QMessageBox msgBox;
    msgBox.setWindowTitle("Confirm Drone Task");
    msgBox.setText("Are you sure you want to assign this task to the drone?");
    msgBox.setInformativeText(QString("Mission Type: %1\nVehicle: %2\nPrompt: %3").arg(missionType, vehicle, prompt));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    
    int ret = msgBox.exec();
    
    if (ret == QMessageBox::Yes) {
        // User confirmed, start the drone animation
        startDroneAnimation();
    }
}

MapViewer::~MapViewer()
{
    // Clear drone paths when the application is closed
    clearDronePathsOnExit();
}