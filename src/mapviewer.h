#pragma once

#include <QMainWindow>
#include <QWebEngineView>

class MapViewer : public QMainWindow {
    Q_OBJECT

public:
    explicit MapViewer(QWidget *parent = nullptr);

private:
    void initializeMap();
    QWebEngineView *m_webView;
}; 