#ifndef MANAGESERVER_H
#define MANAGESERVER_H
#include <QtNetwork>
#include <QRegExp>
#include <QDir>
#include <QFileInfoList>
#include <QMap>
#include "receive_file.h"

#include "messageserver.h"
//class ManageSocket;


QString currentDir();
class FileServer;



class ManageServer:public QTcpServer
{
    Q_OBJECT
public:
    explicit ManageServer(QObject *parent=0);
private:
    void incomingConnection(int socketDesc);
public slots:
    void makeMessageServer(ManageSocket *managesocket,QString anofile_name);
    void messageserver_ondeltete(QString);
public:
    QMap <QString ,MessageServer* > Map_File_MessageServer;
    QMap <QString,int> userList;
public:
signals:
    void userload(ForAUTOSave);

};

//struct ForAUTOSave
//{

//    QString ip;
//    FileServer_send *fileserver_send=0;
//    ManageSocket *managesocket=0;
//    QMap <QString ,MessageServer* > *Map_File_MessageServer=0;
//};
#endif // MANAGESERVER_H
