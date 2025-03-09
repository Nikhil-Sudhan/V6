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
    QByteArrayData data[6];
    char stringdata0[69];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MapViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MapViewer_t qt_meta_stringdata_MapViewer = {
    {
QT_MOC_LITERAL(0, 0, 9), // "MapViewer"
QT_MOC_LITERAL(1, 10, 10), // "toggleView"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 17), // "setDronePositions"
QT_MOC_LITERAL(4, 40, 18), // "QVector<QVector3D>"
QT_MOC_LITERAL(5, 59, 9) // "positions"

    },
    "MapViewer\0toggleView\0\0setDronePositions\0"
    "QVector<QVector3D>\0positions"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MapViewer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x0a /* Public */,
       3,    1,   25,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,

       0        // eod
};

void MapViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MapViewer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->toggleView(); break;
        case 1: _t->setDronePositions((*reinterpret_cast< const QVector<QVector3D>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QVector3D> >(); break;
            }
            break;
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
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
