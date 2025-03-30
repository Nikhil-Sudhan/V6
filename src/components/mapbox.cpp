#include "../../include/components/mapbox.h"

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
    
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        geojsonStr = doc.toJson(QJsonDocument::Compact);
        file.close();
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
                
                // Logging function
                function debugLog(message) {
                    console.log(message);
                }

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
                    map.addControl(window.draw, 'top-right');
                    
                    // Add navigation control
                    map.addControl(new mapboxgl.NavigationControl(), 'top-right');
                    
                    // Add custom drone path source and layer
                    map.addSource('drone-path', {
                        type: 'geojson',
                        data: geojsonData
                    });
                    
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
                    
                    // Add a layer for the drone marker
                    // map.addSource('drone-position', {
                    //     type: 'geojson',
                    //     data: {
                    //         type: 'FeatureCollection',
                    //         features: []
                    //     }
                    // });
                    
                    // map.addLayer({
                    //     id: 'drone-position-layer',
                    //     type: 'circle',
                    //     source: 'drone-position',
                    //     paint: {
                    //         'circle-radius': 10,
                    //         'circle-color': '#ff0000',
                    //         'circle-stroke-width': 2,
                    //         'circle-stroke-color': '#ffffff'
                    //     }
                    // });
                    
                    // Add a source for geometric shapes
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
                    
                    // Listen for draw.create events
                    map.on('draw.create', function(e) {
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
                                qt_object.saveGeometricShape(JSON.stringify(geoJson), shapeName);
                            }
                        }
                    });
                    
                    // Function to update drone positions
                    window.updateDronePositions = function(positions) {
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
                        
                        map.getSource('drone-position').setData(geojson);
                    };
                    
                    // Function to update drone path
                    window.updateDronePath = function(geojsonData) {
                        map.getSource('drone-path').setData(geojsonData);
                    };
                    
                    // Function to update geometric shapes
                    window.updateGeometricShapes = function(geojsonData) {
                        map.getSource('geometric-shapes').setData(geojsonData);
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
                            
                            map.getSource('drone-position').setData(geojson);
                            
                            // Center the map on the drone position
                            map.flyTo({
                                center: position,
                                speed: 0.5,
                                curve: 1,
                                essential: true
                            });
                        }
                    };
                });
            </script>
        </body>
        </html>
    )";
    
    // Replace the token and geojson data
    html = html.arg(m_mapboxToken).arg(geojsonStr);
    
    // Load the HTML into the web view
    m_webView->setHtml(html);
}