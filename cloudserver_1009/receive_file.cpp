#include "receive_file.h"
FileServer::FileServer(QObject *parent):QTcpServer (parent)
{
    clientNum=0;
}

void FileServer::incomingConnection(int socketDesc)
{
    FileSocket_receive *filesocket=new FileSocket_receive(socketDesc);
//    connect(filesocket,SIGNAL(receivefile(QString)),this,SIGNAL(receivedfile(QString)));
    connect(filesocket,SIGNAL(disconnected()),this,SLOT(Socketdisconnect()));
    clientNum++;
}

void FileServer::Socketdisconnect()
{
    if(--clientNum==0)
    {
        emit fileserverdeleted();
    }
}

FileSocket_receive::FileSocket_receive(int socketDesc,QObject *parent)
    :socketId(socketDesc),QTcpSocket (parent)
{
    totalsize=0;
    filenamesize=0;
    m_bytesreceived=0;
    this->setSocketDescriptor(socketId);
    connect(this,SIGNAL(disconnected()),this,SLOT(deleteLater()));
    connect(this,SIGNAL(readyRead()),this,SLOT(readFile()));
}


void FileSocket_receive::readFile()
{
    if(this->m_bytesreceived==0)
    {
        if(this->bytesAvailable()>=16)
        {
            totalsize=this->read(8).toULong();
            filenamesize=this->read(8).toULong();
            m_bytesreceived+=16;
        }
        if(this->bytesAvailable()+m_bytesreceived>=totalsize)
        {
            QDir rootDir("./");
            if(!rootDir.cd("clouddata"))
            {
                rootDir.mkdir("clouddata");
                rootDir.cd("clouddata");
            }

            QString filename=read(filenamesize);
            qDebug()<<filename;
            QByteArray block=readAll();
            QFile file("./clouddata/"+filename);
            file.open(QIODevice::WriteOnly);
            file.write(block);
            file.close();
            m_bytesreceived=0;
            this->write(QString("received "+filename+"\n").toUtf8());
//            qDebug()<<QString("received "+filename+"\n");
        }
    }else {
            if(this->bytesAvailable()+m_bytesreceived>=totalsize)
            {
                QDir rootDir("./");
                if(!rootDir.cd("clouddata"))
                {
                    rootDir.mkdir("clouddata");
                    rootDir.cd("clouddata");
                }

                QString filename=read(filenamesize);
                qDebug()<<filename;
                QByteArray block=readAll();
                QFile file("./clouddata/"+filename);
                file.open(QIODevice::WriteOnly);
                file.write(block);
                file.close();
                m_bytesreceived=0;
                this->write(QString("received "+filename+"\n").toUtf8());
    //            qDebug()<<QString("received "+filename+"\n");

            }
        }
}



