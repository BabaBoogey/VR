/****************************************************************************
** Meta object code from reading C++ file 'messageserver.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../cloudserver_1009/messageserver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'messageserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MessageServer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   15,   14,   14, 0x05,
      58,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      88,   15,   14,   14, 0x0a,
     144,   14,   14,   14, 0x0a,
     196,   14,   14,   14, 0x0a,
     249,   14,   14,   14, 0x0a,
     302,   14,   14,   14, 0x0a,
     358,   14,   14,   14, 0x0a,
     414,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MessageServer[] = {
    "MessageServer\0\0msg\0"
    "MessageServerSignal_sendtoall(QString)\0"
    "MessageServerDeleted(QString)\0"
    "MessageServerSlotAnswerMessageSocket_sendtoall(QString)\0"
    "MessageServerSlotAnswerMessageSocket_disconnected()\0"
    "MessageServerSlotAnswerMessageSocket_addseg(QString)\0"
    "MessageServerSlotAnswerMessageSocket_delseg(QString)\0"
    "MessageServerSlotAnswerMessageSocket_addmarker(QString)\0"
    "MessageServerSlotAnswerMessageSocket_delmarker(QString)\0"
    "autoSave()\0"
};

void MessageServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MessageServer *_t = static_cast<MessageServer *>(_o);
        switch (_id) {
        case 0: _t->MessageServerSignal_sendtoall((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->MessageServerDeleted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->MessageServerSlotAnswerMessageSocket_sendtoall((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->MessageServerSlotAnswerMessageSocket_disconnected(); break;
        case 4: _t->MessageServerSlotAnswerMessageSocket_addseg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->MessageServerSlotAnswerMessageSocket_delseg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->MessageServerSlotAnswerMessageSocket_addmarker((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->MessageServerSlotAnswerMessageSocket_delmarker((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->autoSave(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MessageServer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MessageServer::staticMetaObject = {
    { &QTcpServer::staticMetaObject, qt_meta_stringdata_MessageServer,
      qt_meta_data_MessageServer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MessageServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MessageServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MessageServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MessageServer))
        return static_cast<void*>(const_cast< MessageServer*>(this));
    return QTcpServer::qt_metacast(_clname);
}

int MessageServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpServer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void MessageServer::MessageServerSignal_sendtoall(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MessageServer::MessageServerDeleted(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
