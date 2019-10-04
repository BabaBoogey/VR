#ifndef MESSAGESERVER_H
#define MESSAGESERVER_H

#include <QtNetwork>
#include <QString>
#include <QStringList>
#include <QMap>
#include <vector>
#include <QRegExp>
#include <QReadWriteLock>
#include <QTime>
#include "basic_surf_objs.h"

class  MySocket;

struct clientproperty{
    int number;
    QString name;
    int colortype;
    bool Creator;
    bool online;
   int messageindex;
   int Creator_res;
};

struct Global_Parameters
{
    QMap <MySocket* ,QString> clients;
    QReadWriteLock lock_clients;

    int clientNum;
    QReadWriteLock lock_clientNum;

    std::vector<clientproperty> clientsproperty;
    QReadWriteLock lock_clientsproperty;

    std::vector<QString> messagelist;
    QReadWriteLock lock_messagelist;

    QString Creator;
    QReadWriteLock lock_Creator;

//    QString filename;

//    NeuronTree wholeNT;

//	QList<NeuronTree> NeuronList;

//    int sketchNum;



};

class MySocket:public QTcpSocket
{
    Q_OBJECT
public:
    explicit MySocket(int socketDesc, Global_Parameters *parameters=0, QObject *parent=0);

public slots:
    void MySocketSlot_Read();
    void MySocketSlot_disconnect();
    void MySocketSlotAnswerMyThread_sendtouser(const QString &msg);
signals:
    void MySocketSignalToMyThread_disconnected();
    void MySocketSignalToMyThread_sendtoall(const QString &msg);
protected:
    void SendToUser(const QString &msg);
    void SendToAll(const QString &msg);
    void SendUserList();
    void SendColortype();
    void SendCreatorMsg();

    void updateUserMessage(QString username);
    bool containsClient(const QString &name);
    int getUser(const QString &name);

private:
    void loginProcess(const QString &user);
    void messageProcess(const QString msg);
    void deleteProcess(const QString &delID);
    void markerProcess(const QString &markermsg);
    void delmaekerProcess(const QString &delmarkerpos);
    void dragnodeProcess(const QString &dragnodepos);
    void hmdposProcess(const QString &hmd);
    void askmessageProcess();
    void resindexProcess(const QString &msg);
//    void leaveProcess();

private:
    Global_Parameters *global_parameters;
    int socketId;
    int receivemessageindex;//for debug
    quint64 nextBlockSize;



};

class ThreadList;

class MyThread:public QThread
{
    Q_OBJECT
public:
    explicit MyThread(int socketDesc,ThreadList *list=0, Global_Parameters *parameters=0, QObject *parent=0);
    void sendtouser(const QString &msg);
public slots:
    void MyThreadSlotAnswerMySocket_disconnect();
    void MyThreadSlotAnswerMySocket_sendtoall(const QString &msg);

signals:
    void MyThreadSignalToMySocket_sendtouser(const QString &msg);
    void MyThreadSignalToMyServer_sendtoall(const QString &msg);

private:
    void run();

private:
    MySocket *socket;
    Global_Parameters *global_parameters;
    int socketId;
    ThreadList *threadlist;

};

class ThreadList
{
public:
    void add(MyThread* thread, MySocket* socket)
    {
        lock_threadlist.lockForWrite();
        threadlist[thread]=socket;
        lock_threadlist.unlock();
    }
    void remove(MyThread* thread)
    {
        lock_threadlist.lockForWrite();
        threadlist.remove(thread);
        lock_threadlist.unlock();
    }

    void sendtoall(const QString &msg)//used by server
    {
        lock_threadlist.lockForRead();
        foreach(MyThread *thread,threadlist.keys())
        {
            thread->sendtouser(msg);
        }
        lock_threadlist.unlock();
    }


private:
    QMap <MyThread *,MySocket*> threadlist;
    QReadWriteLock lock_threadlist;
};




class MyServer:public QTcpServer
{
    Q_OBJECT

public:
    explicit MyServer(ThreadList *list=0,Global_Parameters *parameters=0, QObject *PARENT=0);

public slots:
    void MyServerSlotAnswerMyThread_sendtoall(const QString &msg);
    void MyServerSlot_saveANO();

private:

    void incomingConnection(int socketDesc);

private:
    QTimer *time_save;
    ThreadList *threadlist;
    Global_Parameters *global_parameters;

};



#endif // MESSAGESERVER_H
