#include "messageserver.h"
#include <QDataStream>
#include <QByteArray>
#include <QtGlobal>
#include "QTextStream"


MessageServer::MessageServer(QString filename,Global_Parameters *parameters,QObject *parent)
    :global_parameters(parameters),QTcpServer (parent),filename(filename)
{
    qDebug()<<"make a message server";
//    connect(global_parameters->timer,SIGNAL(timeout()),this,SLOT(autoSave()));
    global_parameters->timer->start(6*1000);
}

void MessageServer::incomingConnection(int socketDesc)
{
    qDebug()<<"a new connection";
    MessageSocket *messagesocket=new MessageSocket(socketDesc,global_parameters);

    QThread *thread=new QThread;

    messagesocket->moveToThread(thread);

    connect(thread,SIGNAL(started()),messagesocket,SLOT(MessageSocketSlot_start()));
    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_disconnected()),
            messagesocket,SLOT(deleteLater()));
    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_disconnected()),
            thread,SLOT(deleteLater()));
    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_disconnected()),
            this,SLOT(MessageServerSlotAnswerMessageSocket_disconnected()));
    connect(messagesocket,
            SIGNAL(MessageSocketSignalToMessageServer_sendtoall(const QString &)),
            this,SLOT(MessageServerSlotAnswerMessageSocket_sendtoall(const QString &)));
    connect(this,SIGNAL(MessageServerSignal_sendtoall(const QString &)),
            messagesocket,
            SLOT(MessageSocketSlotAnswerToMessageServer_sendtoall(const QString &)));

    connect(messagesocket,SIGNAL(signal_addseg(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_addseg(QString)));
    connect(messagesocket,SIGNAL(signal_delseg(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_delseg(QString)));
    connect(messagesocket,SIGNAL(signal_addmarker(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_addmarker(QString)));
    connect(messagesocket,SIGNAL(signal_delmarker(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_delmarker(QString)));


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

        QRegExp fileExp("(.*)_stamp_(.*).ano");
        if(fileExp.indexIn(filename)!=-1)
        {
            QDateTime time=QDateTime::currentDateTime();
            QString strtime=time.toString("yyyy_MM_dd_hh_mm_ss");

            QString tempname =fileExp.cap(1)+"_stamp_"+strtime;
                qDebug()<<tempname<<"here 100";

            QFile anofile("./clouddata/"+tempname+".ano");
            QString str1="APOFILE="+tempname+".ano.apo";
            QString str2="SWCFILE="+tempname+".ano.eswc";

            QTextStream out(&anofile);
            out<<str1<<endl<<str2;
            anofile.close();

            global_parameters->lock_wholeNT.lockForWrite();
            writeESWC_file("./clouddata/"+tempname+".ano.eswc",global_parameters->wholeNT);
            global_parameters->lock_wholeNT.unlock();
            global_parameters->lock_wholePoint.lockForRead();
            writeAPO_file("./clouddata/"+tempname+".ano.apo",global_parameters->wholePoint);
            global_parameters->lock_wholePoint.unlock();
        }

        global_parameters->lock_clientNum.unlock();
        this->deleteLater();
        return;
    }
    global_parameters->lock_clientNum.unlock();

}

void MessageServer::MessageServerSlotAnswerMessageSocket_addseg(QString MSG)
{
    qDebug()<<"MessageServerSlotAnswerMessageSocket_addseg";
    /*MSG=QString("/seg:"+user + " " + msg)*/
    QRegExp Reg("/seg:(.*) (.*)");
    QString seg;
    QString username;
    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1);
        seg=Reg.cap(2);
    }

    global_parameters->lock_clientsproperty.lockForRead();
    int colortype=21;
    for(int i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty.at(i).name==username)
        {
            colortype=global_parameters->clientsproperty.at(i).colortype;
            break;
        }
    }
    global_parameters->lock_clientsproperty.unlock();

}

void MessageServer::MessageServerSlotAnswerMessageSocket_delseg(QString MSG)
{
    qDebug()<<"MessageServerSlotAnswerMessageSocket_delseg";
    /*MSG=QString("/del_curve:"+user + " " + msg)*/
    QRegExp Reg("/del_curve:(.*) (.*)");
    QString delseg;
    QString username;
    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1);
        delseg=Reg.cap(2);
    }

    global_parameters->lock_clientsproperty.lockForRead();
    int colortype=21;
    for(int i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty.at(i).name==username)
        {
            colortype=global_parameters->clientsproperty.at(i).colortype;
            break;
        }
    }
    global_parameters->lock_clientsproperty.unlock();



}

void MessageServer::MessageServerSlotAnswerMessageSocket_addmarker(QString MSG)
{
    qDebug()<<"MessageServerSlotAnswerMessageSocket_addmarker";
    /*MSG=QString("/marker:" +user+" "+markermsg)*/
    QRegExp Reg("/marker:(.*) (.*)");
    QString markerpos;
    QString username;
    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1);
        markerpos=Reg.cap(2);
    }
    global_parameters->lock_clientsproperty.lockForRead();
    int colortype=21;
    for(int i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty.at(i).name==username)
        {
            colortype=global_parameters->clientsproperty.at(i).colortype;
            break;
        }
    }
    global_parameters->lock_clientsproperty.unlock();
}
void MessageServer::MessageServerSlotAnswerMessageSocket_delmarker(QString MSG)
{
    qDebug()<<"MessageServerSlotAnswerMessageSocket_delmarker";
    /*MSG=QString("/del_marker:" +user+" "+delmarkerpos )*/
    QRegExp Reg("/del_marker:(.*) (.*)");
    QString delmarkerpos;
    QString username;
    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1);
        delmarkerpos=Reg.cap(2);
    }

    global_parameters->lock_clientsproperty.lockForRead();
    int colortype=21;
    for(int i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty.at(i).name==username)
        {
            colortype=global_parameters->clientsproperty.at(i).colortype;
            break;
        }
    }
    global_parameters->lock_clientsproperty.unlock();
}


void MessageServer::autoSave()
{
    qDebug()<<"autosave";
    QDir rDir("./");
    if(!rDir.cd("autosave"))
    {
        rDir.mkdir("autosave");
    }
    QRegExp fileExp("(.*)_stamp_(.*).ano");
    if(fileExp.indexIn(filename)!=-1)
    {
        QDateTime time=QDateTime::currentDateTime();
        QString strtime=time.toString("yyyy_MM_dd_hh_mm_ss");

        QString tempname =fileExp.cap(1)+"_stamp_autosave_"+strtime;
            qDebug()<<tempname<<"here 100";

        QFile anofile("./autosave/"+tempname+".ano");
        QString str1="APOFILE="+tempname+".ano.apo";
        QString str2="SWCFILE="+tempname+".ano.eswc";

        QTextStream out(&anofile);
        out<<str1<<endl<<str2;
        anofile.close();

        global_parameters->lock_wholeNT.lockForWrite();
        writeESWC_file("./autosave/"+tempname+".ano.eswc",global_parameters->wholeNT);
        global_parameters->lock_wholeNT.unlock();
        global_parameters->lock_wholePoint.lockForRead();
        writeAPO_file("./autosave/"+tempname+".ano.apo",global_parameters->wholePoint);
        global_parameters->lock_wholePoint.unlock();
    }
    global_parameters->timer->start(5*60*1000);
}



