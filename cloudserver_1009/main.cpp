#include <QCoreApplication>
#include "customdebug.h"
#include <QFileInfo>
#include "analyse.h"
#include <QDir>
const QString SWCDir="C:/Users/Brain/Desktop/t";
//#include <QFileInfo>


//#define IMAGEDIR "image"

// void get_random_number()
// {
//     qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
//     int a = qrand()%9;   //随机生成0到9的随机数
//     qDebug()<< a;
// }
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//    qInstallMsgHandler(customMessageHandler);

    QFileInfoList swcList=QDir(SWCDir).entryInfoList(QDir::Files|QDir::NoDotAndDotDot);

    QDir("./").mkdir("swc");
    QDir("./").mkdir("analysetype_distance");
    QDir("./").mkdir("analysetime_distance");
    QDir("./").mkdir("analyseBB_count");
    for(int i=0;i<swcList.size();i++)
    {
        qDebug()<<i;
        analyseSWC(swcList[i]);
    }
    qDebug()<<"end";

//    NeuronTree nt=readSWC_file("C:/Users/Brain/Desktop/18869_00004.ano.eswc");
//    qDebug()<<"nt.listNeuron.size()="<<nt.listNeuron.size();

//    int blocksize=128;
//    QList <CellAPO> List_APO_Write;
//    qsrand(time(NULL));
//    for(int j=0;j<4;j++)
//    {
//        for(int i=0;i<1500;)
//        {

//            int a = qrand()%nt.listNeuron.size();   //随机生成0到9的随机数

//            if((nt.listNeuron[a].x>=0  &&nt.listNeuron[a].x+128<=29400)&&
//                    (nt.listNeuron[a].y>=0  &&nt.listNeuron[a].y+128<=10923)&&
//                    (nt.listNeuron[a].z>=0  &&nt.listNeuron[a].z+128<=16915))
//            {
//                CellAPO centerAPO;
//                centerAPO.n=nt.listNeuron[a].n;
//                centerAPO.x=nt.listNeuron[a].x;
//                centerAPO.y=nt.listNeuron[a].y;
//                centerAPO.z=nt.listNeuron[a].z+blocksize/2;
//                List_APO_Write.push_back(centerAPO);
//                i++;
//            }
//        }
//        writeAPO_file(QString::number(j)+"V3APO.apo",List_APO_Write);
//    }
//    QList <ImageMarker> tmp1;
//    ImageMarker startPoint;//Local
//    startPoint.x=blocksize/2;startPoint.y=blocksize/2;
//    startPoint.z=0;
//    tmp1.push_back(startPoint);
//    writeMarker_file("./tmp.marker",tmp1);



//    ManageServer manageserver;


    //    for(int i=0;i<list.size();i++)
    //    {
    //        NeuronTree nt=readSWC_file(list[i].filePath());
    //        QList<CellAPO> wapo;
    //        int cnt=0;
    //        for(int j=0;j<nt.listNeuron.size();j++)
    //        {
    //            if(nt.listNeuron[j].pn==-1)
    //            {
    //                CellAPO apo;
    //                apo.x=nt.listNeuron[j].x;
    //                apo.y=nt.listNeuron[j].y;
    //                apo.z=nt.listNeuron[j].z;
    //                apo.color.r=200;apo.color.g=200;apo.color.b=0;
    //                apo.n=++cnt;
    //                wapo.push_back(apo);
    //            }
    //        }
    //        for(int j=0;j<wapo.size();j++)
    //        {
    //            CellAPO p1=wapo[j];
    //            for(int k=j+1;k<wapo.size();k++)
    //            {
    //                CellAPO p2=wapo[k];
    //                double d=(p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z);
    //                if(d<=20) wapo.removeAt(k--);
    //            }
    //        }
    //        QRegExp exp("(.*)single2_17_(.*).eswc");
    //        if(exp.indexIn(list[i].filePath())!=-1)
    //        {
    //            qDebug()<<exp.cap(1)+exp.cap(2)+".apo" <<"   "<<cnt;
    //            writeAPO_file(exp.cap(1)+exp.cap(2)+".apo",wapo);
    //        }


    //    }
//    QFileInfoList list=QDir("C:/Users/Brain/Desktop/t").entryInfoList(QDir::NoDotAndDotDot|QDir::Files);
//    for(int i=0;i<list.size();i++)
//    {
//        QList<CellAPO> wapo=readAPO_file(list[i].filePath());
//        for(int j=0;j<wapo.size();j++)
//        {
//            CellAPO p1=wapo[j];
//            if(p1.color.b==200) wapo.removeAt(j--);
//            for(int k=j+1;k<wapo.size();k++)
//            {
//                CellAPO p2=wapo[k];
//                double d=(p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z);
//                if(d<=5) wapo.removeAt(k--);
//            }
//        }
//        qDebug()<<wapo.size();
//        writeAPO_file(list[i].filePath(),wapo);
//    }



//    V_NeuronSWC__2__NeuronTree(seg_temp);
//        SS.nameQString filename(":/new/prefix1/C:/Users/Brain/Desktop/resample_18465_00303_SYY_YLL_YLL_stamp_2019_09_27_10_40.ano.eswc");
//        NeuronTree L0=readSWC_file(filename);
//        NeuronTree L2=readSWC_file(":/new/prefix1/C:/Users/Brain/Desktop/18465_00303.ano.eswc");

//        QList<NeuronTree> MergedNT;

//        V_NeuronSWC_list V_L2= NeuronTree__2__V_NeuronSWC_list(L2);
//        V_NeuronSWC_list V_L0= NeuronTree__2__V_NeuronSWC_list(L0);
//        for(int i=0;i<V_L2.seg.size();i++)
//        {
//            NeuronTree SS;
//            V_NeuronSWC seg_temp =  V_L2.seg.at(i);
//            seg_temp.reverse();
//            for(int i=0;i<seg_temp.row.size();i++)
//            {
//                seg_temp.row.at(i).type=2;
//                seg_temp.row.at(i).r=1;
//            }
//            SS =  = "loaded_" + QString("%1").arg(i);
//        if (SS.listNeuron.size()>=2)
//            MergedNT.push_back(SS);
//    }

////    qDebug()<<"gdhksjgfkhjsagfkhj\n\n\n";
//    QList<NeuronTree> ColordNT;
//    for(int i=0;i<V_L0.seg.size();i++)
//    {
//        NeuronTree SS,SS_Color;
//        V_NeuronSWC seg_temp =  V_L0.seg.at(i);
//        V_NeuronSWC seg_temp_color=seg_temp;
//        seg_temp.reverse();
//        for(int i=0;i<seg_temp.row.size();i++)
//        {
//            seg_temp.row.at(i).type=3;
//            seg_temp.row.at(i).x+=2;
//            seg_temp.row.at(i).y+=2;
//            seg_temp.row.at(i).z+=2;
////            seg_temp_color.row.at(i).type=seg_temp_color.row.at(i).r/10;
//        }
//        SS = V_NeuronSWC__2__NeuronTree(seg_temp);
////        SS_Color=V_NeuronSWC__2__NeuronTree(seg_temp_color);

//        if (SS.listNeuron.size()>=2)
//        {
//            MergedNT.push_back(SS);
////            ColordNT.push_back(SS_Color);
//        }
//    }

//    V_NeuronSWC_list tosave;

//    tosave.clear();
//    for(int i=0;i<MergedNT.size();i++)
//    {
//        NeuronTree ss=MergedNT.at(i);
//        V_NeuronSWC ss_temp=NeuronTree__2__V_NeuronSWC_list(ss).seg.at(0);
//        tosave.seg.push_back(ss_temp);
//    }

//    NeuronTree wholeNT=V_NeuronSWC_list__2__NeuronTree(tosave);
//    writeESWC_file("result.swc",wholeNT);

//    qDebug()<<"sadjkhj";

    return a.exec();
}




