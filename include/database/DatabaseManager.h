#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>
#include <QVariant>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    static DatabaseManager& instance();
    
    bool initialize();
    bool isInitialized() const { return initialized; }
    
    // Mission data operations
    bool saveMissionData(const QString& missionType, const QString& vehicle, const QString& prompt);
    bool saveEnhancedMissionData(const QString& missionType, const QString& missionTitle, 
                               const QString& userName, const QString& vehicle, 
                               const QString& prompt, const QString& assetObjective);
    bool saveChatGPTResponse(int missionId, const QString& response, const QString& functions);
    
    // Query operations
    QSqlQuery getMissionHistory();
    QSqlQuery getMissionDetails(int missionId);
    bool updateMissionStatus(int missionId, const QString& status);

private:
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager();
    
    // Prevent copying
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    bool createTables();
    
    QSqlDatabase db;
    bool initialized;
};

#endif // DATABASEMANAGER_H 