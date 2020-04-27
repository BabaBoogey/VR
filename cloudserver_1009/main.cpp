#include <QCoreApplication>
#include "manage.h"
#include "customdebug.h"


#define IMAGEDIR "image"

int Y[]={15600,43327,16677,17537,13740};
int X[]={16200,16175,22846,20058,15127};
int Z[]={2950,2700,5470,3147,4987};

int blocksize=64;


//QString("17302_00001RES(54600x34412x9847)"),
//QString("17302_00098RES(54600x34412x9847)"),
//QString("17545_00089RES(54600x35989x10750)"),
//QString("17781_00001RES(54600x34722x11021)"),
//QString("18457_00110RES(24275x37800x10955)")
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qInstallMsgHandler(customMessageHandler);
//    ManageServer manageserver;
    QString filename=QString("17302_00098RES(54600x34412x9847)");//1. tf name/RES  2. .v3draw// test:17302_00001RES(54600x34412x9847);


    for( int i=0;i<10;i++)
    {
        int xpos=X[1]+i*10;
        int ypos=Y[1]+i*15;
        int zpos=Z[1]+i*5;


        QRegExp tmp("(.*)RES(.*)");
        tmp.indexIn(filename);

        QString __=tmp.cap(1).trimmed();
        QString string=__+"_" +QString::number(xpos)+ "_" + QString::number(ypos) + "_" + QString::number(zpos)+
                "_" +QString::number(blocksize)+"_"+QString::number(blocksize)+ "_"+QString::number(blocksize);
                        qDebug()<<__;
                        qDebug()<<string;
        QProcess p;
        CellAPO centerAPO;
        centerAPO.x=xpos;centerAPO.y=ypos;centerAPO.z=zpos;
        QList <CellAPO> List_APO_Write;
        List_APO_Write.push_back(centerAPO);
        writeAPO_file(string+".apo",List_APO_Write);//get .apo to get .v3draw
        qDebug()<<"-=================-----ecfd";


        QString order1 =QString("xvfb-run -a ./vaa3d -x ./plugins/image_geometry/crop3d_image_series/libcropped3DImageSeries.so "
                                "-f cropTerafly -i ./%0/%1/ %2.apo ./ -p %3 %4 %5")
                .arg(IMAGEDIR).arg(filename).arg(string).arg(blocksize).arg(blocksize).arg(blocksize);
                        qDebug()<<"order="<<order1;
        qDebug()<<p.execute(order1.toStdString().c_str());
    }


//            QString fName=QString("%1.000_%2.000_%3.000.v3draw").arg(xpos).arg(ypos).arg(zpos);
//            qDebug()<<"1";
//        fileserver_send->sendV3draw(this->peerAddress().toString(),fName);
//        qDebug()<<"-============fesfsef=====-----ecfd";
//                {
//                    QFile f1(string+".apo"); qDebug()<<f1.remove();
//                    QFile f2("./"+fName); qDebug()<<f2.remove();
//                }
//                this->disconnectFromHost();


    return a.exec();
}
