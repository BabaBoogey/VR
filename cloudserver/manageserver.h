#ifndef MANAGESERVER_H
#define MANAGESERVER_H
#include <QtNetwork>
#include <QRegExp>
#include <QDir>
#include <QFileInfoList>
#include "fileserver.h"
#include <QMap>
#include "messageserverandmessagesocket.h"

class ManageSocket:public QTcpSocket
{
    Q_OBJECT
public:
    explicit ManageSocket(QObject *parent=0);
public slots:
    void onReadyRead();
    void filereceived();


protected:
    QString currentDir();
    void sendFile(QTcpSocket* filesocket, QString filename);
signals:
    void makeMessageServer(ManageSocket *socket, QString anofilename);
private:
    QString name;
    QTcpSocket *filesocket;

};
//QMap <QString ,ManageSocket*> client_ManageMap;
//FileServer *fileserver=0;
//QMap <QString ,QString> annotation_portMap;
//QMap <QString ,MessageServer *> client_messageserverMap;

class ManageServer:public QTcpServer
{
    Q_OBJECT
public:
    explicit ManageServer(QObject *parent=0);

private:
    void incomingConnection(int socketDesc);
public slots:
    void makeMessageServer(ManageSocket*,QString);
private:
    QMap <QString ,MessageServer* > Map_File_MessageServer;

};

#endif // MANAGESERVER_H
