#ifndef CHATGPTCLIENT_H
#define CHATGPTCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QDir>
#include <QFile>

class ChatGPTClient : public QObject
{
    Q_OBJECT
public:
    static ChatGPTClient& instance();
    void sendPrompt(const QString& missionType, const QString& vehicle, const QString& prompt);

signals:
    void responseReceived(int missionId, const QString& response, const QString& functions);
    void errorOccurred(const QString& errorMessage);

private slots:
    void handleNetworkReply(QNetworkReply* reply);

private:
    explicit ChatGPTClient(QObject* parent = nullptr);
    ~ChatGPTClient();
    
    // Prevent copying
    ChatGPTClient(const ChatGPTClient&) = delete;
    ChatGPTClient& operator=(const ChatGPTClient&) = delete;
    
    // Helper method to load geometric shapes data
    QJsonObject loadGeometricShapesData();
    
    QNetworkAccessManager* networkManager;
    QString apiKey;
    int currentMissionId;
};

#endif // CHATGPTCLIENT_H