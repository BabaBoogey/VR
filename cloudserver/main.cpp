#include <QCoreApplication>
#include "manageserver.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//    QMap <QString ,ManageSocket*> client_ManageMap;
//    FileServer *fileserver=0;
//    QMap <QString ,QString> annotation_portMap;
    ManageServer manageserver;
    return a.exec();
}
