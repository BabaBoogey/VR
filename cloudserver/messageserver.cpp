#include "messageserver.h"
#include <QDataStream>
#include <QByteArray>
#include <QtGlobal>
#include "QTextStream"


MessageServer::MessageServer(QString filename,Global_Parameters *parameters,QObject *parent)
    :global_parameters(parameters),QTcpServer (parent),filename(filename)
{
    qDebug()<<"make a message server";

    timer= new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(autoSave()));
    timer->start(6*1000);
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

        QRegExp fileExp("(.*).ano");
        if(fileExp.indexIn(filename)!=-1)
        {
            QDateTime time=QDateTime::currentDateTime();
            QString strtime=time.toString("yyyy_MM_dd_hh_mm_ss");

            QString tempname =fileExp.cap(1)+"_"+strtime;
                qDebug()<<tempname<<"here 100";

            QFile anofile("I://new/"+tempname+".ano");
            QString str1="APOFILE="+tempname+".ano.apo";
            QString str2="SWCFILE="+tempname+".ano.eswc";




//            QByteArray block;
//            QDataStream dts(&block,QIODevice::WriteOnly);
//            dts.setVersion(QDataStream::Qt_4_7);

//            dts<<str1<<"\n"<<str2;
//            anofile.write(block);
            anofile.close();
//            global_parameters->lock_wholeNT.lockForWrite();
//            writeESWC_file("I://new/"+tempname+".ano.eswc",global_parameters->wholeNT);
//            global_parameters->lock_wholeNT.unlock();
//            global_parameters->lock_wholePoint.lockForRead();
//            writeAPO_file("I://new/"+tempname+".ano.apo",global_parameters->wholePoint);
//            global_parameters->lock_wholePoint.unlock();
        }

        global_parameters->lock_clientNum.unlock();
        this->deleteLater();
        return;
    }
    global_parameters->lock_clientNum.unlock();

}

void MessageServer::autoSave()
{
    qDebug()<<"autosave";
    qDebug()<<filename;
//    QRegExp fileExp("(.*)_\d{4}_\d{2}_\d{2} \d{2}:\d{2}:\d{2}.ano");
    QRegExp fileExp("(.*).ano");
    if(fileExp.indexIn(filename)!=-1)
    {
        QDateTime time=QDateTime::currentDateTime();
        QString strtime=time.toString("yyyy_MM_dd_hh_mm_ss");

        QString tempname =fileExp.cap(1)+"_"+strtime;
        qDebug()<<tempname<<"here 100_1";

        QFile anofile("I://new/"+tempname+".ano");
        anofile.open(QIODevice::WriteOnly|QIODevice::Text);
        QString str1="APOFILE="+tempname+".ano.apo";
        QString str2="SWCFILE="+tempname+".ano.eswc";

//        QByteArray block;
//        QDataStream dts(&block,QIODevice::WriteOnly);
//        dts.setVersion(QDataStream::Qt_4_7);
//        dts<<str1<<"\n"<<str2;
//        anofile.write(block);

        QTextStream out(&anofile);
        out<<str1<<endl<<str2;
        anofile.close();
        qDebug()<<"here 08";
//            global_parameters->lock_wholeNT.lockForWrite();
            writeESWC_file("I://new/"+tempname+".ano.eswc",global_parameters->wholeNT);
            qDebug()<<"here 09";
//            global_parameters->lock_wholeNT.unlock();
//            global_parameters->lock_wholePoint.lockForRead();
            writeAPO_file("I://new/"+tempname+".ano.apo",global_parameters->wholePoint);
            qDebug()<<"here 10";
//            global_parameters->lock_wholePoint.unlock();
    }
    timer->start(5*60*1000);
}

