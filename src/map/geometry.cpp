#include "../../include/map/geometry.h"

Geometry::Geometry(QWebEngineView* webView, QObject* parent) : QObject(parent), m_webView(webView)
{
    // Initialize the last modified time as invalid
    m_lastShapesFileModified = QDateTime();
    
    // Load geometric shapes on initialization
    loadGeometricShapes();
}

Geometry::~Geometry()
{
    // Clean up all geometry data when object is destroyed
    clearAllGeometryOnExit();
}

void Geometry::saveGeometryData(const QString& geometryData)
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

void Geometry::updateGeometryData(const QString& geometryData)
{
    // This is the same as saveGeometryData now, just save the data
    saveGeometryData(geometryData);
}

void Geometry::saveGeometricShape(const QString& shapeData, const QString& shapeName)
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

void Geometry::loadGeometricShapes()
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
            m_webView->page()->runJavaScript(script, [](const QVariant &result) {
                qDebug() << "Map updated with geometric shapes";
            });
            
            // Update last modified time
            m_lastShapesFileModified = lastModified;
        } else {
            qDebug() << "Failed to open geometric shapes file:" << shapesFilename;
        }
    }
}

void Geometry::deleteGeometricShape(const QString& shapeName)
{
    // Get current path
    QString geojsonDir = QDir::currentPath() + "/drone_geojson";
    QString shapesFilename = geojsonDir + "/geometric_shapes.geojson";
    
    // Read the existing file
    QFile file(shapesFilename);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        qDebug() << "Geometric shapes file does not exist or cannot be opened";
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Invalid geometric shapes file format";
        return;
    }
    
    QJsonObject shapesObj = doc.object();
    if (!shapesObj.contains("features") || !shapesObj["features"].isArray()) {
        qDebug() << "Invalid geometric shapes structure";
        return;
    }
    
    // Get the features array and filter out the shape to delete
    QJsonArray features = shapesObj["features"].toArray();
    QJsonArray filteredFeatures;
    
    for (int i = 0; i < features.size(); ++i) {
        QJsonObject feature = features[i].toObject();
        
        // Check if this feature has the name we're looking for
        if (feature.contains("properties") && feature["properties"].isObject()) {
            QJsonObject props = feature["properties"].toObject();
            
            if (props.contains("name") && props["name"].toString() != shapeName) {
                // Keep this feature as it's not the one we're deleting
                filteredFeatures.append(feature);
            }
        } else {
            // No properties or no name, keep it
            filteredFeatures.append(feature);
        }
    }
    
    // Update the features array
    shapesObj["features"] = filteredFeatures;
    
    // Save the updated file
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        QJsonDocument updatedDoc(shapesObj);
        out << updatedDoc.toJson(QJsonDocument::Indented);
        file.close();
        qDebug() << "Deleted geometric shape:" << shapeName;
        
        // Update the map with the shapes
        loadGeometricShapes();
    } else {
        qDebug() << "Failed to save geometric shapes file after deletion";
    }
}

void Geometry::clearAllGeometryOnExit()
{
    // Get application path
    QString geojsonDir = QDir::currentPath() + "/drone_geojson";
    
    // Check if directory exists
    QDir dir(geojsonDir);
    if (!dir.exists()) {
        qDebug() << "GeoJSON directory does not exist, nothing to clean up";
        return;
    }
    
    // List of GeoJSON files to delete
    QStringList filesToDelete = {
        "/geomatics.geojson",
        "/geometric_shapes.geojson",
        "/all_drone_paths.geojson"
    };
    
    // Delete each file
    for (const QString& file : filesToDelete) {
        QString fullPath = geojsonDir + file;
        QFile fileObj(fullPath);
        
        if (fileObj.exists()) {
            if (fileObj.remove()) {
                qDebug() << "Successfully deleted:" << fullPath;
            } else {
                qWarning() << "Failed to delete:" << fullPath << "-" << fileObj.errorString();
            }
        }
    }
    
    // We're not deleting drone path files anymore as per the request
    // We're also not removing the directory itself
}