/****************************************************************************
** Meta object code from reading C++ file 'messagesocket.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../cloudserver_1009/messagesocket.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'messagesocket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MessageSocket[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      69,   65,   14,   14, 0x05,
     123,   14,   14,   14, 0x05,
     146,   14,   14,   14, 0x05,
     169,   14,   14,   14, 0x05,
     195,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     221,   14,   14,   14, 0x0a,
     247,   14,   14,   14, 0x0a,
     272,   14,   14,   14, 0x0a,
     303,   65,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MessageSocket[] = {
    "MessageSocket\0\0"
    "MessageSocketSignalToMessageServer_disconnected()\0"
    "msg\0MessageSocketSignalToMessageServer_sendtoall(QString)\0"
    "signal_addseg(QString)\0signal_delseg(QString)\0"
    "signal_addmarker(QString)\0"
    "signal_delmarker(QString)\0"
    "MessageSocketSlot_start()\0"
    "MessageSocketSlot_Read()\0"
    "MessageSocketSlot_disconnect()\0"
    "MessageSocketSlotAnswerToMessageServer_sendtoall(QString)\0"
};

void MessageSocket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MessageSocket *_t = static_cast<MessageSocket *>(_o);
        switch (_id) {
        case 0: _t->MessageSocketSignalToMessageServer_disconnected(); break;
        case 1: _t->MessageSocketSignalToMessageServer_sendtoall((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->signal_addseg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->signal_delseg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->signal_addmarker((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->signal_delmarker((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->MessageSocketSlot_start(); break;
        case 7: _t->MessageSocketSlot_Read(); break;
        case 8: _t->MessageSocketSlot_disconnect(); break;
        case 9: _t->MessageSocketSlotAnswerToMessageServer_sendtoall((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MessageSocket::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MessageSocket::staticMetaObject = {
    { &QTcpSocket::staticMetaObject, qt_meta_stringdata_MessageSocket,
      qt_meta_data_MessageSocket, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MessageSocket::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MessageSocket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MessageSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MessageSocket))
        return static_cast<void*>(const_cast< MessageSocket*>(this));
    return QTcpSocket::qt_metacast(_clname);
}

int MessageSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpSocket::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void MessageSocket::MessageSocketSignalToMessageServer_disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void MessageSocket::MessageSocketSignalToMessageServer_sendtoall(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MessageSocket::signal_addseg(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MessageSocket::signal_delseg(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MessageSocket::signal_addmarker(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MessageSocket::signal_delmarker(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
