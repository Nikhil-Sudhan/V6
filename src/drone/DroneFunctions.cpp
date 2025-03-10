#include "../../include/drone/DroneFunctions.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QMap>
#include <QCoreApplication>
#include <QDateTime>

// Static map to store drone paths and metadata
static QMap<QString, QJsonArray> dronePathsMap;
static QMap<QString, QString> droneOperationTypes;

// Base location coordinates
const double BASE_LONGITUDE = 10.3624;
const double BASE_LATITUDE = 77.9695;

// Helper function to create waypoint feature
QJsonObject createWaypointFeature(double lon, double lat, double alt, const QString& type, const QString& color = "#ffffff") {
    QJsonObject feature;
    feature["type"] = "Feature";
    
    QJsonObject properties;
    properties["type"] = type;
    properties["altitude"] = alt;
    properties["color"] = color;
    properties["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    feature["properties"] = properties;
    
    QJsonObject geometry;
    geometry["type"] = "Point";
    QJsonArray coordinates = {lon, lat, alt};
    geometry["coordinates"] = coordinates;
    feature["geometry"] = geometry;
    
    return feature;
}

// Helper function to create path feature
QJsonObject createPathFeature(const QJsonArray& coordinates, const QString& color = "#ff0000") {
    QJsonObject feature;
    feature["type"] = "Feature";
    
    QJsonObject properties;
    properties["type"] = "path";
    properties["name"] = "Drone Flight Path";
    properties["color"] = color;
    properties["description"] = "3D flight trajectory";
    properties["startTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    feature["properties"] = properties;
    
    QJsonObject geometry;
    geometry["type"] = "LineString";
    geometry["coordinates"] = coordinates;
    feature["geometry"] = geometry;
    
    return feature;
}

// Helper function to save flight path GeoJSON
bool saveFlightPath(const QString& droneName) {
    // Get the application directory path
    QString appPath = QCoreApplication::applicationDirPath();
    QDir dir(appPath);
    dir.cdUp();
    
    // Create drone_geojson directory if it doesn't exist
    QDir geojsonDir(dir.absolutePath() + "/drone_geojson");
    if (!geojsonDir.exists()) {
        geojsonDir.mkpath(".");
    }
    
    QJsonObject featureCollection;
    featureCollection["type"] = "FeatureCollection";
    
    // Add metadata
    QJsonObject metadata;
    metadata["droneName"] = droneName;
    metadata["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    metadata["baseLocation"] = QJsonArray({BASE_LONGITUDE, BASE_LATITUDE});
    metadata["description"] = "Flight path from base to target location";
    featureCollection["metadata"] = metadata;
    
    QJsonArray features;
    
    // Add base location marker
    features.append(createWaypointFeature(BASE_LONGITUDE, BASE_LATITUDE, 0, "base", "#0000ff"));
    
    // Add flight path
    features.append(createPathFeature(dronePathsMap[droneName]));
    
    // Add waypoint markers for each point in the path
    QJsonArray coords = dronePathsMap[droneName];
    for (int i = 0; i < coords.size(); ++i) {
        QJsonArray point = coords[i].toArray();
        QString waypointType;
        QString color;
        
        if (i == 0) {
            waypointType = "takeoff_start";
            color = "#00ff00";
        }
        else if (i == coords.size() - 1) {
            waypointType = "landing_point";
            color = "#ff0000";
        }
        else {
            waypointType = "waypoint";
            color = "#ffffff";
        }
        
        features.append(createWaypointFeature(
            point[0].toDouble(),
            point[1].toDouble(),
            point[2].toDouble(),
            waypointType,
            color
        ));
    }
    
    featureCollection["features"] = features;
    
    // Save to file with absolute path
    QJsonDocument doc(featureCollection);
    QString filename = geojsonDir.absoluteFilePath(QString("%1_path.geojson").arg(droneName));
    QFile file(filename);
    
    qDebug() << "Saving GeoJSON to:" << filename;
    
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "Successfully saved GeoJSON with" << features.size() << "features";
        return true;
    }
    
    qDebug() << "Failed to save GeoJSON:" << file.errorString();
    return false;
}

bool DroneFunctions::takeoff(double x, double y, double z, const QString& droneName) {
    if (!dronePathsMap.contains(droneName)) {
        dronePathsMap[droneName] = QJsonArray();
    }
    
    // Start from base location at ground level
    QJsonArray basePoint = {BASE_LONGITUDE, BASE_LATITUDE, 0};
    dronePathsMap[droneName].append(basePoint);
    
    // Rise to target altitude at base location
    QJsonArray takeoffPoint = {BASE_LONGITUDE, BASE_LATITUDE, z};
    dronePathsMap[droneName].append(takeoffPoint);
    
    // Move to target position maintaining altitude
    QJsonArray targetPoint = {x, y, z};
    dronePathsMap[droneName].append(targetPoint);
    
    droneOperationTypes[droneName] = "takeoff";
    return saveFlightPath(droneName);
}

bool DroneFunctions::move(double x, double y, double z, const QString& droneName) {
    if (!dronePathsMap.contains(droneName)) {
        dronePathsMap[droneName] = QJsonArray();
        // If no previous points, start from base
        QJsonArray basePoint = {BASE_LONGITUDE, BASE_LATITUDE, z};
        dronePathsMap[droneName].append(basePoint);
    }
    
    QJsonArray point = {x, y, z};
    dronePathsMap[droneName].append(point);
    
    droneOperationTypes[droneName] = "move";
    return saveFlightPath(droneName);
}

bool DroneFunctions::land(double x, double y, double z, const QString& droneName) {
    if (!dronePathsMap.contains(droneName)) {
        dronePathsMap[droneName] = QJsonArray();
    }
    
    // Add approach point at current altitude
    QJsonArray approachPoint = {x, y, z};
    dronePathsMap[droneName].append(approachPoint);
    
    // Add landing point at ground level
    QJsonArray landingPoint = {x, y, 0};
    dronePathsMap[droneName].append(landingPoint);
    
    droneOperationTypes[droneName] = "land";
    return saveFlightPath(droneName);
}

bool DroneFunctions::arm(const QString& droneName) {
    // Initialize empty path starting from base location
    dronePathsMap[droneName] = QJsonArray();
    QJsonArray basePoint = {BASE_LONGITUDE, BASE_LATITUDE, 0};
    dronePathsMap[droneName].append(basePoint);
    droneOperationTypes[droneName] = "arm";
    return true;
}

bool DroneFunctions::disarm(const QString& droneName) {
    droneOperationTypes[droneName] = "disarm";
    return saveFlightPath(droneName);
} 