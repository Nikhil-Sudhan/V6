#include "map.h"

namespace MapContent {

QString getMapHtml(const QString& mapboxToken) {
    return R"(
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
                #debug { position: absolute; top: 10px; left: 10px; background: rgba(0,0,0,0.7); color: white; padding: 10px; z-index: 1000; }
            </style>
        </head>
        <body>
            <div id='map'></div>
            <div id='debug'></div>
            <script>
                // Debug function for logging
                function debugLog(message) {
                    document.getElementById('debug').innerText = message;
                    console.log(message);
                }

                // Initialize Mapbox
                mapboxgl.accessToken = ')" + mapboxToken + R"(';
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

                // Map load event handler
                map.on('load', () => {
                    // Add terrain
                    addTerrain();
                    
                    // Add 3D buildings
                    add3DBuildings();
                });
                
                // Add terrain to the map
                function addTerrain() {
                    map.addSource('mapbox-dem', {
                        'type': 'raster-dem',
                        'url': 'mapbox://mapbox.mapbox-terrain-dem-v1',
                        'tileSize': 512,
                        'maxzoom': 14
                    });
                    map.setTerrain({ 'source': 'mapbox-dem', 'exaggeration': 1.5 });
                }
                
                // Add 3D buildings to the map
                function add3DBuildings() {
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
                }
            </script>
        </body>
        </html>
    )";
}

} // namespace MapContent
