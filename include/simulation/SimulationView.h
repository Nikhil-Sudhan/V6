#ifndef SIMULATIONVIEW_H
#define SIMULATIONVIEW_H

#include <QWidget>
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>

class SimulationView : public QWidget {
    Q_OBJECT
public:
    explicit SimulationView(QWidget* parent = nullptr);
    
    void loadSimulation();
    void setDronePositions(const QVector<QVector3D>& positions);
    
private:
    QWebEngineView* webView;
    QString simulationHtml;
    
    void setupUI();
    void createSimulationHtml();
    void startPathUpdateTimer();
    void updateDronePath(const QString& droneName);
};

#endif // SIMULATIONVIEW_H 