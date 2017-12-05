/****************************************************************************
** Meta object code from reading C++ file 'vlg_toolkit_core_util.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../toolkit/vlg_toolkit_core_util.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vlg_toolkit_core_util.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_vlg_tlkt__QPlainTextEditApnd_t {
    QByteArrayData data[6];
    char stringdata0[64];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_vlg_tlkt__QPlainTextEditApnd_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_vlg_tlkt__QPlainTextEditApnd_t qt_meta_stringdata_vlg_tlkt__QPlainTextEditApnd = {
    {
QT_MOC_LITERAL(0, 0, 28), // "vlg_tlkt::QPlainTextEditApnd"
QT_MOC_LITERAL(1, 29, 12), // "messageReady"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 13), // "vlg::TraceLVL"
QT_MOC_LITERAL(4, 57, 4), // "tlvl"
QT_MOC_LITERAL(5, 62, 1) // "s"

    },
    "vlg_tlkt::QPlainTextEditApnd\0messageReady\0"
    "\0vlg::TraceLVL\0tlvl\0s"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_vlg_tlkt__QPlainTextEditApnd[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QString,    4,    5,

       0        // eod
};

void vlg_tlkt::QPlainTextEditApnd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QPlainTextEditApnd *_t = static_cast<QPlainTextEditApnd *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->messageReady((*reinterpret_cast< vlg::TraceLVL(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QPlainTextEditApnd::*_t)(vlg::TraceLVL , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QPlainTextEditApnd::messageReady)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject vlg_tlkt::QPlainTextEditApnd::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_vlg_tlkt__QPlainTextEditApnd.data,
      qt_meta_data_vlg_tlkt__QPlainTextEditApnd,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *vlg_tlkt::QPlainTextEditApnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *vlg_tlkt::QPlainTextEditApnd::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_vlg_tlkt__QPlainTextEditApnd.stringdata0))
        return static_cast<void*>(const_cast< QPlainTextEditApnd*>(this));
    if (!strcmp(_clname, "vlg::appender"))
        return static_cast< vlg::appender*>(const_cast< QPlainTextEditApnd*>(this));
    return QObject::qt_metacast(_clname);
}

int vlg_tlkt::QPlainTextEditApnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void vlg_tlkt::QPlainTextEditApnd::messageReady(vlg::TraceLVL _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
