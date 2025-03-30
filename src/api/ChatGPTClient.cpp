#include "../../include/api/ChatGPTClient.h"
#include "../../include/database/DatabaseManager.h"
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QProcessEnvironment>
#include <QCoreApplication>

QString loadApiKey() {
    // Direct path to .profile file
    // QString profilePath = "home/sudhan/V6/.profile";
    // QFile file(profilePath);

    // if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //     QTextStream in(&file);
    //     QString fileContents = in.readAll();
    //     QStringList lines = fileContents.split('\n');
    //     for (const QString& line : lines) {
    //         if (line.startsWith("OPENAI_API_KEY=")) {
    //             QString key = line.mid(15).trimmed();
    //             qDebug() << "Found API key with length:" << key.length();
    //             return key;
    //         }
    //     }
    //     qDebug() << "No API key found in file contents";
    // } else {
    //     qDebug() << "Could not open .profile file:" << file.errorString();
    // }
    return "ADD THE OPENAI API KEY HERE";
}

ChatGPTClient& ChatGPTClient::instance()
{
    static ChatGPTClient instance;
    return instance;
}

ChatGPTClient::ChatGPTClient(QObject* parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this)), currentMissionId(-1)
{
    connect(networkManager, &QNetworkAccessManager::finished, this, &ChatGPTClient::handleNetworkReply);
    apiKey = loadApiKey();
    
    if (apiKey.isEmpty()) {
        qDebug() << "Warning: API key not found in .profile file or environment variables";
    }
}

ChatGPTClient::~ChatGPTClient()
{
}

void ChatGPTClient::sendPrompt(const QString& missionType, const QString& vehicle, const QString& prompt)
{
    if (apiKey.isEmpty()) {
        emit errorOccurred("API key is empty. Please check .profile file");
        return;
    }

    // Save mission data to database first
    if (!DatabaseManager::instance().saveMissionData(missionType, vehicle, prompt)) {
        emit errorOccurred("Failed to save mission data to database.");
        return;
    }
    
    // Get the last inserted mission ID
    QSqlQuery query;
    query.exec("SELECT last_insert_rowid()");
    if (query.next()) {
        currentMissionId = query.value(0).toInt();
    } else {
        emit errorOccurred("Failed to retrieve mission ID.");
        return;
    }
    
    // Prepare the API request
    QNetworkRequest request(QUrl("https://api.openai.com/v1/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // Set up authorization header
    QString authHeader = QString("Bearer %1").arg(apiKey);
    request.setRawHeader("Authorization", authHeader.toUtf8());
    
    // Create the JSON payload
    QJsonObject payload;
    payload["model"] = "gpt-3.5-turbo"; // Use appropriate model
    
    QJsonArray messages;
    
    // System message with GeoJSON format instructions
    QJsonObject systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = R"(
Generate a strictly formatted GeoJSON Feature defining a drone flight path that will be added to a FeatureCollection.

GeoJSON Format (Strict Rules)
You must return a single Feature object with:

"type": "Feature"

"properties": { 
  "name": "[Mission Name]",
  "drone": "[Drone Name]",
  "type": "path"
}

"geometry":

"type": "LineString"

"coordinates": Array of [longitude, latitude] pairs

Constraints of three drone placed 10 meters apart. start the geojson with the base location of the drone
Base Location of Atlas: [77.9695, 10.3624]
Base Location of Bolt: [77.9695, 10.36249]
Base Location of Barbarian: [77.96961, 10.3624]

At least 3 waypoints forming a "LineString"

Correct coordinate order: [longitude, latitude]

No extra properties or null values

Example Output:
{
  "type": "Feature",
  "properties": {
    "name": "Surveillance Flight Path",
    "drone": "Atlas",
    "type": "path"
  },
  "geometry": {
    "type": "LineString",
    "coordinates": [
      [77.9695, 10.3624],
      [77.9700, 10.3628],
      [77.9702, 10.3632]
    ]
  }
}

Keep the response strictly in this format every time. No variations.
# Notes
- Don't add comments in geojson data. 
- Give Code geojson data code alone.
- Ensure that coordinate values are accurate and appropriate for the designated flight region.
- GeoJSON is strictly typed; ensure that types (such as "Feature", "LineString") are specified verbatim.
- Name each feature clearly to reflect the mission or surveillance areas they cover.
- Include the drone name in the properties.)";
    messages.append(systemMessage);
    
    // User message with mission details
    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = QString("Mission Type: %1\nVehicle: %2\nPrompt: %3")
                            .arg(missionType, vehicle, prompt);
    messages.append(userMessage);
    
    payload["messages"] = messages;
    
    // Send the request
    QJsonDocument doc(payload);
    networkManager->post(request, doc.toJson());
}

void ChatGPTClient::handleNetworkReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(QString("Network error: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }
    
    QByteArray responseData = reply->readAll();
    
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    
    if (doc.isNull() || !doc.isObject()) {
        emit errorOccurred("Invalid response format from API");
        reply->deleteLater();
        return;
    }
    
    QJsonObject responseObj = doc.object();
    
    if (!responseObj.contains("choices") || !responseObj["choices"].isArray()) {
        emit errorOccurred("No choices in API response");
        reply->deleteLater();
        return;
    }
    
    QJsonArray choices = responseObj["choices"].toArray();
    if (choices.isEmpty()) {
        emit errorOccurred("Empty choices array in API response");
        reply->deleteLater();
        return;
    }
    
    QJsonObject messageObj = choices[0].toObject()["message"].toObject();
    QString content = messageObj["content"].toString();
    
    // Validate response
    if (content.isEmpty()) {
        emit errorOccurred("Empty response from API");
        reply->deleteLater();
        return;
    }
    
    // Parse the GeoJSON content (which should be a Feature)
    QJsonDocument featureDoc = QJsonDocument::fromJson(content.toUtf8());
    if (featureDoc.isNull() || !featureDoc.isObject()) {
        emit errorOccurred("Invalid GeoJSON format in response");
        reply->deleteLater();
        return;
    }
    
    // Get the Feature object
    QJsonObject feature = featureDoc.object();
    
    // Setup paths
    QString appPath = QCoreApplication::applicationDirPath();
    QString geojsonDir = appPath + "/drone_geojson";
    
    // Ensure directory exists
    QDir dir(geojsonDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Get vehicle name from database
    QSqlQuery query;
    query.prepare("SELECT vehicle FROM missions WHERE id = ?");
    query.addBindValue(currentMissionId);
    QString vehicleName = "drone";
    if (query.exec() && query.next()) {
        vehicleName = query.value(0).toString();
    }
    
    // Add the drone name and color to properties
    QJsonObject properties = feature.value("properties").toObject();
    properties["name"] = vehicleName;
    properties["type"] = "path";
    
    // Assign a color based on drone name for consistency
    QMap<QString, QString> droneColors;
    droneColors["Atlas"] = "#FF5733";    // Bright red/orange
    droneColors["Bolt"] = "#33A8FF";     // Bright blue
    droneColors["Barbarian"] = "#33FF57"; // Bright green
   
            
    // If the drone has a predefined color, use it
    if (droneColors.contains(vehicleName)) {
        properties["color"] = droneColors[vehicleName];
    } else {
        // Generate a random color for unknown drones
        QStringList colors = {"#ff00ff", "#00ffff", "#ff8800", "#8800ff", "#00ff88"};
        int index = qHash(vehicleName) % colors.size();
        properties["color"] = colors[index];
    }
            
    // Set the active flag for the current drone
    properties["active"] = true;
    feature["properties"] = properties;
    
    // Main GeoJSON file path
    QString mainFilename = QString("%1/all_drone_paths.geojson").arg(geojsonDir);
    
    // Create or load the main FeatureCollection file
    QJsonObject mainGeoJson;
    QFile mainFile(mainFilename);
    
    if (mainFile.exists() && mainFile.open(QIODevice::ReadOnly)) {
        // File exists, read and parse it
        QJsonDocument existingDoc = QJsonDocument::fromJson(mainFile.readAll());
        mainFile.close();
        
        if (!existingDoc.isNull() && existingDoc.isObject()) {
            mainGeoJson = existingDoc.object();
        } else {
            // Invalid existing file, create new structure
            mainGeoJson["type"] = "FeatureCollection";
            mainGeoJson["features"] = QJsonArray();
        }
    } else {
        // File doesn't exist or couldn't be opened, create new structure
        mainGeoJson["type"] = "FeatureCollection";
        mainGeoJson["features"] = QJsonArray();
    }
    
    // Add the new feature to the features array
    QJsonArray features;
    if (mainGeoJson.contains("features") && mainGeoJson["features"].isArray()) {
        features = mainGeoJson["features"].toArray();
        
        // Check if there's already a feature for this drone and remove it
        for (int i = features.size() - 1; i >= 0; i--) {
            QJsonObject existingFeature = features[i].toObject();
            if (existingFeature.contains("properties") && existingFeature["properties"].isObject()) {
                QJsonObject existingProps = existingFeature["properties"].toObject();
                if (existingProps.contains("name") && existingProps["name"].toString() == vehicleName) {
                    // Remove the existing feature for this drone
                    features.removeAt(i);
                    qDebug() << "Removed existing path for drone:" << vehicleName;
                }
            }
        }
    }
    
    // Add the new feature
    features.append(feature);
    mainGeoJson["features"] = features;
    
    // Save the updated FeatureCollection to the main file
    if (mainFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&mainFile);
        QJsonDocument mainDoc(mainGeoJson);
        out << mainDoc.toJson(QJsonDocument::Indented);
        mainFile.close();
        qDebug() << "Updated main GeoJSON file with new feature for drone:" << vehicleName;
    } else {
        emit errorOccurred(QString("Failed to save main GeoJSON file: %1").arg(mainFile.errorString()));
    }
    
    // Also save to individual file for backward compatibility
    QString individualFilename = QString("%1/%2_path.geojson").arg(geojsonDir, vehicleName);
    QFile individualFile(individualFilename);
    if (individualFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Create a FeatureCollection with just this feature
        QJsonObject singleDroneGeoJson;
        singleDroneGeoJson["type"] = "FeatureCollection";
        QJsonArray singleFeatureArray;
        singleFeatureArray.append(feature);
        singleDroneGeoJson["features"] = singleFeatureArray;
        
        QTextStream out(&individualFile);
        QJsonDocument singleDoc(singleDroneGeoJson);
        out << singleDoc.toJson(QJsonDocument::Indented);
        individualFile.close();
    }
    
    // Save response to database
    if (currentMissionId > 0) {
        if (!DatabaseManager::instance().saveChatGPTResponse(currentMissionId, content, "{}")) {
            emit errorOccurred("Failed to save response to database");
            reply->deleteLater();
            return;
        }
    }
    
    // Emit signal with response
    emit responseReceived(currentMissionId, content, "{}");
    
    reply->deleteLater();
}