#include "../../include/components/MapViewer.h"
#include <QVBoxLayout>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QTimer>
#include <QDebug>

void DebugWebEnginePage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, 
                                                int lineNumber, const QString &sourceID) {
    qDebug() << "JS Console:" << level << message << "Line:" << lineNumber << "Source:" << sourceID;
}

MapViewer::MapViewer(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // Create and configure profile
    auto profile = new QWebEngineProfile("MapboxProfile", this);
    profile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    profile->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
    
    // Create web view and page
    m_webView = new QWebEngineView(this);
    
    // Configure settings
    auto settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
    
    // Replace the page with our debug page
    auto debugPage = new DebugWebEnginePage(profile, m_webView);
    m_webView->setPage(debugPage);
    
    layout->addWidget(m_webView);
    
    initializeMap();
}

MapViewer::~MapViewer()
{
}

void MapViewer::initializeMap()
{
    qDebug() << "Initializing map with Threebox and drone path...";
    const QString mapboxToken = "pk.eyJ1Ijoibmlja3lqMTIxIiwiYSI6ImNtN3N3eHFtcTB1MTkya3M4Mnc0dmQxanAifQ.gLJZYJe_zH9b9yxFxQZm6g";
    
    QString html = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset='utf-8'>
    <title>Threebox Drone</title>
            <meta name='viewport' content='initial-scale=1,maximum-scale=1,user-scalable=no'>
    <link href='https://api.mapbox.com/mapbox-gl-js/v2.2.0/mapbox-gl.css' rel='stylesheet'>
    <script src='https://api.mapbox.com/mapbox-gl-js/v2.2.0/mapbox-gl.js'></script>
    <script src='https://cdn.jsdelivr.net/npm/threebox-plugin@2.2.7/dist/threebox.min.js'></script>
            <style>
        body, html { margin: 0; height: 100%; }
        #map { width: 100%; height: 100%; }
            </style>
        </head>
        <body>
            <div id='map'></div>
            <script>
                    mapboxgl.accessToken = '%1';
                    
                    const map = new mapboxgl.Map({
                        container: 'map',
                        style: 'mapbox://styles/mapbox/dark-v11',
            center: [-3.44885, 40.49198],
            zoom: 13.4,
            pitch: 50,
            bearing: -13
        });

        let tb;
        let drone;

        map.on('style.load', function() {
            tb = new Threebox(
                map,
                map.getCanvas().getContext('webgl'),
                { defaultLights: true }
            );

            const flightPath = {
                "type": "Feature",
                "geometry": {
                    "type": "LineString",
                    "coordinates": [
                        [-3.459164318324355, 40.483196679459695, 0],
                        [-3.46032158100065006, 40.48405772625512, 0],
                        [-3.4601480276212726, 40.48464924045098, 0],
                        [-3.4605399688768728, 40.48492144503072, 0],
                        [-3.4544247306827174, 40.489871726679894, 0],
                        [-3.4419511970175165, 40.49989552385142, 100],
                        [-3.4199262740950473, 40.51776139362727, 800],
                        [-3.4064155093898023, 40.52744748436612, 1000],
                        [-3.394276165400413, 40.53214151673197, 1400],
                        [-3.3774962506359145, 40.53130304189972, 1800],
                        [-3.35977648690141, 40.523996322867305, 2000]
                    ]
                }
            };

            // Add the flight path line
            map.addSource('flight-path', {
                'type': 'geojson',
                'data': flightPath
            });

                            map.addLayer({
                'id': 'flight-path-line',
                'type': 'line',
                'source': 'flight-path',
                                'paint': {
                    'line-color': '#00ff00',
                    'line-width': 4,
                    'line-opacity': 0.8
                }
            });

                            map.addLayer({
                id: 'custom-layer',
                type: 'custom',
                renderingMode: '3d',
                onAdd: function() {
                    const options = {
                        obj: '.assets/models/drone.glb',
                        type: 'gltf',
                        scale: 1,
                        units: 'meters',
                        rotation: { x: 90, y: 0, z: 0 },
                        anchor: 'center'
                    };

                    tb.loadObj(options, function(model) {
                        drone = model;
                        drone.setCoords(flightPath.geometry.coordinates[0]);
                        drone.setRotation({ x: 0, y: 0, z: 135 });
                        tb.add(drone);

                        // Animate drone along path
                        let step = 0;
                        const coords = flightPath.geometry.coordinates;
                        
                        function animate() {
                            if (step < coords.length - 1) {
                                const start = coords[step];
                                const end = coords[step + 1];
                                const progress = (Date.now() % 1000) / 1000;
                                
                                const currentPos = [
                                    start[0] + (end[0] - start[0]) * progress,
                                    start[1] + (end[1] - start[1]) * progress,
                                    start[2] + (end[2] - start[2]) * progress
                                ];
                                
                                drone.setCoords(currentPos);
                                
                                if (progress >= 0.99) {
                                    step++;
                                }
                            } else {
                                step = 0;
                            }
                            
                            requestAnimationFrame(animate);
                        }
                        
                        animate();
                    });
                },
                render: function() {
                    tb.update();
                }
            });
        });
            </script>
        </body>
        </html>
    )";

    m_webView->setHtml(html.arg(mapboxToken), QUrl("https://api.mapbox.com"));
} 