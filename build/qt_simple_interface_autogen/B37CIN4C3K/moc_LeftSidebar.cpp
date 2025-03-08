/****************************************************************************
** Meta object code from reading C++ file 'LeftSidebar.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../include/components/LeftSidebar.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LeftSidebar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LeftSidebar_t {
    QByteArrayData data[18];
    char stringdata0[204];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LeftSidebar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LeftSidebar_t qt_meta_stringdata_LeftSidebar = {
    {
QT_MOC_LITERAL(0, 0, 11), // "LeftSidebar"
QT_MOC_LITERAL(1, 12, 12), // "panelChanged"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 5), // "index"
QT_MOC_LITERAL(4, 32, 15), // "missionAssigned"
QT_MOC_LITERAL(5, 48, 11), // "missionType"
QT_MOC_LITERAL(6, 60, 7), // "vehicle"
QT_MOC_LITERAL(7, 68, 6), // "prompt"
QT_MOC_LITERAL(8, 75, 17), // "handleButtonClick"
QT_MOC_LITERAL(9, 93, 8), // "setTitle"
QT_MOC_LITERAL(10, 102, 5), // "title"
QT_MOC_LITERAL(11, 108, 16), // "handleAssignTask"
QT_MOC_LITERAL(12, 125, 21), // "handleChatGPTResponse"
QT_MOC_LITERAL(13, 147, 9), // "missionId"
QT_MOC_LITERAL(14, 157, 8), // "response"
QT_MOC_LITERAL(15, 166, 9), // "functions"
QT_MOC_LITERAL(16, 176, 14), // "handleApiError"
QT_MOC_LITERAL(17, 191, 12) // "errorMessage"

    },
    "LeftSidebar\0panelChanged\0\0index\0"
    "missionAssigned\0missionType\0vehicle\0"
    "prompt\0handleButtonClick\0setTitle\0"
    "title\0handleAssignTask\0handleChatGPTResponse\0"
    "missionId\0response\0functions\0"
    "handleApiError\0errorMessage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LeftSidebar[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       4,    3,   52,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   59,    2, 0x0a /* Public */,
       9,    1,   62,    2, 0x0a /* Public */,
      11,    0,   65,    2, 0x0a /* Public */,
      12,    3,   66,    2, 0x0a /* Public */,
      16,    1,   73,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    5,    6,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString,   13,   14,   15,
    QMetaType::Void, QMetaType::QString,   17,

       0        // eod
};

void LeftSidebar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LeftSidebar *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->panelChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->missionAssigned((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 2: _t->handleButtonClick((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setTitle((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->handleAssignTask(); break;
        case 5: _t->handleChatGPTResponse((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 6: _t->handleApiError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LeftSidebar::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LeftSidebar::panelChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LeftSidebar::*)(const QString & , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LeftSidebar::missionAssigned)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject LeftSidebar::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_LeftSidebar.data,
    qt_meta_data_LeftSidebar,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LeftSidebar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LeftSidebar::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LeftSidebar.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LeftSidebar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void LeftSidebar::panelChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LeftSidebar::missionAssigned(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
