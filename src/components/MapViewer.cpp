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
    qDebug() << "Initializing map with Mapbox...";
    const QString mapboxToken = "pk.eyJ1Ijoibmlja3lqMTIxIiwiYSI6ImNtN3N3eHFtcTB1MTkya3M4Mnc0dmQxanAifQ.gLJZYJe_zH9b9yxFxQZm6g";
    
    QString html = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset='utf-8'>
            <title>Mapbox 3D Map</title>
            <meta name='viewport' content='initial-scale=1,maximum-scale=1,user-scalable=no'>
            <meta http-equiv='Content-Security-Policy' content="default-src * blob: data: 'unsafe-inline' 'unsafe-eval'; script-src * blob: data: 'unsafe-inline' 'unsafe-eval'; connect-src * blob: data:; img-src * blob: data:; frame-src *; style-src * 'unsafe-inline';">
            <link href='https://api.mapbox.com/mapbox-gl-js/v2.15.0/mapbox-gl.css' rel='stylesheet'>
            <script src='https://api.mapbox.com/mapbox-gl-js/v2.15.0/mapbox-gl.js'></script>
            <style>
                body { margin: 0; padding: 0; background-color: #1e1e1e; }
                #map { position: absolute; top: 0; bottom: 0; width: 100%; }
                .mapboxgl-missing-css { display: none; }
                .mapboxgl-canvas { outline: none; }
                .error-message {
                    position: absolute;
                    top: 10px;
                    left: 10px;
                    z-index: 999;
                    background: rgba(255, 0, 0, 0.8);
                    color: white;
                    padding: 10px;
                    border-radius: 4px;
                    max-width: 80%;
                }
            </style>
        </head>
        <body>
            <div id='map'></div>
            <script>
                console.log('Initializing Mapbox...');
                
                // Add error handling
                function handleMapError(err) {
                    console.error('Mapbox Error:', err);
                    const errorDiv = document.createElement('div');
                    errorDiv.className = 'error-message';
                    errorDiv.textContent = 'Map Error: ' + (err.message || JSON.stringify(err));
                    document.body.appendChild(errorDiv);
                }
                
                try {
                    mapboxgl.accessToken = '%1';
                    console.log('Access token set:', mapboxgl.accessToken);
                    
                    const map = new mapboxgl.Map({
                        container: 'map',
                        style: 'mapbox://styles/mapbox/dark-v11',
                        center: [-74.0060, 40.7128], // New York City coordinates
                        zoom: 15.5,
                        pitch: 60,
                        bearing: -60,
                        antialias: true,
                        preserveDrawingBuffer: true,
                        maxPitch: 85
                    });
                    
                    console.log('Map instance created');
                    
                    map.on('error', function(e) {
                        handleMapError(e.error || e);
                    });
                    
                    // Wait for style to load before adding 3D features
                    map.on('style.load', () => {
                        console.log('Style loaded, adding 3D features...');
                        
                        try {
                            // Add terrain source and layer
                            map.addSource('mapbox-dem', {
                                'type': 'raster-dem',
                                'url': 'mapbox://mapbox.terrain-rgb',
                                'tileSize': 512,
                                'maxzoom': 14
                            });
                            
                            // Add terrain with higher exaggeration
                            map.setTerrain({ 'source': 'mapbox-dem', 'exaggeration': 1.5 });
                            
                            // Add sky layer for better 3D effect
                            map.addLayer({
                                'id': 'sky',
                                'type': 'sky',
                                'paint': {
                                    'sky-type': 'atmosphere',
                                    'sky-atmosphere-sun': [0.0, 90.0],
                                    'sky-atmosphere-sun-intensity': 15
                                }
                            });

                            // Add 3D building layer
                            map.addLayer({
                                'id': '3d-buildings',
                                'source': 'composite',
                                'source-layer': 'building',
                                'filter': ['==', 'extrude', 'true'],
                                'type': 'fill-extrusion',
                                'minzoom': 15,
                                'paint': {
                                    'fill-extrusion-color': '#aaa',
                                    'fill-extrusion-height': ['get', 'height'],
                                    'fill-extrusion-base': ['get', 'min_height'],
                                    'fill-extrusion-opacity': 0.6
                                }
                            });

                            console.log('3D features added successfully');
                        } catch (e) {
                            console.error('Error setting up 3D features:', e);
                            handleMapError(e);
                        }
                    });
                    
                    map.on('load', () => {
                        console.log('Map loaded successfully');
                        
                        // Add navigation controls
                        map.addControl(new mapboxgl.NavigationControl());
                        
                        // Add a marker for visibility testing
                        new mapboxgl.Marker()
                            .setLngLat([-74.0060, 40.7128])
                            .addTo(map);
                            
                        // Try to switch to custom style
                        try {
                            map.setStyle('mapbox://styles/nickyj121/cm6tjxn1o017h01r5h9g03n5o');
                        } catch (e) {
                            console.error('Error setting custom style:', e);
                            handleMapError(e);
                        }
                    });
                } catch (e) {
                    handleMapError(e);
                }
            </script>
        </body>
        </html>
    )";

    m_webView->setHtml(html.arg(mapboxToken), QUrl("https://api.mapbox.com"));
    
    // Add a timer to check if map is still white after a few seconds
    QTimer::singleShot(5000, this, [this]() {
        qDebug() << "Checking map status after 5 seconds...";
        m_webView->page()->runJavaScript("document.body.innerHTML", [](const QVariant &result) {
            qDebug() << "Current HTML:" << result.toString().left(100) + "...";
        });
        
        // Try to take a screenshot of the map
        m_webView->page()->runJavaScript(
            "document.querySelector('.mapboxgl-canvas').toDataURL('image/png').substring(0, 100)",
            [](const QVariant &result) {
                qDebug() << "Canvas data:" << result.toString();
            }
        );
    });
} 