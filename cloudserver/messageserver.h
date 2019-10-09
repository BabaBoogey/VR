#ifndef MESSAGESERVER_H
#define MESSAGESERVER_H

#include <QtNetwork>

#include <QString>
#include <QStringList>
#include <QMap>
#include <QThread>
#include <QReadWriteLock>
#include "basic_surf_objs.h"
#include "messagesocket.h"







class MessageServer:public QTcpServer
{
    Q_OBJECT
public:
    explicit MessageServer(QString filename,Global_Parameters *parameters,QObject *parent=0);
protected:

private:
    void incomingConnection(int socketDesc);

    Global_Parameters *global_parameters;
    QString filename;

    QTimer *timer;


public slots:
    void MessageServerSlotAnswerMessageSocket_sendtoall(const QString &msg);
    void MessageServerSlotAnswerMessageSocket_disconnected();
    void autoSave();
signals:
    void MessageServerSignal_sendtoall(const QString &msg);
};






#endif // MESSAGESERVER_H
