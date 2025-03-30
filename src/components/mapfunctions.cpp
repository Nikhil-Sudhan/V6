#include "components/mapfunctions.h"

MapFunctions::MapFunctions(QWebEngineView* webView, QObject* parent)
    : QObject(parent)
    , m_webView(webView)
    , m_lastGeojsonHash("")
    , m_lastGeojsonPath("")
    , m_activeDroneName("Atlas")
    , m_currentPathIndex(0)
    , m_animationSpeed(1.0)
    , m_isAnimating(false)
{
    // Initialize drone path colors
    m_dronePathColors["Atlas"] = "#FF5733";    // Bright red/orange
    m_dronePathColors["Bolt"] = "#33A8FF";     // Bright blue
    m_dronePathColors["Barbarian"] = "#33FF57"; // Bright green
    
    // Initialize animation timer
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &MapFunctions::updateDronePosition);
}

void MapFunctions::setDronePositions(const QVector<QVector3D>& positions)
{
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

void MapFunctions::updateDronePath(const QJsonObject& geojsonData)
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
        m_webView->page()->runJavaScript(script, [](const QVariant &result) {
            qDebug() << "Map updated with drone paths";
        });
        
        m_lastGeojsonHash = newHash;
    }
}

void MapFunctions::setActiveDrone(const QString& droneName)
{
    qDebug() << "Setting active drone to:" << droneName;
    m_activeDroneName = droneName;
    
    // Initialize drone path colors if not already set
    if (m_dronePathColors.isEmpty()) {
        // Define a set of distinct colors for different drones
        m_dronePathColors["Atlas"] = "#FF5733";    // Bright red/orange
        m_dronePathColors["Bolt"] = "#33A8FF";     // Bright blue
        m_dronePathColors["Barbarian"] = "#33FF57"; // Bright green
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

void MapFunctions::saveGeometryData(const QString& geometryData)
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

void MapFunctions::updateGeometryData(const QString& geometryData)
{
    // This is the same as saveGeometryData now, just save the data
    saveGeometryData(geometryData);
}

void MapFunctions::saveGeometricShape(const QString& shapeData, const QString& shapeName)
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

void MapFunctions::loadGeometricShapes()
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

void MapFunctions::checkForFileChanges()
{
    // Define the directory path for GeoJSON files
    QString geojsonDirectory = QDir::currentPath() + "/drone_geojson";
    
    // Ensure the directory exists
    QDir directory(geojsonDirectory);
    if (!directory.exists()) {
        directory.mkpath(".");
        return; // No files to check yet
    }
    
    // Path to the main drone paths file
    QString dronePathsFilePath = geojsonDirectory + "/all_drone_paths.geojson";
    QFileInfo dronePathsFileInfo(dronePathsFilePath);
    
    // Check if the main file exists and has been modified
    if (dronePathsFileInfo.exists() && dronePathsFileInfo.isFile()) {
        QDateTime lastModifiedTime = dronePathsFileInfo.lastModified();
        
        // Reload the file if it has been modified since the last check
        if (!m_lastDronePathsFileModified.isValid() || lastModifiedTime > m_lastDronePathsFileModified) {
            QFile file(dronePathsFilePath);
            if (file.open(QIODevice::ReadOnly)) {
                QJsonDocument jsonDocument = QJsonDocument::fromJson(file.readAll());
                QJsonObject geoJsonObject = jsonDocument.object();
                file.close();
                
                // Update the map with the drone paths
                updateDronePath(geoJsonObject);
                
                // Update the last modified time
                m_lastDronePathsFileModified = lastModifiedTime;
            }
        }
    } else if (!dronePathsFileInfo.exists()) {
        // Create an empty GeoJSON file if it doesn't exist
        QFile file(dronePathsFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QJsonObject emptyGeoJsonObject;
            emptyGeoJsonObject["type"] = "FeatureCollection";
            emptyGeoJsonObject["features"] = QJsonArray();
            
            QJsonDocument jsonDocument(emptyGeoJsonObject);
            file.write(jsonDocument.toJson());
            file.close();
            
            // Set the initial last modified time
            m_lastDronePathsFileModified = QFileInfo(dronePathsFilePath).lastModified();
        }
    }
    
    // Check for changes in the geometric shapes file
    loadGeometricShapes();
}

void MapFunctions::deleteGeometricShape(const QString& shapeName)
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

void MapFunctions::clearDronePathsOnExit()
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

void MapFunctions::startDroneAnimation()
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

void MapFunctions::updateDronePosition()
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

void MapFunctions::confirmDroneTask(const QString& missionType, const QString& vehicle, const QString& prompt)
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

MapFunctions::~MapFunctions()
{
    // Clear drone paths when the application is closed
    clearDronePathsOnExit();
}