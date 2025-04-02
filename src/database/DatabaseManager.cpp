#include "../../include/database/DatabaseManager.h"
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent), initialized(false)
{
}

DatabaseManager::~DatabaseManager()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::initialize()
{
    if (initialized) {
        return true;
    }
    
    // Set up database in app data location
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    qDebug() << "Database path:" << dataPath + "/uav_missions.db";
    
    // Initialize database
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dataPath + "/uav_missions.db");
    
    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return false;
    }
    qDebug() << "Database opened successfully";
    
    // Create tables if they don't exist
    if (!createTables()) {
        qDebug() << "Error creating tables";
        return false;
    }
    qDebug() << "Tables created successfully";
    
    initialized = true;
    return true;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query;
    
    // Create missions table
    QString createMissionsTable = "CREATE TABLE IF NOT EXISTS missions ("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                "mission_type TEXT NOT NULL, "
                                "mission_title TEXT, "
                                "user_name TEXT, "
                                "vehicle TEXT NOT NULL, "
                                "prompt TEXT NOT NULL, "
                                "asset_objective TEXT, "
                                "status TEXT DEFAULT 'pending', "
                                "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)";
                                
    if (!query.exec(createMissionsTable)) {
        qDebug() << "Error creating missions table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Missions table created/exists";
    
    // Create responses table
    QString createResponsesTable = "CREATE TABLE IF NOT EXISTS responses ("
                                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "mission_id INTEGER NOT NULL, "
                                 "response TEXT NOT NULL, "
                                 "functions TEXT NOT NULL, "
                                 "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
                                 "FOREIGN KEY (mission_id) REFERENCES missions(id))";
                                 
    if (!query.exec(createResponsesTable)) {
        qDebug() << "Error creating responses table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Responses table created/exists";
    
    return true;
}

bool DatabaseManager::saveMissionData(const QString& missionType, const QString& vehicle, const QString& prompt)
{
    if (!initialized && !initialize()) {
        qDebug() << "Database not initialized";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO missions (mission_type, vehicle, prompt) "
                 "VALUES (:mission_type, :vehicle, :prompt)");
    query.bindValue(":mission_type", missionType);
    query.bindValue(":vehicle", vehicle);
    query.bindValue(":prompt", prompt);
    
    if (!query.exec()) {
        qDebug() << "Error saving mission data:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Mission data saved successfully. ID:" << query.lastInsertId().toInt();
    return true;
}

bool DatabaseManager::saveEnhancedMissionData(const QString& missionType, const QString& missionTitle, 
                                           const QString& userName, const QString& vehicle, 
                                           const QString& prompt, const QString& assetObjective)
{
    if (!initialized && !initialize()) {
        qDebug() << "Database not initialized";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO missions (mission_type, mission_title, user_name, vehicle, prompt, asset_objective) "
                 "VALUES (:mission_type, :mission_title, :user_name, :vehicle, :prompt, :asset_objective)");
    query.bindValue(":mission_type", missionType);
    query.bindValue(":mission_title", missionTitle);
    query.bindValue(":user_name", userName);
    query.bindValue(":vehicle", vehicle);
    query.bindValue(":prompt", prompt);
    query.bindValue(":asset_objective", assetObjective);
    
    if (!query.exec()) {
        qDebug() << "Error saving enhanced mission data:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Enhanced mission data saved successfully. ID:" << query.lastInsertId().toInt();
    return true;
}

bool DatabaseManager::saveChatGPTResponse(int missionId, const QString& response, const QString& functions)
{
    if (!initialized && !initialize()) {
        qDebug() << "Database not initialized";
        return false;
    }
    
    qDebug() << "Saving response for mission ID:" << missionId;
    qDebug() << "Response length:" << response.length();
    qDebug() << "Functions length:" << functions.length();
    
    QSqlQuery query;
    query.prepare("INSERT INTO responses (mission_id, response, functions) "
                 "VALUES (:mission_id, :response, :functions)");
    query.bindValue(":mission_id", missionId);
    query.bindValue(":response", response);
    query.bindValue(":functions", functions);
    
    if (!query.exec()) {
        qDebug() << "Error saving ChatGPT response:" << query.lastError().text();
        qDebug() << "Query:" << query.lastQuery();
        qDebug() << "Mission ID:" << missionId;
        qDebug() << "Response:" << response;
        qDebug() << "Functions:" << functions;
        return false;
    }
    
    qDebug() << "ChatGPT response saved successfully";
    return true;
}

QSqlQuery DatabaseManager::getMissionHistory()
{
    QSqlQuery query;
    query.prepare("SELECT id, mission_type, vehicle, prompt, timestamp FROM missions ORDER BY timestamp DESC");
    
    if (!query.exec()) {
        qDebug() << "Error retrieving mission history:" << query.lastError().text();
    }
    
    return query;
}

QSqlQuery DatabaseManager::getMissionDetails(int missionId)
{
    QSqlQuery query;
    query.prepare("SELECT m.id, m.mission_type, m.vehicle, m.prompt, m.timestamp, m.status, "
                 "r.response, r.functions, r.timestamp as response_timestamp "
                 "FROM missions m "
                 "LEFT JOIN responses r ON m.id = r.mission_id "
                 "WHERE m.id = :mission_id");
    query.bindValue(":mission_id", missionId);
    
    if (!query.exec()) {
        qDebug() << "Error retrieving mission details:" << query.lastError().text();
    }
    
    return query;
}

bool DatabaseManager::updateMissionStatus(int missionId, const QString& status)
{
    if (!initialized && !initialize()) {
        qDebug() << "Database not initialized";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("UPDATE missions SET status = :status WHERE id = :id");
    query.bindValue(":id", missionId);
    query.bindValue(":status", status);
    
    if (!query.exec()) {
        qDebug() << "Error updating mission status:" << query.lastError().text();
        return false;
    }
    
    return true;
}