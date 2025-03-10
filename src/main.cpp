#include "../include/MainWindow.h"
#include <QApplication>
#include <QWebEngineSettings>
#include <QDir>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDebug>
#include <QSurfaceFormat>

QString findWebEngineProcess() {
    QStringList searchPaths = {
        "/usr/lib/x86_64-linux-gnu/qt5/libexec/QtWebEngineProcess",
        QCoreApplication::applicationDirPath() + "/QtWebEngineProcess"
    };

    for (const QString& path : searchPaths) {
        if (QFile::exists(path)) {
            qDebug() << "Found QtWebEngineProcess at:" << path;
            return path;
        }
    }
    
    QString systemPath = QStandardPaths::findExecutable("QtWebEngineProcess");
    if (!systemPath.isEmpty()) {
        qDebug() << "Found QtWebEngineProcess in system path:" << systemPath;
        return systemPath;
    }

    qDebug() << "Could not find QtWebEngineProcess in any of the expected locations";
    return QString();
}

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        break;
    }
    fprintf(stderr, "%s\n", txt.toLocal8Bit().constData());
    fflush(stderr);
}

int main(int argc, char *argv[]) {
    // Set OpenGL format
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    
    qputenv("QT_DEBUG_PLUGINS", "1");
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--enable-gpu-rasterization --enable-native-gpu-memory-buffers --ignore-gpu-blacklist");
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9222");
    
    QString webEngineProcess = findWebEngineProcess();
    if (!webEngineProcess.isEmpty()) {
        qputenv("QTWEBENGINEPROCESS_PATH", webEngineProcess.toLocal8Bit());
    } else {
        qDebug() << "WARNING: Could not find QtWebEngineProcess!";
    }
    
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    
    qInstallMessageHandler(messageHandler);
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    return app.exec();
} 