#include <QCoreApplication>
#include "manage.h"
#include "customdebug.h"


#define IMAGEDIR "image"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qInstallMsgHandler(customMessageHandler);
    ManageServer manageserver;
//    QRegExp ImgBlockRex("(.*):imgblock.\n");
//    if(ImgBlockRex.indexIn(QString("17302_00001RES(54600x34412x9847)__128__128__128__64:imgblock.\n"))!=-1)
//        {
//            QStringList paraList=ImgBlockRex.cap(1).trimmed().split("__",QString::SkipEmptyParts);
//            QString filename=paraList.at(0);//1. tf name/RES  2. .v3draw// test:17302_00001/RES(54600x34412x9847);

//            int xpos=paraList.at(1).toInt();
//            int ypos=paraList.at(2).toInt();
//            int zpos=paraList.at(3).toInt();
//            int blocksize=paraList.at(4).toInt();

//            if(filename.contains("v3draw"))
//            {
//                qDebug()<<"error";
//            }
//            else
//            {
//                QRegExp tmp("(.*)RES(.*)");
//                tmp.indexIn(filename);

//                QString __=tmp.cap(1).trimmed();
//                QString string=__+"_" +QString::number(xpos)+ "_" + QString::number(xpos) + "_" + QString::number(xpos)+
//                        "_" +QString::number(blocksize)+"_"+QString::number(blocksize)+ "_"+QString::number(blocksize);
//                qDebug()<<__;
//                qDebug()<<string;


//                qDebug()<<"-=================-----";
//                QProcess p;
//                CellAPO centerAPO;
//                centerAPO.x=xpos;centerAPO.y=ypos;centerAPO.z=zpos;
//                QList <CellAPO> List_APO_Write;
//                List_APO_Write.push_back(centerAPO);
//                writeAPO_file(string+".apo",List_APO_Write);//get .apo to get .v3draw
//                qDebug()<<"-=================-----ecfd";


////                qDebug()<<"-============dsadad=====-----ecfd";
////                qDebug()<<p.execute("xvfb-run -a ./vaa3d",QStringList()
////                          <<"-x"<<"./plugins/image_geometry/crop3d_image_series/libcropped3DImageSeries.so"
////                          <<"-f"<<"cropTerafly"<<"-i"<<"./"+QString(IMAGEDIR)+"/"+filename<<string+".apo"
////                          <<"./v3draw/"<<"-p"<<QString::number(blocksize)<<QString::number(blocksize)<<QString::number(blocksize));
//                qDebug()<<"-====dedede=============-----ecfd";

////                QString fName=QString("%1.000_%2.000_%3.000.v3draw").arg(xpos).arg(ypos).arg(zpos);

//    //            fileserver_send->sendV3draw(this->peerAddress().toString(),fName);
//                //                    {
//                //                        QFile f1(string+".apo"); f1.remove();
//                //                        QFile f2("./vaa3d/"+fName); f2.remove();
//                //                    }
//            }

//        }
    return a.exec();
}
