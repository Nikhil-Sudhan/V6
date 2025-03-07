#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebEnginePage>

// Custom WebEnginePage for debugging
class DebugWebEnginePage : public QWebEnginePage {
public:
    DebugWebEnginePage(QWebEngineProfile *profile, QObject *parent = nullptr) 
        : QWebEnginePage(profile, parent) {}

protected:
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, 
                                 int lineNumber, const QString &sourceID) override;
};

class MapViewer : public QWidget {
    Q_OBJECT

public:
    explicit MapViewer(QWidget *parent = nullptr);
    ~MapViewer();

private:
    void initializeMap();
    QWebEngineView *m_webView;
};

#endif // MAPVIEWER_H 