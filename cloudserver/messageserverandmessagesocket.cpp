#include "messageserverandmessagesocket.h"
#include <QDataStream>
#include <QByteArray>
MessageSocket::MessageSocket(int socketDesc,Global_Parameters *parameters,QObject *parent)
    :socketId(socketDesc),global_parameters(parameters),QTcpSocket (parent)
{
    nextblocksize=0;
    qDebug()<<"make a messagesocket, and don't set it's socketId ";

}

void MessageSocket::MessageSocketSlot_Read()
{
    QRegExp loginRex("^/login:(.*)$");
    QRegExp messageRex("^/say:(.*)$");
    QRegExp hmdposRex("^/hmdpos:(.*)$");
    QRegExp deleteRex("^/del_curve:(.*)$");
    QRegExp markerRex("^/marker:(.*)$");
    QRegExp delmarkerRex("^/del_marker:(.*)$");
    QRegExp dragnodeRex("^/drag_node:(.*)$");
    QRegExp askmessageRex("^/ask:(.*)$");
    QRegExp ResIndexRex("^/ResIndex:(.*)$");

    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_7);

    if (nextblocksize == 0) {
        if (bytesAvailable() < sizeof(quint16))
            return;
        in >> nextblocksize;
    }

    if (bytesAvailable() < nextblocksize)
        return;

    QString msg;
    in>>msg;
    nextblocksize=0;

    if(loginRex.indexIn(msg)!=-1)
    {
        QString user=loginRex.cap(1);
//            qDebug()<<"in login";
        loginProcess(user);
    }else if (messageRex.indexIn(msg)!=-1)
    {

        QString msg=messageRex.cap(1);
        qDebug()<<"in message";
        messageProcess(msg);
    }else if(deleteRex.indexIn(msg)!=-1)
    {
        QString delID = deleteRex.cap(1);
//            qDebug()<<"in delete";
        deleteProcess(delID);
    }else if(markerRex.indexIn(msg)!=-1)
    {
        QString markermsg=markerRex.cap(1);
//            qDebug()<<"in marker";
        markerProcess(markermsg);
    }else if(delmarkerRex.indexIn(msg)!=-1)
    {
        QString delmarkerpos=delmarkerRex.cap(1);
//            qDebug()<<"in delmarker";
        delmaekerProcess(delmarkerpos);
    }else if(dragnodeRex.indexIn(msg)!=-1)
    {
        QString dragnodepos = dragnodeRex.cap(1);
//            qDebug()<<"in dragnode";
        dragnodeProcess(dragnodepos);
    }else if(hmdposRex.indexIn(msg)!=-1)
    {
         QString hmd = hmdposRex.cap(1);
//             qDebug()<<"in hmdpos";
         hmdposProcess(hmd);
    }else if(askmessageRex.indexIn(msg)!=-1)
    {
//            qDebug()<<"in askmessage";
        askmessageProcess();
    }else if(ResIndexRex.indexIn(msg)!=-1)
    {
         QString msg = ResIndexRex.cap(1);
//             qDebug()<<"in resindex";
         resindexProcess(msg);
    }else
    {
         qDebug() << "Bad message  ";
    }


}

void MessageSocket::loginProcess(const QString &name)
{
    global_parameters->lock_clients.lockForWrite();
    global_parameters->clients[this]=name;
    global_parameters->lock_clients.unlock();

    clientproperty client00={global_parameters->clientNum,name,21,false,true, 0};


    if(!containsClient(name))
    {
        global_parameters->lock_clientNum.lockForWrite();
        global_parameters->clientNum++;
        client00.colortype=global_parameters->clientNum+2;
        global_parameters->lock_clientNum.unlock();

        global_parameters->lock_clientsproperty.lockForWrite();
        global_parameters->clientsproperty.push_back(client00);
        global_parameters->lock_clientsproperty.unlock();

    }else {
        int i=getUser(name);
        global_parameters->lock_clientsproperty.lockForWrite();
        global_parameters->clientsproperty[i].online=true;
        global_parameters->lock_clientsproperty.unlock();
    }

    int onlineusernum=0;
    global_parameters->lock_clientsproperty.lockForRead();
    for(int  i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty[i].online)
            onlineusernum++;
    }
    global_parameters->lock_clientsproperty.unlock();

    int i=getUser(name);
    global_parameters->lock_clientsproperty.lockForWrite();
    if(global_parameters->clientsproperty.size()==1)
    {
        global_parameters->clientsproperty[i].Creator=true;

        global_parameters->lock_Creator.lockForWrite();
        global_parameters->Creator=name;
        global_parameters->lock_Creator.unlock();
    }
    global_parameters->lock_clientsproperty.unlock();

    SendToAll(QString("/system:" + name + " joined."));
    SendUserList();
    SendColortype();
    SendCreaorMsg();
}

void MessageSocket::messageProcess(const QString &msg)
{
    global_parameters->lock_clients.lockForRead();
    QString user=global_parameters->clients.value(this);
    global_parameters->lock_clients.unlock();

    global_parameters->lock_messagelist.lockForWrite();
    global_parameters->messagelist.push_back(QString(user + ":" + msg));
    global_parameters->lock_messagelist.unlock();

    //修改NeuronTreeList 参数QString(user + ":" + msg)
}

void MessageSocket::deleteProcess(const QString &delID)
{
    global_parameters->lock_clients.lockForRead();
    QString user=global_parameters->clients.value(this);
    global_parameters->lock_clients.unlock();

    global_parameters->lock_messagelist.lockForWrite();
    global_parameters->messagelist.push_back(QString("/del_curve:" +user+" "+delID ));
    global_parameters->lock_messagelist.unlock();

    //修改NeuronTreeList 参数QString("/del_curve:" +user+" "+delID )
}

void MessageSocket::markerProcess(const QString &markermsg)
{
    global_parameters->lock_clients.lockForRead();
    QString user=global_parameters->clients.value(this);
    global_parameters->lock_clients.unlock();

    global_parameters->lock_messagelist.lockForWrite();
    global_parameters->messagelist.push_back(QString("/marker:" +user+" "+markermsg));
    global_parameters->lock_messagelist.unlock();

    //加Marker ,QString("/marker:" +user+" "+markermsg)
}

void MessageSocket::delmaekerProcess(const QString &delmarkerpos)
{
    global_parameters->lock_clients.lockForRead();
    QString user=global_parameters->clients.value(this);
    global_parameters->lock_clients.unlock();

    global_parameters->lock_messagelist.lockForWrite();
    global_parameters->messagelist.push_back(QString("/del_marker:" +user+" "+delmarkerpos ));
    global_parameters->lock_messagelist.unlock();

    //减marker ，QString("/del_marker:" +user+" "+delmarkerpos )
}

void MessageSocket::dragnodeProcess(const QString &dragnodepos)
{
    global_parameters->lock_clients.lockForRead();
    QString user=global_parameters->clients.value(this);
    global_parameters->lock_clients.unlock();

    global_parameters->lock_messagelist.lockForWrite();
    global_parameters->messagelist.push_back(QString("/drag_node:" +user+" "+dragnodepos));
    global_parameters->lock_messagelist.unlock();

    //移动MARKER ,QString("/drag_node:" +user+" "+dragnodepos)
}

void MessageSocket::hmdposProcess(const QString &hmd)
{
    global_parameters->lock_clients.lockForRead();
    QString user=global_parameters->clients.value(this);
    global_parameters->lock_clients.unlock();

    SendToAll(QString("/hmdpos:" +user+" "+hmd ));
}

void MessageSocket::askmessageProcess()
{
    global_parameters->lock_clients.lockForRead();
    QString user=global_parameters->clients.value(this);
    global_parameters->lock_clients.unlock();
    updateUserMessage(user);
}

void MessageSocket::resindexProcess(const QString &msg)
{
    global_parameters->lock_clients.lockForRead();
    QString user=global_parameters->clients.value(this);
    global_parameters->lock_clients.unlock();

    global_parameters->lock_clientsproperty.lockForWrite();
    for(int i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty.at(i).name==user)
        {
            if(global_parameters->clientsproperty.at(i).Creator)
            {
                qDebug()<<"msg"<<msg;
                qDebug()<<"msg.toInt()"<<msg.toInt();
                global_parameters->clientsproperty.at(i).Creator_res = msg.toInt();
                qDebug()<<"update creator_res = "<<global_parameters->clientsproperty.at(i).Creator_res;
            }
        }
    }

    global_parameters->lock_clientsproperty.unlock();
}

void MessageSocket::SendToUser(const QString &msg)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);

    out<<quint64(0)<<msg;
    out.device()->seek(0);
    out<<quint64(sizeof (block)-sizeof (quint64))<<msg;

    this->write(block);
}

void MessageSocket::SendToAll(const QString &msg)
{
    emit MessageSocketSignalToMessageServer_sendtoall(msg);
}

void MessageSocket::SendUserList()
{
    global_parameters->lock_clients.lockForRead();
    QString line="/users:"+ ((QStringList)global_parameters->clients.values()).join(",");
    global_parameters->lock_clients.unlock();
    SendToAll(line);
}

void MessageSocket::SendColortype()
{
    global_parameters->lock_clients.lockForRead();

    foreach(MessageSocket *socket,global_parameters->clients.keys())
    {
        QString username=global_parameters->clients.value(socket);

        global_parameters->lock_clientsproperty.lockForRead();

        for(int i=0;i<global_parameters->clientsproperty.size();i++)
        {
            if(global_parameters->clientsproperty.at(i).name==username)
            {
                QString msg=QString::number(global_parameters->clientsproperty.at(i).colortype, 10);
                SendToAll(QString("/color:"+username+" "+msg+" "));break;
            }
        }
        global_parameters->lock_clientsproperty.unlock();
    }

    global_parameters->lock_clients.unlock();
}

void MessageSocket::SendCreaorMsg()
{
    global_parameters->lock_clients.lockForRead();

    foreach(MessageSocket *socket,global_parameters->clients.keys())
    {
        QString username=global_parameters->clients.value(socket);

        global_parameters->lock_clientsproperty.lockForRead();

        for(int i=0;i<global_parameters->clientsproperty.size();i++)
        {
            if(global_parameters->clientsproperty.at(i).name==username
                    &&global_parameters->clientsproperty.at(i).Creator)
            {
                QString msg= QString::number(global_parameters->clientsproperty.at(i).Creator_res, 10);
                SendToAll(QString("/creator:"+username+" "+msg+" "));break;
            }
        }
        global_parameters->lock_clientsproperty.unlock();
    }

    global_parameters->lock_clients.unlock();
}

void MessageSocket::updateUserMessage(QString username)
{
    int i=getUser(username);

    if(i==-1) return ;
    global_parameters->lock_clientsproperty.lockForRead();
    int messageindex=global_parameters->clientsproperty.at(i).messageindex;
    global_parameters->lock_clientsproperty.unlock();

    global_parameters->lock_messagelist.lockForRead();
    if(messageindex<global_parameters->messagelist.size())
    {
        global_parameters->lock_clientsproperty.lockForWrite();
        if(global_parameters->clientsproperty.at(i).online)
        {
            SendToUser(global_parameters->messagelist.at(messageindex));
            global_parameters->clientsproperty[i].messageindex++;
        }
        global_parameters->lock_clientsproperty.unlock();
    }else {
        qDebug()<<"user is outline can't update messagelist";
    }
    global_parameters->lock_messagelist.unlock();
}

bool MessageSocket::containsClient(const QString &name)
{
    bool res = false;

    global_parameters->lock_clientsproperty.lockForRead();
    for(int i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty.at(i).name==name)
        {
            res=true;break;
        }
    }
    global_parameters->lock_clientsproperty.unlock();
    return res;
}

int MessageSocket::getUser(const QString &name)
{
    int res=-1;
    global_parameters->lock_clientsproperty.lockForRead();
    for(int i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty.at(i).name==name)
        {
            res=i;break;
        }
    }
    global_parameters->lock_clientsproperty.unlock();
    return res;
}

void MessageSocket::MessageSocketSlot_start()
{
    this->setSocketDescriptor(socketId);
    connect(this,SIGNAL(readyRead()),this,SLOT(MessageSocketSlot_Read()));
    connect(this,SIGNAL(disconnected()),this,SLOT(MessageSocketSlot_disconnect()));
    qDebug()<<this->peerAddress().toString()<<" ThreadId:"<<QThread::currentThreadId();

}

void MessageSocket::MessageSocketSlot_disconnect()
{
    global_parameters->lock_clients.lockForWrite();
    QString username=global_parameters->clients.value(this);
    global_parameters->clients.remove(this);

    global_parameters->lock_clientsproperty.lockForWrite();
    for(int i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty.at(i).name == username)
            global_parameters->clientsproperty[i].online=false;
    }
    global_parameters->lock_clientsproperty.unlock();

    global_parameters->lock_clients.unlock();

    SendToAll(QString("/system:"+username+" left."));
    SendUserList();


    emit MessageSocketSignalToMessageServer_disconnected();
}

void MessageSocket::MessageSocketSlotAnswerToMessageServer_sendtoall(const QString &msg)
{
    SendToUser(msg);
}

MessageServer::MessageServer(QString filename,Global_Parameters *parameters,QObject *parent)
    :global_parameters(parameters),QTcpServer (parent),filename(filename)
{
    qDebug()<<"make a message server";
    NeuronList.clear();
    sketchNum=0;
    timer= new QTimer(this);
    connect(timer,SIGNAL(timeout),this,SLOT(timetoSave()));
    timer->start(300*1000);
}

void MessageServer::incomingConnection(int socketDesc)
{
    MessageSocket *messagesocket=new MessageSocket(socketDesc,global_parameters);

    QThread *thread=new QThread;

    messagesocket->moveToThread(thread);

    connect(thread,SIGNAL(started()),messagesocket,SLOT(MessageSocketSlot_start()));
    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_disconnected()),
            thread,SLOT(deleteLater()));
    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_disconnected()),
            messagesocket,SLOT(deleteLater()));
    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_disconnected()),
            this,SLOT(MessageServerSlotAnswerMessageSocket_disconnected()));
    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_sendtoall(const QString &msg)),
            this,SLOT(MessageServerSlotAnswerMessageSocket_sendtoall(const QString &msg)));
    connect(this,SIGNAL(MessageServerSignal_sendtoall(const QString &msg)),
            messagesocket,SLOT(MessageSocketSlotAnswerToMessageServer_sendtoall(const QString &msg)));

    thread->start();

}

void MessageServer::MessageServerSlotAnswerMessageSocket_sendtoall(const QString &msg)
{
    emit MessageServerSignal_sendtoall(msg);

}

void MessageServer::MessageServerSlotAnswerMessageSocket_disconnected()
{
    global_parameters->lock_clientNum.lockForWrite();
    if(--global_parameters->clientNum==0)
    {
        global_parameters->lock_clientNum.unlock();
        this->deleteLater();
        return;
    }
    global_parameters->lock_clientNum.unlock();

}

void MessageServer::timetoSave()
{

    timer->start(300*1000);
}

