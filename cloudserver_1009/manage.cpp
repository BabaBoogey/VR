#include "manage.h"

FileServer *fileserver=0;
ManageServer::ManageServer(QObject *parent)
    :QTcpServer (parent)
{
    if(listen(QHostAddress::Any,9999))
    {
        qDebug()<<"ManageServer is started.";
    }else {
        qDebug()<<"ManageServer is not started ,please try again.";
    }
}

void ManageServer::incomingConnection(int socketDesc)
{
    ManageSocket *managesocket=new ManageSocket;
    managesocket->setSocketDescriptor(socketDesc);

    connect(managesocket,SIGNAL(makeMessageServer(ManageSocket *,QString)),
            this,SLOT(makeMessageServer(ManageSocket *,QString)));


}

void ManageServer::makeMessageServer(ManageSocket *managesocket,QString anofile_name)
{
    ;
}

ManageSocket::ManageSocket(QObject *parent)
    :QTcpSocket (parent)
{
    connect(this,SIGNAL(readyRead()),this,SLOT(readManage()));
    connect(this,SIGNAL(disconnected()),this,SLOT(deleteLater()));
}

void ManageSocket::readManage()
{
    QRegExp LoginRex("(.*):login.\n");
    QRegExp LogoutRex("(.*):logout.\n");
    QRegExp ImportRex("(.*):import.\n");
    QRegExp DownRex("(.*):down.\n");
    QRegExp LoadRex("(.*):load.\n");
    QRegExp FileRex("(.*):choose.\n");



    while(this->canReadLine())
    {
        QString manageMSG=QString::fromUtf8(this->readLine());
        qDebug()<<manageMSG;
        if(LoginRex.indexIn(manageMSG)!=-1)
        {
            QString username=LoginRex.cap(1);
            this->write(QString(username+":log in success."+"\n").toUtf8());
        }else if(LogoutRex.indexIn(manageMSG)!=-1)
        {
            QString username=LogoutRex.cap(1);
            this->write(QString(username+":log out success."+"\n").toUtf8());
        }else if(ImportRex.indexIn(manageMSG)!=-1)
        {
            QString username=ImportRex.cap(1);
            QString port_receivefile="9998";
            if(fileserver==0)
            {
                fileserver=new FileServer;
                // connect 注意哦要加的
                if(!fileserver->listen(QHostAddress::Any,port_receivefile.toInt()))
                {
                    qDebug()<<"error:cannot start fileserver.";
                    return;
                }
            }
            this->write(QString(QString::number(fileserver->serverPort())+":import port."+"\n").toUtf8());
        }else if(DownRex.indexIn(manageMSG)!=-1)
        {
            QString username=DownRex.cap(1);
            this->write(QString(currentDir()+":currentDir."+"\n").toUtf8());
        }else if(LoadRex.indexIn(manageMSG)!=-1)
        {
            QString username=LoadRex.cap(1);
//            this->write(QString(currentDir()+":currentDir."+"\n").toUtf8());
        }else if(FileRex.indexIn(manageMSG)!=-1)
        {
            QString filename=FileRex.cap(1);
            QString anopath="./clouddata/"+filename;
            qDebug()<<"3";
            FileSocket_send *filesocket=new FileSocket_send(this->peerAddress().toString(),"9998",anopath);

        }

    }
}

QString currentDir()
{
    QDir rootDir("./");
    if(!rootDir.cd("clouddata"))
    {
        rootDir.mkdir("clouddata");
        rootDir.cd("clouddata");
    }
    QFileInfoList list=rootDir.entryInfoList();

    QStringList TEMPLIST;
    TEMPLIST.clear();

    for(unsigned i=0;i<list.size();i++)
    {
        QRegExp anoRex("(.*).ano");
        QRegExp eswcRex("(.*).ano.eswc");
        QRegExp apoRex("(.*).ano.apo");
        QFileInfo tmpFileInfo=list.at(i);
        if(!tmpFileInfo.isDir())
        {
            QString fileName = tmpFileInfo.fileName();
            if(anoRex.indexIn(fileName)!=-1&&eswcRex.indexIn(fileName)==-1&&apoRex.indexIn(fileName)==-1)
                TEMPLIST.append(fileName);
        }
    }
    return TEMPLIST.join(";");
}






