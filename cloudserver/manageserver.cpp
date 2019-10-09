#include "manageserver.h"
#include <QtGlobal>
#include "basic_surf_objs.h"

 QMap <QString ,ManageSocket*> client_ManageMap;
 FileServer *fileserver=0;
//extern QMap <QString ,QString> annotation_portMap;
ManageServer::ManageServer(QObject *parent):QTcpServer (parent)
{
    if(this->listen(QHostAddress::Any,9999))
    {
            qDebug()<<"ManageServer is started.";
    }else{
        qDebug()<<"ManageServer is not started ,please try again.";
    }
}

void ManageServer::incomingConnection(int socketDesc)
{
    qDebug()<<"new connection.";
    ManageSocket *managesocket=new ManageSocket;

    managesocket->setSocketDescriptor(socketDesc);
    connect(managesocket,SIGNAL(readyRead()),managesocket,SLOT(onReadyRead()));
    connect(managesocket,SIGNAL(makeMessageServer(ManageSocket*,QString)),this,SLOT(makeMessageServer(ManageSocket*,QString)));
    connect(managesocket,SIGNAL(disconnected()),managesocket,SLOT(deleteLater()));
    connect(managesocket,SIGNAL(disconnected()),managesocket,SLOT(deleteclient()));

}
void ManageServer::makeMessageServer(ManageSocket *socket,QString filename)
{
    QRegExp fileExp("(.*).ano");
    if(fileExp.indexIn(filename)!=-1)
    {
        if(!Map_File_MessageServer.contains(filename))
        {
          label:  qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
            QString messageport=QString::number(qrand()%2000+5000);

            foreach(QString filename,Map_File_MessageServer.keys())
            {
                if(Map_File_MessageServer.value(filename)->serverPort()==messageport.toInt())
                    goto label;
            }


            qDebug()<<messageport;
            Global_Parameters *global_parameters=new Global_Parameters;
            global_parameters->Creator.clear();
            global_parameters->clients.clear();
            global_parameters->clientNum=0;
            global_parameters->clientsproperty.clear();
            global_parameters->messagelist.clear();

            global_parameters->sketchNum=0;

            global_parameters->wholeNT=readSWC_file("I://new/"+fileExp.cap(1)+".ano.eswc");
            global_parameters->wholePoint=readAPO_file("I://new/"+fileExp.cap(1)+".ano.apo");



            MessageServer *messageserver=new MessageServer(filename, global_parameters);

            if(!messageserver->listen(QHostAddress::Any,messageport.toInt()))
            {
                qDebug()<<filename<<" MessageServer is not started.";
            }else {
                 socket->write(QString("messageport:"+messageport+".\n").toUtf8());
                 qDebug()<<"here 3";
                 Map_File_MessageServer[filename]=messageserver;
            }
        }else {
            QString messageport=QString::number(Map_File_MessageServer[filename]->serverPort());
            socket->write(QString("messageport:"+messageport+".\n").toUtf8());
        }
    }
}


void ManageSocket::deleteclient()
{
    foreach(QString user,client_ManageMap.keys())
    {
        if(client_ManageMap.value(user)==this)
        {
            client_ManageMap.remove(user);break;
        }
    }
}

ManageSocket::ManageSocket(QObject *parent)
{
    filesocket=0;
}

void ManageSocket::onReadyRead()
{
    QRegExp LoginRex("(.*):login.\n");
    QRegExp LogoutRex("(.*):logout.\n");
    QRegExp ImportRex("(.*):import.\n");
    QRegExp DownloadRex("(.*):download.\n");
    QRegExp LoadRex("(.*):load.\n");
    QRegExp FileNameRex("(.*) choose:(.*)\n");
    QRegExp LoadFileRex("(.*) load:(.*)\n");
    if(this->canReadLine())
    {
        QString manageMsg=QString::fromUtf8(this->readLine());
        qDebug()<<manageMsg;
        if(LoginRex.indexIn(manageMsg)!=-1 )
        {
            name=LoginRex.cap(1);
            qDebug()<<QString(name+":logged in.");
            client_ManageMap[name]=this;
            this->write(QString(name+":logged in."+"\n").toUtf8());
        }else if(LogoutRex.indexIn(manageMsg)!=-1)
        {
            qDebug()<<QString(name+":logged out."+"\n");
            name=LogoutRex.cap(1);

            this->write(QString(name+":logged out."+"\n").toUtf8());
        }else if(ImportRex.indexIn(manageMsg)!=-1)
        {
            name=ImportRex.cap(1);
            QString fileport="9998";// （ fileserver's port=9998）
            if(fileserver==0)
            {
                qDebug()<<"22222";
                fileserver=new FileServer;
                if(fileserver->listen(QHostAddress::Any,fileport.toInt()))
                      qDebug()<<"fileserver made successfully"  ;
            }
            this->write(QString(name+":import port :"+
                 QString::number(fileserver->serverPort())+".\n").toUtf8());
        }else if(DownloadRex.indexIn(manageMsg)!=-1)
        {
            name=DownloadRex.cap(1);
            qDebug()<<QString("currentDir:"+currentDir());
            this->write(QString("currentDir:"+currentDir()+".\n").toUtf8());
        }else if(LoadRex.indexIn(manageMsg)!=-1)
        {
            name=LoadRex.cap(1);
            qDebug()<<name<<"send load";

            this->write(("loadcurrentDir:"+currentDir()+".\n").toUtf8());
        }else if(FileNameRex.indexIn(manageMsg)!=-1)
        {
            qDebug()<<"here 1";
            QString filename=FileNameRex.cap(2);
            qDebug()<<filename;//here
            QString fileport="9998";
            filesocket=new QTcpSocket;
            connect(filesocket,SIGNAL(disconnected()),
                          filesocket,SLOT(deleterlater()));

            connect(filesocket,SIGNAL(readyRead()),this,SLOT(filereceived()));
            filesocket->connectToHost(this->peerAddress().toString(),fileport.toInt());
            if(filesocket->waitForConnected())
            {
                sendFile(filesocket,filename);
            }

        }else if(LoadFileRex.indexIn(manageMsg)!=-1)
        {
            qDebug()<<"here 2";
            QString filename=LoadFileRex.cap(2);
            qDebug()<<filename;
            emit makeMessageServer(this,filename);

            qDebug()<<"makeMessageServer";
        }
    }
}

void ManageSocket::filereceived()
{
    qDebug()<<"88888999";
    if(filesocket->canReadLine())
    {
        QString fileMsg=QString::fromUtf8(filesocket->readLine());;
        qDebug()<<"recive fileMsg"<<fileMsg;
        QRegExp ReceivedRex("received (.*)\n");


        if(ReceivedRex.indexIn(fileMsg)!=-1)
           {
                QRegExp anoRex("(.*).ano");
                QRegExp eswcRex("(.*).ano.eswc");
                QRegExp apoRex("(.*).ano.apo");
                QString receivedfilename=ReceivedRex.cap(1);
                qDebug()<<"receivedfilename"<<receivedfilename;
                if(anoRex.indexIn(receivedfilename)!=-1&&eswcRex.indexIn(receivedfilename)==-1&&apoRex.indexIn(receivedfilename)==-1)
                {
                    qDebug()<<"123";
                    sendFile(filesocket,anoRex.cap(1)+".ano.eswc");
                }
                else if(eswcRex.indexIn(receivedfilename)!=-1)
                {
                    qDebug()<<"124";
                    sendFile(filesocket,eswcRex.cap(1)+".ano.apo");
                }
                else if(apoRex.indexIn(receivedfilename)!=-1)
                {
                    qDebug()<<"125";
                    filesocket->disconnectFromHost();
                }

            }
        else {
            qDebug()<<"false";
            filesocket->disconnectFromHost();
        }
    }
}

QString ManageSocket::currentDir()
{
    QString root="I://new";
    QDir rootDir(root);
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

void ManageSocket::sendFile(QTcpSocket *filesocket, QString filename)
{
    QFile f("I://new/"+filename);
    f.open(QIODevice::ReadOnly);
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

    filesocket->write(block);
    qDebug()<<"send:"<<"I://new/"+filename;

}


