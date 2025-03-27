#pragma once

#include <QMainWindow>
#include <QWebEngineView>

/**
 * @brief The MapViewer class provides a 3D map visualization using Mapbox GL JS
 */
class MapViewer : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructor for the MapViewer
     * @param parent The parent widget
     */
    explicit MapViewer(QWidget *parent = nullptr);

private:
    /**
     * @brief Sets up the UI components
     */
    void setupUI();
    
    /**
     * @brief Configures the WebEngine settings
     */
    void setupWebEngine();
    
    /**
     * @brief Initializes the Mapbox map
     */
    void initializeMap();
    
    /** The web view for displaying the map */
    QWebEngineView *m_webView;
}; 