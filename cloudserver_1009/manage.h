#ifndef MANAGESERVER_H
#define MANAGESERVER_H
#include <QtNetwork>
#include <QRegExp>
#include <QDir>
#include <QFileInfoList>
#include <QMap>
#include "receive_file.h"
#include "send_file.h"

QString currentDir();
class FileServer;

class ManageSocket:public QTcpSocket
{
    Q_OBJECT
public:
    explicit ManageSocket(QObject *parent=0);
public slots:
    void readManage();
//    void ondisconnect();

protected:
signals:
    void makeMessageServer(ManageSocket *managesocket,QString anofile_name);

private:



};

class ManageServer:public QTcpServer
{
    Q_OBJECT
public:
    explicit ManageServer(QObject *parent=0);
private:
    void incomingConnection(int socketDesc);
public slots:
    void makeMessageServer(ManageSocket *managesocket,QString anofile_name);
private:
public:

};



#endif // MANAGESERVER_H
