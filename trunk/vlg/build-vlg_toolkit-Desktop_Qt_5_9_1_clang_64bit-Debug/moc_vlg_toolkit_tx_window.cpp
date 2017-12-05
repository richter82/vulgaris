/****************************************************************************
** Meta object code from reading C++ file 'vlg_toolkit_tx_window.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../toolkit/vlg_toolkit_tx_window.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vlg_toolkit_tx_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_vlg_toolkit_tx_window_t {
    QByteArrayData data[10];
    char stringdata0[174];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_vlg_toolkit_tx_window_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_vlg_toolkit_tx_window_t qt_meta_stringdata_vlg_toolkit_tx_window = {
    {
QT_MOC_LITERAL(0, 0, 21), // "vlg_toolkit_tx_window"
QT_MOC_LITERAL(1, 22, 20), // "SignalTxStatusChange"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 22), // "vlg::TransactionStatus"
QT_MOC_LITERAL(4, 67, 6), // "status"
QT_MOC_LITERAL(5, 74, 15), // "SignalTxClosure"
QT_MOC_LITERAL(6, 90, 16), // "OnTxStatusChange"
QT_MOC_LITERAL(7, 107, 11), // "OnTxClosure"
QT_MOC_LITERAL(8, 119, 26), // "on_actionSend_TX_triggered"
QT_MOC_LITERAL(9, 146, 27) // "on_actionReNew_TX_triggered"

    },
    "vlg_toolkit_tx_window\0SignalTxStatusChange\0"
    "\0vlg::TransactionStatus\0status\0"
    "SignalTxClosure\0OnTxStatusChange\0"
    "OnTxClosure\0on_actionSend_TX_triggered\0"
    "on_actionReNew_TX_triggered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_vlg_toolkit_tx_window[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       5,    0,   47,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   48,    2, 0x0a /* Public */,
       7,    0,   51,    2, 0x0a /* Public */,
       8,    0,   52,    2, 0x08 /* Private */,
       9,    0,   53,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void vlg_toolkit_tx_window::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        vlg_toolkit_tx_window *_t = static_cast<vlg_toolkit_tx_window *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SignalTxStatusChange((*reinterpret_cast< vlg::TransactionStatus(*)>(_a[1]))); break;
        case 1: _t->SignalTxClosure(); break;
        case 2: _t->OnTxStatusChange((*reinterpret_cast< vlg::TransactionStatus(*)>(_a[1]))); break;
        case 3: _t->OnTxClosure(); break;
        case 4: _t->on_actionSend_TX_triggered(); break;
        case 5: _t->on_actionReNew_TX_triggered(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (vlg_toolkit_tx_window::*_t)(vlg::TransactionStatus );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&vlg_toolkit_tx_window::SignalTxStatusChange)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (vlg_toolkit_tx_window::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&vlg_toolkit_tx_window::SignalTxClosure)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject vlg_toolkit_tx_window::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_vlg_toolkit_tx_window.data,
      qt_meta_data_vlg_toolkit_tx_window,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *vlg_toolkit_tx_window::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *vlg_toolkit_tx_window::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_vlg_toolkit_tx_window.stringdata0))
        return static_cast<void*>(const_cast< vlg_toolkit_tx_window*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int vlg_toolkit_tx_window::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void vlg_toolkit_tx_window::SignalTxStatusChange(vlg::TransactionStatus _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void vlg_toolkit_tx_window::SignalTxClosure()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
