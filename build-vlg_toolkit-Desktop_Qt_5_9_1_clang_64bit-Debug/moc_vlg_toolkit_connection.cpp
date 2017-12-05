/****************************************************************************
** Meta object code from reading C++ file 'vlg_toolkit_connection.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../toolkit/vlg_toolkit_connection.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vlg_toolkit_connection.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_vlg_toolkit_Connection_t {
    QByteArrayData data[17];
    char stringdata0[324];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_vlg_toolkit_Connection_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_vlg_toolkit_Connection_t qt_meta_stringdata_vlg_toolkit_Connection = {
    {
QT_MOC_LITERAL(0, 0, 22), // "vlg_toolkit_Connection"
QT_MOC_LITERAL(1, 23, 22), // "SignalConnStatusChange"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 21), // "vlg::ConnectionStatus"
QT_MOC_LITERAL(4, 69, 6), // "status"
QT_MOC_LITERAL(5, 76, 23), // "SignalConnectionTimeout"
QT_MOC_LITERAL(6, 100, 3), // "msg"
QT_MOC_LITERAL(7, 104, 26), // "SignalDisconnectionTimeout"
QT_MOC_LITERAL(8, 131, 18), // "OnConnStatusChange"
QT_MOC_LITERAL(9, 150, 10), // "OnTestSlot"
QT_MOC_LITERAL(10, 161, 21), // "OnCustomMenuRequested"
QT_MOC_LITERAL(11, 183, 3), // "pos"
QT_MOC_LITERAL(12, 187, 25), // "on_connect_button_clicked"
QT_MOC_LITERAL(13, 213, 28), // "on_disconnect_button_clicked"
QT_MOC_LITERAL(14, 242, 30), // "on_extend_model_button_clicked"
QT_MOC_LITERAL(15, 273, 24), // "on_new_tx_button_clicked"
QT_MOC_LITERAL(16, 298, 25) // "on_new_sbs_button_clicked"

    },
    "vlg_toolkit_Connection\0SignalConnStatusChange\0"
    "\0vlg::ConnectionStatus\0status\0"
    "SignalConnectionTimeout\0msg\0"
    "SignalDisconnectionTimeout\0"
    "OnConnStatusChange\0OnTestSlot\0"
    "OnCustomMenuRequested\0pos\0"
    "on_connect_button_clicked\0"
    "on_disconnect_button_clicked\0"
    "on_extend_model_button_clicked\0"
    "on_new_tx_button_clicked\0"
    "on_new_sbs_button_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_vlg_toolkit_Connection[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       5,    1,   72,    2, 0x06 /* Public */,
       7,    1,   75,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    1,   78,    2, 0x0a /* Public */,
       9,    0,   81,    2, 0x0a /* Public */,
      10,    1,   82,    2, 0x0a /* Public */,
      12,    0,   85,    2, 0x08 /* Private */,
      13,    0,   86,    2, 0x08 /* Private */,
      14,    0,   87,    2, 0x08 /* Private */,
      15,    0,   88,    2, 0x08 /* Private */,
      16,    0,   89,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void vlg_toolkit_Connection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        vlg_toolkit_Connection *_t = static_cast<vlg_toolkit_Connection *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SignalConnStatusChange((*reinterpret_cast< vlg::ConnectionStatus(*)>(_a[1]))); break;
        case 1: _t->SignalConnectionTimeout((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->SignalDisconnectionTimeout((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->OnConnStatusChange((*reinterpret_cast< vlg::ConnectionStatus(*)>(_a[1]))); break;
        case 4: _t->OnTestSlot(); break;
        case 5: _t->OnCustomMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 6: _t->on_connect_button_clicked(); break;
        case 7: _t->on_disconnect_button_clicked(); break;
        case 8: _t->on_extend_model_button_clicked(); break;
        case 9: _t->on_new_tx_button_clicked(); break;
        case 10: _t->on_new_sbs_button_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (vlg_toolkit_Connection::*_t)(vlg::ConnectionStatus );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&vlg_toolkit_Connection::SignalConnStatusChange)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (vlg_toolkit_Connection::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&vlg_toolkit_Connection::SignalConnectionTimeout)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (vlg_toolkit_Connection::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&vlg_toolkit_Connection::SignalDisconnectionTimeout)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject vlg_toolkit_Connection::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_vlg_toolkit_Connection.data,
      qt_meta_data_vlg_toolkit_Connection,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *vlg_toolkit_Connection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *vlg_toolkit_Connection::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_vlg_toolkit_Connection.stringdata0))
        return static_cast<void*>(const_cast< vlg_toolkit_Connection*>(this));
    return QWidget::qt_metacast(_clname);
}

int vlg_toolkit_Connection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void vlg_toolkit_Connection::SignalConnStatusChange(vlg::ConnectionStatus _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void vlg_toolkit_Connection::SignalConnectionTimeout(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void vlg_toolkit_Connection::SignalDisconnectionTimeout(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
