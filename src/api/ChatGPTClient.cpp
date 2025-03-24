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
    QString profilePath = "/home/sudhan/V6/.profile";
    QFile file(profilePath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString fileContents = in.readAll();
        QStringList lines = fileContents.split('\n');
        for (const QString& line : lines) {
            if (line.startsWith("OPENAI_API_KEY=")) {
                QString key = line.mid(15).trimmed();
                qDebug() << "Found API key with length:" << key.length();
                return key;
            }
        }
        qDebug() << "No API key found in file contents";
    } else {
        qDebug() << "Could not open .profile file:" << file.errorString();
    }
    
    return "";
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
    payload["model"] = "gpt-4"; // Use appropriate model
    
    QJsonArray messages;
    
    // System message with GeoJSON format instructions
    QJsonObject systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = R"(
Generate a strictly formatted GeoJSON file defining a drone flight path.

GeoJSON Format (Strict Rules)
type: "FeatureCollection"

features: Array of "Feature" objects

Each "Feature" has:

"type": "Feature"

"properties": { "name": "[Mission Name]" }

"geometry":

"type": "LineString"

"coordinates": Array of [longitude, latitude] pairs

Constraints
Base location: [77.9695, 10.3624]

At least 3 waypoints forming a "LineString"

Correct coordinate order: [longitude, latitude]

No extra properties or null values

Example Output:
{
  "type": "FeatureCollection",
  "features": [
    {
      "type": "Feature",
      "properties": {
        "name": "Surveillance Flight Path"
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
  ]
}
Keep the response strictly in this format every time. No variations.
# Notes
- Don't add comments in geojson data. 
- Give Code geojson data code alone.
- Ensure that coordinate values are accurate and appropriate for the designated flight region.
- GeoJSON is strictly typed; ensure that types (such as "FeatureCollection", "Feature", "LineString") are specified verbatim.
- Name each feature clearly to reflect the mission or surveillance areas they cover.)";
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
    
    // Parse the GeoJSON content
    QJsonDocument geojsonDoc = QJsonDocument::fromJson(content.toUtf8());
    if (geojsonDoc.isNull() || !geojsonDoc.isObject()) {
        emit errorOccurred("Invalid GeoJSON format in response");
        reply->deleteLater();
        return;
    }
    
    // Save GeoJSON to file
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
    
    // Save to file
    QString filename = QString("%1/%2_path.geojson").arg(geojsonDir, vehicleName);
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << geojsonDoc.toJson(QJsonDocument::Indented);
        file.close();
    } else {
        emit errorOccurred(QString("Failed to save GeoJSON to file: %1").arg(file.errorString()));
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