/****************************************************************************
** Meta object code from reading C++ file 'vlg_toolkit_sbs_window.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../toolkit/vlg_toolkit_sbs_window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vlg_toolkit_sbs_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_vlg_toolkit_sbs_window_t {
    QByteArrayData data[15];
    char stringdata0[254];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_vlg_toolkit_sbs_window_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_vlg_toolkit_sbs_window_t qt_meta_stringdata_vlg_toolkit_sbs_window = {
    {
QT_MOC_LITERAL(0, 0, 22), // "vlg_toolkit_sbs_window"
QT_MOC_LITERAL(1, 23, 21), // "SignalSbsStatusChange"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 23), // "vlg::SubscriptionStatus"
QT_MOC_LITERAL(4, 70, 6), // "status"
QT_MOC_LITERAL(5, 77, 14), // "SignalSbsEvent"
QT_MOC_LITERAL(6, 92, 24), // "vlg::subscription_event*"
QT_MOC_LITERAL(7, 117, 7), // "sbs_evt"
QT_MOC_LITERAL(8, 125, 28), // "on_actionStart_SBS_triggered"
QT_MOC_LITERAL(9, 154, 27), // "on_actionStop_SBS_triggered"
QT_MOC_LITERAL(10, 182, 17), // "OnSbsStatusChange"
QT_MOC_LITERAL(11, 200, 10), // "OnSbsEvent"
QT_MOC_LITERAL(12, 211, 21), // "OnCustomMenuRequested"
QT_MOC_LITERAL(13, 233, 3), // "pos"
QT_MOC_LITERAL(14, 237, 16) // "OnNewTxRequested"

    },
    "vlg_toolkit_sbs_window\0SignalSbsStatusChange\0"
    "\0vlg::SubscriptionStatus\0status\0"
    "SignalSbsEvent\0vlg::subscription_event*\0"
    "sbs_evt\0on_actionStart_SBS_triggered\0"
    "on_actionStop_SBS_triggered\0"
    "OnSbsStatusChange\0OnSbsEvent\0"
    "OnCustomMenuRequested\0pos\0OnNewTxRequested"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_vlg_toolkit_sbs_window[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       5,    1,   57,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   60,    2, 0x08 /* Private */,
       9,    0,   61,    2, 0x08 /* Private */,
      10,    1,   62,    2, 0x0a /* Public */,
      11,    1,   65,    2, 0x0a /* Public */,
      12,    1,   68,    2, 0x0a /* Public */,
      14,    0,   71,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, QMetaType::QPoint,   13,
    QMetaType::Void,

       0        // eod
};

void vlg_toolkit_sbs_window::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        vlg_toolkit_sbs_window *_t = static_cast<vlg_toolkit_sbs_window *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SignalSbsStatusChange((*reinterpret_cast< vlg::SubscriptionStatus(*)>(_a[1]))); break;
        case 1: _t->SignalSbsEvent((*reinterpret_cast< vlg::subscription_event*(*)>(_a[1]))); break;
        case 2: _t->on_actionStart_SBS_triggered(); break;
        case 3: _t->on_actionStop_SBS_triggered(); break;
        case 4: _t->OnSbsStatusChange((*reinterpret_cast< vlg::SubscriptionStatus(*)>(_a[1]))); break;
        case 5: _t->OnSbsEvent((*reinterpret_cast< vlg::subscription_event*(*)>(_a[1]))); break;
        case 6: _t->OnCustomMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 7: _t->OnNewTxRequested(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (vlg_toolkit_sbs_window::*_t)(vlg::SubscriptionStatus );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&vlg_toolkit_sbs_window::SignalSbsStatusChange)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (vlg_toolkit_sbs_window::*_t)(vlg::subscription_event * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&vlg_toolkit_sbs_window::SignalSbsEvent)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject vlg_toolkit_sbs_window::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_vlg_toolkit_sbs_window.data,
      qt_meta_data_vlg_toolkit_sbs_window,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *vlg_toolkit_sbs_window::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *vlg_toolkit_sbs_window::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_vlg_toolkit_sbs_window.stringdata0))
        return static_cast<void*>(const_cast< vlg_toolkit_sbs_window*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int vlg_toolkit_sbs_window::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void vlg_toolkit_sbs_window::SignalSbsStatusChange(vlg::SubscriptionStatus _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void vlg_toolkit_sbs_window::SignalSbsEvent(vlg::subscription_event * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
