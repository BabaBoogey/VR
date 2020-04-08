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
#include <managesocket.h>
#include "send_file.h"
struct RemoveInfo{
    NeuronTree NT;
    int time;
    int id;
};
const QVector3D neuron_type_color[ ] = {///////////////////////////////////////////////////////
        QVector3D(255, 255, 255),  // white,   0-undefined
        QVector3D(20,  20,  20 ),  // black,   1-soma
        QVector3D(200, 20,  0  ),  // red,     2-axon
        QVector3D(0,   20,  200),  // blue,    3-dendrite
        QVector3D(200, 0,   200),  // purple,  4-apical dendrite
        //the following is Hanchuan's extended color. 090331
        QVector3D(0,   200, 200),  // cyan,    5
        QVector3D(220, 200, 0  ),  // yellow,  6
        QVector3D(0,   200, 20 ),  // green,   7
        QVector3D(188, 94,  37 ),  // coffee,  8
        QVector3D(180, 200, 120),  // asparagus,	9
        QVector3D(250, 100, 120),  // salmon,	10
        QVector3D(120, 200, 200),  // ice,		11
        QVector3D(100, 120, 200),  // orchid,	12
    //the following is Hanchuan's further extended color. 111003
    QVector3D(255, 128, 168),  //	13
    QVector3D(128, 255, 168),  //	14
    QVector3D(128, 168, 255),  //	15
    QVector3D(168, 255, 128),  //	16
    QVector3D(255, 168, 128),  //	17
    QVector3D(168, 128, 255), //	18
        };
class MessageServer;
struct ForAUTOSave
{

    QString ip;
    FileServer_send *fileserver_send=0;
    ManageSocket *managesocket=0;
    QMap <QString ,MessageServer* > *Map_File_MessageServer=0;
    QString anofile_name;
    QString messageport;
};

class MessageServer:public QTcpServer
{
    Q_OBJECT
public:
    explicit MessageServer(QString filename,Global_Parameters *parameters=0,QObject *parent=0);
    inline void emitUserLoad(ForAUTOSave t)
    {
        emit userLoadSignal(t);
    }
signals:
    void userLoadSignal(ForAUTOSave);
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
    void MessageServerSlotAnswerMessageSocket_addmarker(QString,int flag=0);
    void MessageServerSlotAnswerMessageSocket_delmarker(QString,bool flag=0);
    void MessageServerSlotAnswerMessageSocket_retype(QString);
//    void MessageServerSlotAnswerMessageSocket_insert(QString);
//    void MessageServerSlotAnswerMessageSocket_directconnect(QString);
    QMap<quint32 ,QString> autoSave();
    void userLoad(ForAUTOSave);
signals:
    void MessageServerSignal_sendtoall(const QString &msg);
    void MessageServerDeleted(QString);

};
#endif // MESSAGESERVER_H
