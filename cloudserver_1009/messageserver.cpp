#include "messageserver.h"
#include <QDataStream>
#include <QByteArray>
#include <QtGlobal>
#include "QTextStream"
#include "basic_c_fun/v_neuronswc.h"

const double dist_thres=0.05;

MessageServer::MessageServer(QString filename,Global_Parameters *parameters,QObject *parent)
    :global_parameters(parameters),QTcpServer (parent),filename(filename),sketchNum(0)
{
    qDebug()<<"make a message server";

    V_NeuronSWC_list testVNL= NeuronTree__2__V_NeuronSWC_list(global_parameters->wholeNT);
    for(int i=0;i<testVNL.seg.size();i++)
    {
        NeuronTree SS;
        V_NeuronSWC seg_temp =  testVNL.seg.at(i);
        seg_temp.reverse();
        SS = V_NeuronSWC__2__NeuronTree(seg_temp);
        SS.name = "loaded_" + QString("%1").arg(i);
        if (SS.listNeuron.size()>0)
            sketchedNTList.push_back(SS);
    }
    sketchNum=sketchedNTList.size();
    m_globalScale=10000;//need modified

//    connect(global_parameters->timer,SIGNAL(timeout()),this,SLOT(autoSave()));
    global_parameters->timer->start(6*1000);
}

void MessageServer::incomingConnection(int socketDesc)
{
    qDebug()<<"a new connection";
    MessageSocket *messagesocket=new MessageSocket(socketDesc,global_parameters);

    QThread *thread=new QThread;

    messagesocket->moveToThread(thread);

    connect(thread,SIGNAL(started()),messagesocket,SLOT(MessageSocketSlot_start()));
    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_disconnected()),
            messagesocket,SLOT(deleteLater()));
    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_disconnected()),
            thread,SLOT(deleteLater()));
    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_disconnected()),
            this,SLOT(MessageServerSlotAnswerMessageSocket_disconnected()));
    connect(messagesocket,
            SIGNAL(MessageSocketSignalToMessageServer_sendtoall(const QString &)),
            this,SLOT(MessageServerSlotAnswerMessageSocket_sendtoall(const QString &)));
    connect(this,SIGNAL(MessageServerSignal_sendtoall(const QString &)),
            messagesocket,
            SLOT(MessageSocketSlotAnswerToMessageServer_sendtoall(const QString &)));

    connect(messagesocket,SIGNAL(signal_addseg(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_addseg(QString)));
    connect(messagesocket,SIGNAL(signal_delseg(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_delseg(QString)));
    connect(messagesocket,SIGNAL(signal_addmarker(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_addmarker(QString)));
    connect(messagesocket,SIGNAL(signal_delmarker(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_delmarker(QString)));


    thread->start();

}

void MessageServer::MessageServerSlotAnswerMessageSocket_sendtoall(const QString &msg)
{
    emit MessageServerSignal_sendtoall(msg);

}

void MessageServer::MessageServerSlotAnswerMessageSocket_disconnected()
{
    global_parameters->lock_clientNum.lockForWrite();
    if(--global_parameters->clientNum==0)
    {

        QRegExp fileExp("(.*)_stamp_(.*).ano");
        if(fileExp.indexIn(filename)!=-1)
        {
            qDebug()<<"in disconnected.++++";
            QDateTime time=QDateTime::currentDateTime();
            QString strtime=time.toString("yyyy_MM_dd_hh_mm_ss");

            QString tempname =fileExp.cap(1)+"_stamp_"+strtime;
                qDebug()<<tempname<<"here 100";

            QFile anofile("./clouddata/"+tempname+".ano");
            anofile.open(QIODevice::WriteOnly);
            QString str1="APOFILE="+tempname+".ano.apo";
            QString str2="SWCFILE="+tempname+".ano.eswc";

            QTextStream out(&anofile);
            out<<str1<<endl<<str2;
            anofile.close();
//            global_parameters->wholeNT=V_NeuronSWC_list__2__NeuronTree(sketchedNTList);

            {
                V_NeuronSWC_list tosave;
                tosave.clear();
                for(int i=0;i<sketchedNTList.size();i++)
                {
                    NeuronTree ss=sketchedNTList.at(i);
                    V_NeuronSWC ss_temp=NeuronTree__2__V_NeuronSWC_list(ss).seg.at(0);
                    tosave.seg.push_back(ss_temp);
                }
                global_parameters->wholeNT=V_NeuronSWC_list__2__NeuronTree(tosave);
            }

            global_parameters->lock_wholeNT.lockForWrite();
            writeESWC_file("./clouddata/"+tempname+".ano.eswc",global_parameters->wholeNT);
            global_parameters->lock_wholeNT.unlock();
            global_parameters->lock_wholePoint.lockForRead();
            writeAPO_file("./clouddata/"+tempname+".ano.apo",global_parameters->wholePoint);
            global_parameters->lock_wholePoint.unlock();
        }

        global_parameters->lock_clientNum.unlock();
        emit MessageServerDeleted(filename);
        delete global_parameters;
        this->deleteLater();
        qDebug()<<"save successfully";
        return;
    }
    global_parameters->lock_clientNum.unlock();

}

void MessageServer::MessageServerSlotAnswerMessageSocket_addseg(QString MSG)
{
    qDebug()<<"MessageServerSlotAnswerMessageSocket_addseg";
    /*MSG=QString("/seg:"+user + " " + msg)*/
    QRegExp Reg("/seg:(.*) (.*)");
    QString seg;
    QString username;

    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1);
        qDebug()<< username;
        seg=Reg.cap(2);
    }

    global_parameters->lock_clientsproperty.lockForRead();
    int colortype=21;
    for(int i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty.at(i).name==username)
        {
            colortype=global_parameters->clientsproperty.at(i).colortype;
            qDebug()<<username<<":"<<colortype;
            break;
        }
    }
    global_parameters->lock_clientsproperty.unlock();

    QStringList qsl=QString(seg).trimmed().split(" ",QString::SkipEmptyParts);
    int str_size = qsl.size()-(qsl.size()%7);

    NeuronSWC S_temp;
    NeuronTree newTempNT;
    newTempNT.listNeuron.clear();
    newTempNT.hashNeuron.clear();
    newTempNT.name  = "sketch_"+ QString("%1").arg(sketchNum++);
    for(int i=0;i<str_size;i++)
    {
        qsl[i].truncate(99);
        //qDebug()<<qsl[i];
        int iy = i%7;
        if (iy==0)
        {
            S_temp.n = qsl[i].toInt();
        }
        else if (iy==1)
        {
            S_temp.type = colortype;
        }
        else if (iy==2)
        {
            S_temp.x = qsl[i].toFloat();

        }
        else if (iy==3)
        {
            S_temp.y = qsl[i].toFloat();

        }
        else if (iy==4)
        {
            S_temp.z = qsl[i].toFloat();

        }
        else if (iy==5)
        {
            S_temp.r = qsl[i].toFloat();

        }
        else if (iy==6)
        {
            S_temp.pn = qsl[i].toInt();
//            XYZ tempxyz = ConvertGlobaltoLocalCoords(S_temp.x,S_temp.y,S_temp.z);//
            //全局坐标转换？？？？
            newTempNT.listNeuron.append(S_temp);
            newTempNT.hashNeuron.insert(S_temp.n,newTempNT.listNeuron.size()-1);
   //         global_parameters->wholeNT.listNeuron.append(S_temp);
      //      global_parameters->wholeNT.hashNeuron.insert(S_temp.n, global_parameters->wholeNT.listNeuron.size()-1);
        }
    }
    sketchedNTList.push_back(newTempNT);
    global_parameters->messageUsedIndex++;
}

void MessageServer::MessageServerSlotAnswerMessageSocket_delseg(QString MSG)
{
    qDebug()<<"MessageServerSlotAnswerMessageSocket_delseg";
    /*MSG=QString("/del_curve:"+user + " " + msg)*/
    QRegExp Reg("/del_curve:(.*) (.*)");
    QString delseg;
    QString username;
    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1);
        delseg=Reg.cap(2);
    }

    global_parameters->lock_clientsproperty.lockForRead();
    int colortype=21;
    for(int i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty.at(i).name==username)
        {
            colortype=global_parameters->clientsproperty.at(i).colortype;
            qDebug()<<username<<":"<<colortype;
            break;
        }
    }
    global_parameters->lock_clientsproperty.unlock();

    QStringList delMSGs = delseg.split(" ");
    if(delMSGs.size()<2)
    {
            qDebug()<<"size < 2";
            return;
    }
    QString user = delMSGs.at(0);//why have username;
    float dx = delMSGs.at(1).toFloat();
    float dy = delMSGs.at(2).toFloat();
    float dz = delMSGs.at(3).toFloat();//global or local?? ->ask liqi
    float resx = delMSGs.at(4).toFloat();
    float resy = delMSGs.at(5).toFloat();
    float resz = delMSGs.at(6).toFloat();

    QString delID="";

    /*find nearest segment*/
    if(sketchedNTList.size()>=1)
    {
        for(int i=0;i<sketchedNTList.size();i++)
        {
            NeuronTree nt=sketchedNTList.at(i);
            for(int j=0;j<nt.listNeuron.size();j++)
            {
                NeuronSWC ss0;
                ss0=nt.listNeuron.at(j);
                float dist;
                dist=sqrt((dx-ss0.x)*(dx-ss0.x)+(dy-ss0.y)*(dy-ss0.y)
                          +(dz-ss0.z)*(dz-ss0.z));

                if(dist<dist_thres/m_globalScale)
                {
                    delID=nt.name;
                    goto L;
                }
            }
        }
    }

    L:  if(delID=="")
        {
            qDebug()<<"cannot find segID";
            return;
        }
    for(int i=0;i<sketchedNTList.size();i++)
    {
        QString NTname="";
        NTname = sketchedNTList.at(i).name;
        if(delID==NTname)
        {
            sketchedNTList.removeAt(i);
            qDebug()<<"delete segment success";
        }
    }


}

void MessageServer::MessageServerSlotAnswerMessageSocket_addmarker(QString MSG)
{
    qDebug()<<"MessageServerSlotAnswerMessageSocket_addmarker";
    /*MSG=QString("/marker:" +user+" "+markermsg)*/
    QRegExp Reg("/marker:(.*) (.*)");
    QString markerpos;
    QString username;
    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1);
        markerpos=Reg.cap(2);
    }
    global_parameters->lock_clientsproperty.lockForRead();
    int colortype=21;
    for(int i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty.at(i).name==username)
        {
            colortype=global_parameters->clientsproperty.at(i).colortype;
            qDebug()<<username<<":"<<colortype;
            break;
        }
    }
    global_parameters->lock_clientsproperty.unlock();

    QStringList markerMSGs=markerpos.trimmed().split(" ");
    if(markerMSGs.size()<4) return;
    QString user = markerMSGs.at(0);
    float mx = markerMSGs.at(1).toFloat();
    float my = markerMSGs.at(2).toFloat();
    float mz = markerMSGs.at(3).toFloat();
    int resx = markerMSGs.at(4).toFloat();
    int resy = markerMSGs.at(5).toFloat();
    int resz = markerMSGs.at(6).toFloat();

//  for(int i=0;i<global_parameters->wholePoint.size();i++)
//  {

//  }

    //不判断附近的点，直接add
    CellAPO marker0;
    marker0.x=mx;marker0.y=my;marker0.z=mz;

    marker0.color.r=255;
    marker0.color.g=0;
    marker0.color.b=0;
    marker0.n=global_parameters->wholePoint.size();//need do something

    marker0.volsize=314.159;
    marker0.orderinfo="";
    marker0.name="";
    marker0.comment="";
    global_parameters->wholePoint.push_back(marker0);

}
void MessageServer::MessageServerSlotAnswerMessageSocket_delmarker(QString MSG)
{
    qDebug()<<"MessageServerSlotAnswerMessageSocket_delmarker";
    /*MSG=QString("/del_marker:" +user+" "+delmarkerpos )*/
    QRegExp Reg("/del_marker:(.*) (.*)");
    QString delmarkerpos;
    QString username;
    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1);
        delmarkerpos=Reg.cap(2);
    }

//    global_parameters->lock_clientsproperty.lockForRead();
//    int colortype=21;
//    for(int i=0;i<global_parameters->clientsproperty.size();i++)
//    {
//        if(global_parameters->clientsproperty.at(i).name==username)
//        {
//            colortype=global_parameters->clientsproperty.at(i).colortype;
//            break;
//        }
//    }
//    global_parameters->lock_clientsproperty.unlock();
    QStringList delmarkerPOS = delmarkerpos.trimmed().split(" ");
    if(delmarkerPOS.size()<4)
    {
            qDebug()<<"size < 4";
            return;
    }
    QString user = delmarkerPOS.at(0);
    float mx = delmarkerPOS.at(1).toFloat();
    float my = delmarkerPOS.at(2).toFloat();
    float mz = delmarkerPOS.at(3).toFloat();

    for(int i=0;i<global_parameters->wholePoint.size();i++)
    {
        CellAPO marker0=global_parameters->wholePoint.at(i);
        double dist=sqrt((mx-marker0.x)*(mx-marker0.x)+
                         (my-marker0.y)*(my-marker0.y)+(mz-marker0.z)*(mz-marker0.z));
        if(dist<dist_thres/m_globalScale)
        {
            global_parameters->wholePoint.removeAt(i);
            break;
        }
    }

}


void MessageServer::autoSave()
{
    qDebug()<<"autosave";
    QDir rDir("./");
    if(!rDir.cd("autosave"))
    {
        rDir.mkdir("autosave");
    }
    QRegExp fileExp("(.*)_stamp_(.*).ano");
    if(fileExp.indexIn(filename)!=-1)
    {
        QDateTime time=QDateTime::currentDateTime();
        QString strtime=time.toString("yyyy_MM_dd_hh_mm_ss");

        QString tempname =fileExp.cap(1)+"_stamp_autosave_"+strtime;
            qDebug()<<tempname<<"here 100";

        QFile anofile("./autosave/"+tempname+".ano");
        QString str1="APOFILE="+tempname+".ano.apo";
        QString str2="SWCFILE="+tempname+".ano.eswc";

        QTextStream out(&anofile);
        out<<str1<<endl<<str2;
        anofile.close();

        {
            V_NeuronSWC_list tosave;
            tosave.clear();
            for(int i=0;i<sketchedNTList.size();i++)
            {
                NeuronTree ss=sketchedNTList.at(i);
                V_NeuronSWC ss_temp=NeuronTree__2__V_NeuronSWC_list(ss).seg.at(0);
                tosave.seg.push_back(ss_temp);
            }
            global_parameters->wholeNT=V_NeuronSWC_list__2__NeuronTree(tosave);
        }

//        global_parameters->lock_wholeNT.lockForWrite();
        writeESWC_file("./autosave/"+tempname+".ano.eswc",global_parameters->wholeNT);
//        global_parameters->lock_wholeNT.unlock();
//        global_parameters->lock_wholePoint.lockForRead();
        writeAPO_file("./autosave/"+tempname+".ano.apo",global_parameters->wholePoint);
//        global_parameters->lock_wholePoint.unlock();
    }
    global_parameters->timer->start(5*60*1000);
}



