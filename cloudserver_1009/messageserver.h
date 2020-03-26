#ifndef MESSAGESERVER_H
#define MESSAGESERVER_H

#include <QtNetwork>
#include <ctime>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QThread>
#include <QReadWriteLock>
//#include "basic_surf_objs.h"
#include "messagesocket.h"
//#include "basic_c_fun/neuron_format_converter.h"
#include "neuron_editing/neuron_format_converter.h"
#include <QMap>
#include <QDateTime>
struct RemoveInfo{
    NeuronTree NT;
    int time;
    int id;
//    QDateTime dateTime;
};

class MessageServer:public QTcpServer
{
    Q_OBJECT
public:
    explicit MessageServer(QString filename,Global_Parameters *parameters,QObject *parent=0);
protected:
public:
        Global_Parameters *global_parameters;
private:
    void incomingConnection(int socketDesc);
    QString filename;

    QList<NeuronTree> sketchedNTList;
    QList<RemoveInfo> removedNTList;
    QStringList orderList;
    int sketchNum;

public slots:
    void MessageServerSlotAnswerMessageSocket_sendtoall(const QString &msg);
    void MessageServerSlotAnswerMessageSocket_disconnected();

    void MessageServerSlotAnswerMessageSocket_addseg(QString);
    void MessageServerSlotAnswerMessageSocket_delseg(QString);
    void MessageServerSlotAnswerMessageSocket_addmarker(QString);
    void MessageServerSlotAnswerMessageSocket_delmarker(QString);
    void MessageServerSlotAnswerMessageSocket_retype(QString);
//    void MessageServerSlotAnswerMessageSocket_insert(QString);
//    void MessageServerSlotAnswerMessageSocket_directconnect(QString);
    QMap<quint32 ,QString> autoSave();
signals:
    void MessageServerSignal_sendtoall(const QString &msg);
    void MessageServerDeleted(QString);

};
#endif // MESSAGESERVER_H
