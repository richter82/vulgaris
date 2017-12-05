/****************************************************************************
** Meta object code from reading C++ file 'vlg_toolkit_sbs_vlg_class_model.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../toolkit/vlg_toolkit_sbs_vlg_class_model.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vlg_toolkit_sbs_vlg_class_model.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_sbs_col_data_timer_t {
    QByteArrayData data[6];
    char stringdata0[80];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_sbs_col_data_timer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_sbs_col_data_timer_t qt_meta_stringdata_sbs_col_data_timer = {
    {
QT_MOC_LITERAL(0, 0, 18), // "sbs_col_data_timer"
QT_MOC_LITERAL(1, 19, 20), // "SignalCellResetColor"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 23), // "VLG_SBS_COL_DATA_ENTRY&"
QT_MOC_LITERAL(4, 65, 4), // "scde"
QT_MOC_LITERAL(5, 70, 9) // "OnTimeout"

    },
    "sbs_col_data_timer\0SignalCellResetColor\0"
    "\0VLG_SBS_COL_DATA_ENTRY&\0scde\0OnTimeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_sbs_col_data_timer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   27,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void sbs_col_data_timer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        sbs_col_data_timer *_t = static_cast<sbs_col_data_timer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SignalCellResetColor((*reinterpret_cast< VLG_SBS_COL_DATA_ENTRY(*)>(_a[1]))); break;
        case 1: _t->OnTimeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (sbs_col_data_timer::*_t)(VLG_SBS_COL_DATA_ENTRY & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&sbs_col_data_timer::SignalCellResetColor)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject sbs_col_data_timer::staticMetaObject = {
    { &QTimer::staticMetaObject, qt_meta_stringdata_sbs_col_data_timer.data,
      qt_meta_data_sbs_col_data_timer,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *sbs_col_data_timer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *sbs_col_data_timer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_sbs_col_data_timer.stringdata0))
        return static_cast<void*>(const_cast< sbs_col_data_timer*>(this));
    return QTimer::qt_metacast(_clname);
}

int sbs_col_data_timer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTimer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void sbs_col_data_timer::SignalCellResetColor(VLG_SBS_COL_DATA_ENTRY & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_vlg_toolkit_sbs_vlg_class_model_t {
    QByteArrayData data[5];
    char stringdata0[79];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_vlg_toolkit_sbs_vlg_class_model_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_vlg_toolkit_sbs_vlg_class_model_t qt_meta_stringdata_vlg_toolkit_sbs_vlg_class_model = {
    {
QT_MOC_LITERAL(0, 0, 31), // "vlg_toolkit_sbs_vlg_class_model"
QT_MOC_LITERAL(1, 32, 16), // "OnCellResetColor"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 23), // "VLG_SBS_COL_DATA_ENTRY&"
QT_MOC_LITERAL(4, 74, 4) // "scde"

    },
    "vlg_toolkit_sbs_vlg_class_model\0"
    "OnCellResetColor\0\0VLG_SBS_COL_DATA_ENTRY&\0"
    "scde"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_vlg_toolkit_sbs_vlg_class_model[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void vlg_toolkit_sbs_vlg_class_model::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        vlg_toolkit_sbs_vlg_class_model *_t = static_cast<vlg_toolkit_sbs_vlg_class_model *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnCellResetColor((*reinterpret_cast< VLG_SBS_COL_DATA_ENTRY(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject vlg_toolkit_sbs_vlg_class_model::staticMetaObject = {
    { &QAbstractTableModel::staticMetaObject, qt_meta_stringdata_vlg_toolkit_sbs_vlg_class_model.data,
      qt_meta_data_vlg_toolkit_sbs_vlg_class_model,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *vlg_toolkit_sbs_vlg_class_model::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *vlg_toolkit_sbs_vlg_class_model::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_vlg_toolkit_sbs_vlg_class_model.stringdata0))
        return static_cast<void*>(const_cast< vlg_toolkit_sbs_vlg_class_model*>(this));
    return QAbstractTableModel::qt_metacast(_clname);
}

int vlg_toolkit_sbs_vlg_class_model::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
