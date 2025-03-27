#include "mapviewer.h"
#include "map.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEnginePage>
#include <QDebug>

// ==========================================
// Constructor and Initialization
// ==========================================

MapViewer::MapViewer(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Mapbox 3D Viewer");
    resize(1024, 768);

    setupUI();
    setupWebEngine();
    initializeMap();
}

// ==========================================
// UI Setup
// ==========================================

void MapViewer::setupUI() {
    auto centralWidget = new QWidget(this);
    auto layout = new QVBoxLayout(centralWidget);
    
    m_webView = new QWebEngineView(this);
    layout->addWidget(m_webView);
    setCentralWidget(centralWidget);
}

// ==========================================
// Web Engine Configuration
// ==========================================

void MapViewer::setupWebEngine() {
    // Create and configure profile
    auto profile = new QWebEngineProfile("MapboxProfile", this);
    profile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    profile->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
    
    // Create web view and page
    auto page = new QWebEnginePage(profile, m_webView);
    m_webView->setPage(page);
    
    // Configure settings
    auto settings = page->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::WebGLEnabled, true);
    settings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
    
    // Connect error handling
    connect(page, &QWebEnginePage::loadFinished, this, [this](bool ok) {
        if (!ok) {
            qDebug() << "Failed to load the page";
        }
    });

// ==========================================
// Map Initialization
// ==========================================

void MapViewer::initializeMap() {
    const QString mapboxToken = "pk.eyJ1Ijoibmlja3lqMTIxIiwiYSI6ImNtN3N3eHFtcTB1MTkya3M4Mnc0dmQxanAifQ.gLJZYJe_zH9b9yxFxQZm6g";
    
    // Get the HTML content from the MapContent namespace
    QString html = MapContent::getMapHtml(mapboxToken);

    // Load the HTML content into the web view
    m_webView->setHtml(html, QUrl("https://api.mapbox.com"));
}