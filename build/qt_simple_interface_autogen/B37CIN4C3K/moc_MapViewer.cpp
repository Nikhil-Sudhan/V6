/****************************************************************************
** Meta object code from reading C++ file 'MapViewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../include/components/MapViewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MapViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DebugWebEnginePage_t {
    QByteArrayData data[1];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DebugWebEnginePage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DebugWebEnginePage_t qt_meta_stringdata_DebugWebEnginePage = {
    {
QT_MOC_LITERAL(0, 0, 18) // "DebugWebEnginePage"

    },
    "DebugWebEnginePage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DebugWebEnginePage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void DebugWebEnginePage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject DebugWebEnginePage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWebEnginePage::staticMetaObject>(),
    qt_meta_stringdata_DebugWebEnginePage.data,
    qt_meta_data_DebugWebEnginePage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DebugWebEnginePage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DebugWebEnginePage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DebugWebEnginePage.stringdata0))
        return static_cast<void*>(this);
    return QWebEnginePage::qt_metacast(_clname);
}

int DebugWebEnginePage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebEnginePage::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_MapViewer_t {
    QByteArrayData data[27];
    char stringdata0[410];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MapViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MapViewer_t qt_meta_stringdata_MapViewer = {
    {
QT_MOC_LITERAL(0, 0, 9), // "MapViewer"
QT_MOC_LITERAL(1, 10, 19), // "geometricShapeSaved"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 9), // "shapeName"
QT_MOC_LITERAL(4, 41, 23), // "droneAnimationCompleted"
QT_MOC_LITERAL(5, 65, 17), // "setDronePositions"
QT_MOC_LITERAL(6, 83, 18), // "QVector<QVector3D>"
QT_MOC_LITERAL(7, 102, 9), // "positions"
QT_MOC_LITERAL(8, 112, 15), // "updateDronePath"
QT_MOC_LITERAL(9, 128, 11), // "geojsonData"
QT_MOC_LITERAL(10, 140, 16), // "saveGeometryData"
QT_MOC_LITERAL(11, 157, 12), // "geometryData"
QT_MOC_LITERAL(12, 170, 18), // "updateGeometryData"
QT_MOC_LITERAL(13, 189, 19), // "checkForFileChanges"
QT_MOC_LITERAL(14, 209, 14), // "setActiveDrone"
QT_MOC_LITERAL(15, 224, 9), // "droneName"
QT_MOC_LITERAL(16, 234, 18), // "saveGeometricShape"
QT_MOC_LITERAL(17, 253, 9), // "shapeData"
QT_MOC_LITERAL(18, 263, 19), // "loadGeometricShapes"
QT_MOC_LITERAL(19, 283, 20), // "deleteGeometricShape"
QT_MOC_LITERAL(20, 304, 21), // "clearDronePathsOnExit"
QT_MOC_LITERAL(21, 326, 19), // "startDroneAnimation"
QT_MOC_LITERAL(22, 346, 16), // "confirmDroneTask"
QT_MOC_LITERAL(23, 363, 11), // "missionType"
QT_MOC_LITERAL(24, 375, 7), // "vehicle"
QT_MOC_LITERAL(25, 383, 6), // "prompt"
QT_MOC_LITERAL(26, 390, 19) // "updateDronePosition"

    },
    "MapViewer\0geometricShapeSaved\0\0shapeName\0"
    "droneAnimationCompleted\0setDronePositions\0"
    "QVector<QVector3D>\0positions\0"
    "updateDronePath\0geojsonData\0"
    "saveGeometryData\0geometryData\0"
    "updateGeometryData\0checkForFileChanges\0"
    "setActiveDrone\0droneName\0saveGeometricShape\0"
    "shapeData\0loadGeometricShapes\0"
    "deleteGeometricShape\0clearDronePathsOnExit\0"
    "startDroneAnimation\0confirmDroneTask\0"
    "missionType\0vehicle\0prompt\0"
    "updateDronePosition"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MapViewer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   89,    2, 0x06 /* Public */,
       4,    0,   92,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   93,    2, 0x0a /* Public */,
       8,    1,   96,    2, 0x0a /* Public */,
      10,    1,   99,    2, 0x0a /* Public */,
      12,    1,  102,    2, 0x0a /* Public */,
      13,    0,  105,    2, 0x0a /* Public */,
      14,    1,  106,    2, 0x0a /* Public */,
      16,    2,  109,    2, 0x0a /* Public */,
      18,    0,  114,    2, 0x0a /* Public */,
      19,    1,  115,    2, 0x0a /* Public */,
      20,    0,  118,    2, 0x0a /* Public */,
      21,    0,  119,    2, 0x0a /* Public */,
      22,    3,  120,    2, 0x0a /* Public */,
      26,    0,  127,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::QJsonObject,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   17,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,   23,   24,   25,
    QMetaType::Void,

       0        // eod
};

void MapViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MapViewer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->geometricShapeSaved((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->droneAnimationCompleted(); break;
        case 2: _t->setDronePositions((*reinterpret_cast< const QVector<QVector3D>(*)>(_a[1]))); break;
        case 3: _t->updateDronePath((*reinterpret_cast< const QJsonObject(*)>(_a[1]))); break;
        case 4: _t->saveGeometryData((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->updateGeometryData((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->checkForFileChanges(); break;
        case 7: _t->setActiveDrone((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->saveGeometricShape((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 9: _t->loadGeometricShapes(); break;
        case 10: _t->deleteGeometricShape((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->clearDronePathsOnExit(); break;
        case 12: _t->startDroneAnimation(); break;
        case 13: _t->confirmDroneTask((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 14: _t->updateDronePosition(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QVector3D> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MapViewer::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MapViewer::geometricShapeSaved)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MapViewer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MapViewer::droneAnimationCompleted)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MapViewer::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_MapViewer.data,
    qt_meta_data_MapViewer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MapViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MapViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MapViewer.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int MapViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void MapViewer::geometricShapeSaved(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MapViewer::droneAnimationCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
