#ifndef ANALYSE_H
#define ANALYSE_H
#include <QFileInfo>
#include "basic_c_fun/basic_surf_objs.h"
#include "basic_c_fun/neuron_format_converter.h"
#include <map>
#include <QMap>
#include <QProcess>
#include <QSet>
#include <cstdlib>
#include <vector>
#include <QVector3D>
#include "neuron_editing/global_feature_compute.h"
uint blocksize=256;
double cuc_length(QString swcname)
{
    double length=0;
    double feature[22]={0};

    NeuronTree nt=readSWC_file(swcname);
    computeFeature(nt,feature);
    length=feature[10];
    //计算长度
//        p.execute(QString("C:/Users/Brain/Desktop/Vaa3D_V3.601_Windows_MSVC_64bit/vaa3d_msvc.exe /x C:/Users/Brain/Desktop/Vaa3D_V3.601_Windows_MSVC_64bit/plugins/neuron_utilities/global_neuron_feature/global_neuron_feature.dll /f compute_feature "
//                  "/i %1").arg(swcName+"_user"+QString::number(iter->first)+".ano.eswc"));
    return length;
}

QMap<int,double> retypeSWC_USER(NeuronTree nt,QString swcName)
{
    QSet<int> types;
    for(int i=0;i<nt.listNeuron.size();i++)
    {
        int type=int(nt.listNeuron[i].r)/10;
        nt.listNeuron[i].type=type;

        types.insert(type);
    }
    {
        NeuronTree nt1=nt;
        for(int i=0;i<nt.listNeuron.size();i++)
        {
            nt1.listNeuron[i].r=1;
        }
        writeESWC_file("./swc/"+swcName+"_user"+".ano.eswc",nt1);
    }

    QMap<int,double> map_type_distance;

    QList<int > ltypes=types.values();
    qDebug()<<ltypes;
    for(int j=0;j<ltypes.size();j++)
    {
        NeuronTree nt_t=nt;
        for(int i=0;i<nt_t.listNeuron.size();i++)
            if(nt_t.listNeuron[i].type!=ltypes[j])
                nt_t.listNeuron.removeAt(i--);
        writeESWC_file("./swc/"+swcName+"_user"+QString::number(ltypes[j])+".ano.eswc",nt_t);
        map_type_distance.insert(ltypes[j],cuc_length("./swc/"+swcName+"_user"+QString::number(ltypes[j])+".ano.eswc"));

    }
    qDebug()<<"hfkjehfekj====+++++++++++++++++++++";
    return map_type_distance;
}
void retypeSWC_MODE(NeuronTree nt,QString swcName)
{
    for(int i=0;i<nt.listNeuron.size();i++)
        nt.listNeuron[i].type=int(nt.listNeuron[i].r)%10;
    writeESWC_file(swcName+"_mode"+"ano.eswc",nt);
}

QMap<double,double> cuc_time(NeuronTree nt,QFileInfo swcName)
{

    int timeskip=180;

    QMap<double ,double > map_time_distance;
    QSet<int> timeset;
     for(int i=0;i<nt.listNeuron.size();i++)
     {
        timeset.insert(nt.listNeuron[i].timestamp);
     }

     QList<int> timelist=timeset.values();
     qSort(timelist.begin(),timelist.end());

     if(timelist.empty()) return map_time_distance;

     QMap<int,int> timeStart_Stop;
     int timeStart,timeStop;
     timeStart=timeStop=timelist.at(0);
     for(int i=1;i<timelist.size();i++)
     {
         if(timelist.at(i)-timeStop<=timeskip)
         {
             timeStop=timelist.at(i);
         }else
         {
             timeStart_Stop.insert(timeStart,timeStop);
             timeStart=timeStop=timelist.at(i);
         }
     }
     int timespent=0;
     auto iter=timeStart_Stop.begin();
     while(iter!=timeStart_Stop.end())
     {
         timespent+=(iter.value()-iter.key());
         iter++;
     }
     double length=cuc_length(swcName.absoluteFilePath());
     map_time_distance.insert(timespent,length);
     return map_time_distance;
}

QMap<QVector3D,QVector3D> cucNTBB(NeuronTree nt)
{
    qDebug()<<"hkjhewkrfheukyrfuoeyfuor";
    uint bias =5;
    QList<double> xlist,ylist,zlist;
    for(int i=0;i<nt.listNeuron.size();i++)
    {
        xlist.push_back(nt.listNeuron[i].x);
        ylist.push_back(nt.listNeuron[i].y);
        zlist.push_back(nt.listNeuron[i].z);
    }
    qSort(xlist.begin(),xlist.end());
    qSort(ylist.begin(),ylist.end());
    qSort(zlist.begin(),zlist.end());
    qDebug()<<"++++++++++++++++hkjhewkrfheukyrfuoeyfuor";
    int min_x,min_y,min_z;
    int max_x,max_y,max_z;

    min_x=xlist[0];min_y=ylist[0];min_z=zlist[0];
    max_x=xlist[xlist.size()-1]+1;max_y=ylist[ylist.size()-1]+1;max_z=zlist[zlist.size()-1]+1;
    if((max_x-min_x)%blocksize!=0)
    {
        max_x+=(blocksize-(max_x-min_x)%blocksize);
        max_x-=bias;
        min_x-=bias;
    }
    if((max_y-min_y)%blocksize!=0)
    {
        max_y+=(blocksize-(max_y-min_y)%blocksize);
        max_y-=bias;
        min_y-=bias;
    }
    if((max_z-min_z)%blocksize!=0)
    {
        max_z+=(blocksize-(max_z-min_z)%blocksize);
        max_z-=bias;min_z-=bias;
    }
    qDebug()<<"-----------------hkjhewkrfheukyrfuoeyfuor";
    QVector3D start,stop;
    start.setX(min_x);start.setY(min_y);start.setZ(min_z);
    stop.setX(max_x);stop.setY(max_y);stop.setZ(max_z);
    QMap<QVector3D,QVector3D> BB;
    BB.insert(start,stop);
    return BB;
}



int cuc_cnt(QMap<QVector3D,QVector3D> block,NeuronTree &nt)
{
    int cnt=0;
    int min_x,min_y,min_z;
    int max_x,max_y,max_z;
    min_x=block.keys().at(0).x();min_y=block.keys().at(0).y();min_z=block.keys().at(0).z();
    max_x=block.values().at(0).x();max_y=block.values().at(0).y();max_z=block.values().at(0).z();

    QSet<int> set;
    for(int i=0;i<nt.listNeuron.size();i++)
    {
//        qDebug()<<" x= "<<min_x<<" "<<nt.listNeuron[i].x<<" "<<max_x;
//        qDebug()<<" y= "<<min_y<<" "<<nt.listNeuron[i].y<<" "<<max_y;
//        qDebug()<<" z= "<<min_z<<" "<<nt.listNeuron[i].z<<" "<<max_z;
        if((min_x<=nt.listNeuron[i].x&&nt.listNeuron[i].x<=max_x)
          &&(min_y<=nt.listNeuron[i].y&&nt.listNeuron[i].y<=max_y)
          &&(min_z<=nt.listNeuron[i].z&&nt.listNeuron[i].z<=max_z)
                )
        {
//            qDebug()<<"in this bb:"<<min_x<<" "<<max_x<<" "<<min_y<<" "<<max_y<<" "<< min_z<<" "<<max_z;
            set.insert(nt.listNeuron.at(i).r/10);
        }
    }
    cnt=set.size();
    if(cnt!=0)
    for(int i=0;i<nt.listNeuron.size();i++)
    {
//        qDebug()<<" x= "<<min_x<<" "<<nt.listNeuron[i].x<<" "<<max_x;
//        qDebug()<<" y= "<<min_y<<" "<<nt.listNeuron[i].y<<" "<<max_y;
//        qDebug()<<" z= "<<min_z<<" "<<nt.listNeuron[i].z<<" "<<max_z;
        if((min_x<=nt.listNeuron[i].x&&nt.listNeuron[i].x<=max_x)
          &&(min_y<=nt.listNeuron[i].y&&nt.listNeuron[i].y<=max_y)
          &&(min_z<=nt.listNeuron[i].z&&nt.listNeuron[i].z<=max_z)
                )
        {
//            qDebug()<<"in this bb:"<<min_x<<" "<<max_x<<" "<<min_y<<" "<<max_y<<" "<< min_z<<" "<<max_z;
            nt.listNeuron[i].type=3+cnt;
        }
    }



    return cnt;
}

QMap<QVector3D,int> heatMap(NeuronTree &nt)
{
    QMap<QVector3D,QVector3D> BB=cucNTBB(nt);

//    QMap<QMap<QVector3D,QVector3D>,int> map_BB_CNT;
    QMap<QVector3D,int> map_BB_CNT;
    qDebug()<<BB.values().at(0).x()<<" "<<BB.keys().at(0).x();
    qDebug()<<BB.values().at(0).y()<<" "<<BB.keys().at(0).y();
    qDebug()<<BB.values().at(0).z()<<" "<<BB.keys().at(0).z();
    const int k1=((BB.values().at(0).x())-(BB.keys().at(0).x()))/blocksize;
    const int k2=((BB.values().at(0).y())-(BB.keys().at(0).y()))/blocksize;
    const int k3=((BB.values().at(0).z())-(BB.keys().at(0).z()))/blocksize;
    for(int i1=1;i1<=k1;i1++)
    {
        for(int i2=1;i2<=k2;i2++)
        {
            for(int i3=1;i3<=k3;i3++)
            {
//
                QVector3D start;
                start.setX((BB.keys().at(0).x())+(i1-1)*blocksize);
                start.setY((BB.keys().at(0).y())+(i2-1)*blocksize);
                start.setZ((BB.keys().at(0).z())+(i3-1)*blocksize);
                QVector3D stop;
                stop.setX((BB.keys().at(0).x())+(i1)*blocksize);
                stop.setY((BB.keys().at(0).y())+(i2)*blocksize);
                stop.setZ((BB.keys().at(0).z())+(i3)*blocksize);
                QMap<QVector3D,QVector3D> BB_child;
                BB_child.insert(start,stop);
                int num=cuc_cnt(BB_child,nt);
//                if(num!=0)
//                {
//                    QVector3D __t(i1,i2,i3);
//                    map_BB_CNT.insert(__t,cuc_cnt(BB_child,nt));
//                    qDebug()<<"cuc_cnt(BB_child,nt)="<<num;
//                    map_BB_CNT.insert(BB_child,cuc_cnt(BB_child,nt));
//                }
            }
        }
    }
    return map_BB_CNT;
}

void analyseSWC(QFileInfo swc)
{
    qDebug()<<swc.absoluteFilePath();
    NeuronTree nt=readSWC_file(swc.absoluteFilePath());
    QString swcName=swc.fileName();
    swcName.chop(9);//.ano.eswc


//    QMap<int,double> map_type_distance=retypeSWC_USER(nt,swcName);
//    QFile f1("./analysetype_distance/"+swc.baseName()+".txt");
//    if(f1.open(QIODevice::WriteOnly|QIODevice::Text))
//    {
//        QTextStream txtOutput(&f1);
//        txtOutput<<swc.fileName()<<endl;
//        txtOutput<<"type:"<<swc.baseName()<<endl;
//        auto iter1=map_type_distance.begin();
//        while(iter1!=map_type_distance.end())
//        {
//            txtOutput<<iter1.key()<<":"<<iter1.value()<<endl;
//            iter1++;
//        }
//        f1.close();
//    }


//    QMap<double,double> map_time_distance=cuc_time(nt,swc);
//    QFile f2("./analysetime_distance/"+swc.baseName()+".txt");
//    if(f2.open(QIODevice::WriteOnly|QIODevice::Text))
//    {
//        QTextStream txtOutput(&f2);
//        txtOutput<<swc.fileName()<<endl;
//        txtOutput<<"time:"<<swc.baseName()<<endl;
//        auto iter2=map_time_distance.begin();
//        if(iter2!=map_time_distance.end())
//            txtOutput<<1.0*iter2.key()/3600<<":"<<iter2.value()/1000<<endl;
//        f2.close();
//    }

//    QMap<QMap<QVector3D,QVector3D>,int> map_bb_cnt=heatMap(nt);
    QMap<QVector3D,int> map_bb_cnt=heatMap(nt);
    writeESWC_file("./analyseBB_count/"+swc.baseName()+"_cnt.ano.eswc",nt);
//    QFile f3("./analyseBB_count/"+swc.baseName()+".txt");
//    if(f3.open(QIODevice::WriteOnly|QIODevice::Text))
//    {
//        QTextStream txtOutput(&f3);
//        txtOutput<<swc.fileName()<<endl;
//        txtOutput<<"BB:userCount"<<endl;
//        auto iter3=map_bb_cnt.begin();
//        while(iter3!=map_bb_cnt.end())
//        {
////            QMap<QVector3D,QVector3D> bb=iter3.key();
//            QVector3D bb=iter3.key();
////            auto iter4=bb.begin();
////            txtOutput<<iter4.key().x()<<" "<<iter4.key().y()<<" "<<iter4.key().z()
////                    <<" "<<iter4.value().x()<<" "<<iter4.value().y()<<" "<<iter4.value().z()
////                    <<":"<<iter3.value()<<endl;
//            txtOutput<<bb.x()<<":"<<bb.y()<<":" <<bb.z()<<":"<<iter3.value()<<endl;
//            iter3++;
//        }
//        f3.close();
//    }

//    QFile f("./analyse/"+swc.baseName()+".txt");
//    if(f.open(QIODevice::WriteOnly|QIODevice::Text))
//    {
//        QTextStream txtOutput(&f);
//        txtOutput<<swc.fileName()<<endl;
//        txtOutput<<"type:typedistance"<<endl;


//        auto iter1=map_type_distance.begin();
//        while(iter1!=map_type_distance.end())
//        {
//            txtOutput<<iter1.key()<<":"<<iter1.value()<<endl;
//            iter1++;
//        }
//        txtOutput<<"time:distance"<<endl;
//        auto iter2=map_time_distance.begin();
//        if(iter2!=map_time_distance.end())
//            txtOutput<<iter2.key()<<":"<<iter2.value()<<endl;


//        txtOutput<<"BB:userCount"<<endl;
//        auto iter3=map_bb_cnt.begin();
//        while(iter3!=map_bb_cnt.end())
//        {
//            QMap<QVector3D,QVector3D> bb=iter3.key();
//            auto iter4=bb.begin();
//            txtOutput<<iter4.key().x()<<" "<<iter4.key().y()<<" "<<iter4.key().z()
//                    <<" "<<iter4.value().x()<<" "<<iter4.value().y()<<" "<<iter4.value().z()
//                    <<":"<<iter3.value()<<endl;
//            iter3++;
//        }
//        f.close();
//    }

}

#endif // ANALYSE_H
