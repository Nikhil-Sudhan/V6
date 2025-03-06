#include "mapviewer.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEnginePage>

MapViewer::MapViewer(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Mapbox 3D Viewer");
    resize(1024, 768);

    auto centralWidget = new QWidget(this);
    auto layout = new QVBoxLayout(centralWidget);
    
    // Create and configure profile
    auto profile = new QWebEngineProfile("MapboxProfile", this);
    profile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    profile->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
    
    // Create web view and page
    m_webView = new QWebEngineView(this);
    auto page = new QWebEnginePage(profile, m_webView);
    m_webView->setPage(page);
    
    // Configure settings
    auto settings = page->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
    
    layout->addWidget(m_webView);
    setCentralWidget(centralWidget);

    // Connect error handling
    connect(page, &QWebEnginePage::loadFinished, this, [this](bool ok) {
        if (!ok) {
            qDebug() << "Failed to load the page";
        }
    });

    initializeMap();
}

void MapViewer::initializeMap() {
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
                body { margin: 0; padding: 0; }
                #map { position: absolute; top: 0; bottom: 0; width: 100%; }
            </style>
        </head>
        <body>
            <div id='map'></div>
            <script>
                mapboxgl.accessToken = '%1';
                const map = new mapboxgl.Map({
                    container: 'map',
                    style: 'mapbox://styles/mapbox/streets-v12',
                    center: [-74.5, 40],
                    zoom: 9,
                    pitch: 60,
                    bearing: -60,
                    antialias: true,
                    preserveDrawingBuffer: true,
                    maxPitch: 85
                });

                map.on('load', () => {
                    map.addSource('mapbox-dem', {
                        'type': 'raster-dem',
                        'url': 'mapbox://mapbox.mapbox-terrain-dem-v1',
                        'tileSize': 512,
                        'maxzoom': 14
                    });
                    map.setTerrain({ 'source': 'mapbox-dem', 'exaggeration': 1.5 });

                    map.addLayer({
                        'id': '3d-buildings',
                        'source': 'composite',
                        'source-layer': 'building',
                        'filter': ['==', 'extrude', 'true'],
                        'type': 'fill-extrusion',
                        'minzoom': 15,
                        'paint': {
                            'fill-extrusion-color': '#aaa',
                            'fill-extrusion-height': [
                                'interpolate',
                                ['linear'],
                                ['zoom'],
                                15,
                                0,
                                15.05,
                                ['get', 'height']
                            ],
                            'fill-extrusion-base': [
                                'interpolate',
                                ['linear'],
                                ['zoom'],
                                15,
                                0,
                                15.05,
                                ['get', 'min_height']
                            ],
                            'fill-extrusion-opacity': 0.6
                        }
                    });
                });
            </script>
        </body>
        </html>
    )";

    m_webView->setHtml(html.arg(mapboxToken), QUrl("https://api.mapbox.com"));
} 