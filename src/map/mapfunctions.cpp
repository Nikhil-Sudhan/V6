#include "../../include/map/mapfunctions.h"

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
        // Set the active drone
        setActiveDrone(vehicle);
        
        // Load the drone paths from file to ensure we have the latest data
        checkForFileChanges();
        
        // Display a message that the task has been assigned
        QMessageBox::information(nullptr, "Task Assigned", 
            "Task has been assigned to " + vehicle + ". The drone path will be displayed on the map.");
    }
}

void MapFunctions::saveGeometryData(const QString& geometryData)
{
    // This function has been moved to the Geometry class
    // This is just a stub to satisfy the interface
    qDebug() << "MapFunctions::saveGeometryData is deprecated. Use Geometry class instead.";
}

void MapFunctions::updateGeometryData(const QString& geometryData)
{
    // This function has been moved to the Geometry class
    // This is just a stub to satisfy the interface
    qDebug() << "MapFunctions::updateGeometryData is deprecated. Use Geometry class instead.";
}

void MapFunctions::saveGeometricShape(const QString& shapeData, const QString& shapeName)
{
    // This function has been moved to the Geometry class
    // This is just a stub to satisfy the interface
    qDebug() << "MapFunctions::saveGeometricShape is deprecated. Use Geometry class instead.";
}

void MapFunctions::loadGeometricShapes()
{
    // This function has been moved to the Geometry class
    // This is just a stub to satisfy the interface
    qDebug() << "MapFunctions::loadGeometricShapes is deprecated. Use Geometry class instead.";
}

void MapFunctions::deleteGeometricShape(const QString& shapeName)
{
    // This function has been moved to the Geometry class
    // This is just a stub to satisfy the interface
    qDebug() << "MapFunctions::deleteGeometricShape is deprecated. Use Geometry class instead.";
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
    
    // List of drone path files to delete
    QStringList dronePathFiles = {
        "/Atlas_path.geojson",
        "/Bolt_path.geojson",
        "/Barbarian_path.geojson",
        "/all_drone_paths.geojson"
    };
    
    // Delete each drone path file
    for (const QString& file : dronePathFiles) {
        QString fullPath = geojsonDir + file;
        QFile fileObj(fullPath);
        
        if (fileObj.exists()) {
            if (fileObj.remove()) {
                qDebug() << "Successfully deleted drone path file:" << fullPath;
            } else {
                qWarning() << "Failed to delete drone path file:" << fullPath << "-" << fileObj.errorString();
            }
        }
    }
}

MapFunctions::~MapFunctions()
{
    // Clear drone paths when the application is closed
    clearDronePathsOnExit();
}