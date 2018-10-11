/****************************************************************************
** Meta object code from reading C++ file 'GUISHLightingApp.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../GUISHLightingApp.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GUISHLightingApp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_GUISHLightingApp_t {
    QByteArrayData data[10];
    char stringdata0[198];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GUISHLightingApp_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GUISHLightingApp_t qt_meta_stringdata_GUISHLightingApp = {
    {
QT_MOC_LITERAL(0, 0, 16), // "GUISHLightingApp"
QT_MOC_LITERAL(1, 17, 25), // "Slot_LoadSphericalTexture"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 16), // "Slot_LoadCubeMap"
QT_MOC_LITERAL(4, 61, 29), // "Slot_SaveSHCoefficientsToFile"
QT_MOC_LITERAL(5, 91, 14), // "Slot_Menu_Exit"
QT_MOC_LITERAL(6, 106, 15), // "Slot_Menu_About"
QT_MOC_LITERAL(7, 122, 25), // "Slot_ComputeShCoefficient"
QT_MOC_LITERAL(8, 148, 21), // "Slot_CameraProj_Ortho"
QT_MOC_LITERAL(9, 170, 27) // "Slot_CameraProj_Perspective"

    },
    "GUISHLightingApp\0Slot_LoadSphericalTexture\0"
    "\0Slot_LoadCubeMap\0Slot_SaveSHCoefficientsToFile\0"
    "Slot_Menu_Exit\0Slot_Menu_About\0"
    "Slot_ComputeShCoefficient\0"
    "Slot_CameraProj_Ortho\0Slot_CameraProj_Perspective"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GUISHLightingApp[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x0a /* Public */,
       3,    0,   55,    2, 0x0a /* Public */,
       4,    0,   56,    2, 0x0a /* Public */,
       5,    0,   57,    2, 0x0a /* Public */,
       6,    0,   58,    2, 0x0a /* Public */,
       7,    0,   59,    2, 0x0a /* Public */,
       8,    0,   60,    2, 0x0a /* Public */,
       9,    0,   61,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void GUISHLightingApp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GUISHLightingApp *_t = static_cast<GUISHLightingApp *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Slot_LoadSphericalTexture(); break;
        case 1: _t->Slot_LoadCubeMap(); break;
        case 2: _t->Slot_SaveSHCoefficientsToFile(); break;
        case 3: _t->Slot_Menu_Exit(); break;
        case 4: _t->Slot_Menu_About(); break;
        case 5: _t->Slot_ComputeShCoefficient(); break;
        case 6: _t->Slot_CameraProj_Ortho(); break;
        case 7: _t->Slot_CameraProj_Perspective(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject GUISHLightingApp::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_GUISHLightingApp.data,
      qt_meta_data_GUISHLightingApp,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *GUISHLightingApp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GUISHLightingApp::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_GUISHLightingApp.stringdata0))
        return static_cast<void*>(const_cast< GUISHLightingApp*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int GUISHLightingApp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
