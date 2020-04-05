﻿#include "send_file.h"

FileServer_send::FileServer_send(QObject *parent):QTcpServer(parent)
{
    list.clear();
    this->listen(QHostAddress::Any,9997);
}

void FileServer_send::incomingConnection(int socketDesc)
{

    FileSocket_send *filesocket=new FileSocket_send;
    filesocket->setSocketDescriptor(socketDesc);
    list.push_back(filesocket);
}

void FileServer_send::sendFile(QString ip, QString filename)
{
    for(int i=0;i<list.size();i++)
    {
        if(list[i]->peerAddress().toString()==ip)
        {
            FileSocket_send *temp=list[i];
            list.removeAt(i);
            temp->anoname=filename;
            if(filename.contains("stamp"))
                temp->sendFile("./autosave/"+filename,filename);
            else
                temp->sendFile("./clouddata/"+filename,filename);
            break;
        }
    }
}

void FileServer_send::sendV3draw(QString ip, QString filename)
{
    for(int i=0;i<list.size();i++)
    {
        if(list[i]->peerAddress().toString()==ip)
        {
            FileSocket_send *temp=list[i];
            list.removeAt(i);
            temp->anoname.clear();
            temp->sendFile("./v3draw/"+filename,filename);
            temp->disconnectFromHost();
        }
    }
}
FileSocket_send::FileSocket_send(QObject *parent)
    :QTcpSocket (parent)
{
    connect(this,SIGNAL(disconnected()),this,SLOT(deleteLater()));
    connect(this,SIGNAL(readyRead()),this,SLOT(readMSG()));
}

void FileSocket_send::sendFile(QString filepath, QString filename)
{
    QFile f(filepath);
//    qDebug()<<filepath;


    if(f.exists()&&f.open(QIODevice::ReadOnly))
    {
//            qDebug()<<filename;
        QByteArray data=f.readAll();
        QByteArray block;

        QDataStream dts(&block,QIODevice::WriteOnly);
        dts.setVersion(QDataStream::Qt_4_7);

        dts<<qint64(0)<<qint64(0)<<filename;
        dts.device()->seek(0);
        dts<<(qint64)(block.size()+f.size());
        dts<<(qint64)(block.size()-sizeof(qint64)*2);
        dts<<filename;
        dts<<data;

        this->write(block);
        this->flush();
        this->waitForBytesWritten();
//        qDebug()<<"asafas";
    }else
    {
        disconnectFromHost();
    }

}

void FileSocket_send::readMSG()
{
    while (this->canReadLine()) {
        QRegExp anoRex("received (.*).ano\n");
        QRegExp swcRex("received (.*).eswc\n");
        QRegExp apoRex("received (.*).apo\n");
        QRegExp txtRex("received (.*).txt\n");
        QString MSG=QString::fromUtf8(this->readLine());
        if(anoRex.indexIn(MSG)!=-1)
        {
            if(anoname.contains("stamp"))
                sendFile("./autosave/"+anoname+".eswc",anoname+".eswc");
            else
                sendFile("./clouddata/"+anoname+".eswc",anoname+".eswc");
        }else if(swcRex.indexIn(MSG)!=-1)
        {
            if(anoname.contains("stamp"))
                sendFile("./autosave/"+anoname+".apo",anoname+".apo");
            else
            sendFile("./clouddata/"+anoname+".apo",anoname+".apo");
        }else if(apoRex.indexIn(MSG)!=-1)
        {
            this->disconnectFromHost();
        }
    }
}


