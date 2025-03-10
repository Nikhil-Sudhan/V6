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
    
    // Debug output
    qDebug() << "API Key length:" << apiKey.length();
    qDebug() << "First 10 chars of API key:" << apiKey.left(10);
    qDebug() << "Auth header length:" << authHeader.length();
    qDebug() << "First 20 chars of auth header:" << authHeader.left(20);
    
    // Create the JSON payload
    QJsonObject payload;
    payload["model"] = "gpt-4"; // Use appropriate model
    
    QJsonArray messages;
    
    // System message with function definitions
    QJsonObject systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = R"(You are a UAV control assistant. You help with mission planning and execution.
Base location is at [10.3624, 77.9695].
When given a command:
1. Always start by arming the drone
2. Then takeoff from the base location
3. Execute any moves to target locations
4. Finally land at the specified location
5. End by disarming the drone

Break down complex commands into these basic operations.
All coordinates should be in [longitude, latitude] format.)";
    messages.append(systemMessage);
    
    // User message with mission details
    QJsonObject userMessage;
    userMessage["role"] = "user";
    userMessage["content"] = QString("Mission Type: %1\nVehicle: %2\nPrompt: %3")
                            .arg(missionType, vehicle, prompt);
    messages.append(userMessage);
    
    payload["messages"] = messages;
    
    // Define available functions
    QJsonArray functions;
    
    // Takeoff function
    QJsonObject takeoffFunction;
    takeoffFunction["name"] = "takeoff";
    takeoffFunction["description"] = "Command the drone to take off from current position to specified altitude";
    QJsonObject takeoffParams;
    takeoffParams["type"] = "object";
    takeoffParams["properties"] = QJsonObject({
        {"x", QJsonObject({
            {"type", "number"},
            {"description", "Longitude"}
        })},
        {"y", QJsonObject({
            {"type", "number"},
            {"description", "Latitude"}
        })},
        {"altitude", QJsonObject({
            {"type", "number"},
            {"description", "Target altitude in meters (typically between 10-500)"}
        })},
        {"drone", QJsonObject({
            {"type", "string"},
            {"description", "Name of the drone"}
        })}
    });
    takeoffParams["required"] = QJsonArray({"x", "y", "altitude", "drone"});
    takeoffFunction["parameters"] = takeoffParams;
    functions.append(takeoffFunction);
    
    // Move function
    QJsonObject moveFunction;
    moveFunction["name"] = "move";
    moveFunction["description"] = "Command the drone to move to a specific position while maintaining altitude";
    QJsonObject moveParams;
    moveParams["type"] = "object";
    moveParams["properties"] = QJsonObject({
        {"x", QJsonObject({
            {"type", "number"},
            {"description", "Target longitude"}
        })},
        {"y", QJsonObject({
            {"type", "number"},
            {"description", "Target latitude"}
        })},
        {"z", QJsonObject({
            {"type", "number"},
            {"description", "Target altitude in meters"}
        })},
        {"drone", QJsonObject({
            {"type", "string"},
            {"description", "Name of the drone"}
        })}
    });
    moveParams["required"] = QJsonArray({"x", "y", "z", "drone"});
    moveFunction["parameters"] = moveParams;
    functions.append(moveFunction);
    
    // Land function
    QJsonObject landFunction;
    landFunction["name"] = "land";
    landFunction["description"] = "Command the drone to land at the current position";
    QJsonObject landParams;
    landParams["type"] = "object";
    landParams["properties"] = QJsonObject({
        {"x", QJsonObject({
            {"type", "number"},
            {"description", "Landing longitude"}
        })},
        {"y", QJsonObject({
            {"type", "number"},
            {"description", "Landing latitude"}
        })},
        {"z", QJsonObject({
            {"type", "number"},
            {"description", "Final approach altitude (typically 0)"}
        })},
        {"drone", QJsonObject({
            {"type", "string"},
            {"description", "Name of the drone"}
        })}
    });
    landParams["required"] = QJsonArray({"x", "y", "z", "drone"});
    landFunction["parameters"] = landParams;
    functions.append(landFunction);
    
    payload["functions"] = functions;
    payload["function_call"] = "auto";
    
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
    qDebug() << "Raw response data:" << responseData;
    
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
    QString functionCalls;
    
    // Check for function calls
    if (messageObj.contains("function_call")) {
        QJsonObject functionCall = messageObj["function_call"].toObject();
        functionCalls = QJsonDocument(functionCall).toJson();
        
        // If content is null but we have a function call, create a descriptive content
        if (content.isEmpty()) {
            QString functionName = functionCall["name"].toString();
            QString args = functionCall["arguments"].toString();
            content = QString("Executing function: %1 with arguments: %2").arg(functionName, args);
        }
    }
    
    // Validate response
    if (content.isEmpty() && functionCalls.isEmpty()) {
        emit errorOccurred("Empty response and no function calls from API");
        reply->deleteLater();
        return;
    }
    
    qDebug() << "Response content:" << content;
    qDebug() << "Function calls:" << functionCalls;
    
    // Save response to database
    if (currentMissionId > 0) {
        if (!DatabaseManager::instance().saveChatGPTResponse(currentMissionId, content, functionCalls.isEmpty() ? "{}" : functionCalls)) {
            emit errorOccurred("Failed to save response to database");
            reply->deleteLater();
            return;
        }
    }
    
    // Emit signal with response
    emit responseReceived(currentMissionId, content, functionCalls);
    
    reply->deleteLater();
} 