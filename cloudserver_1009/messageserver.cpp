#include "messageserver.h"
#include <QDataStream>
#include <QByteArray>
#include <QtGlobal>
#include "QTextStream"
#include "basic_c_fun/v_neuronswc.h"
#include <QDir>
#include <QTextStream>
#include <QString>
#include <QMap>

const double dist_thres=0.05;

void MessageServer::userLoad(ForAUTOSave foraotosave)
{
    QString ip=foraotosave.ip;
    FileServer_send  *fileserver_send=foraotosave.fileserver_send;
    ManageSocket *managesocket=foraotosave.managesocket;
    QMap <QString ,MessageServer* > *Map_File_MessageServer=foraotosave.Map_File_MessageServer;
    QString messageport=foraotosave.messageport;
    QString anofile_name=foraotosave.anofile_name;

    QMap<quint32 ,QString> map=this->autoSave();
    this->global_parameters->Map_Ip_NumMessage[managesocket->peerAddress().toString()]=map.keys().at(0);
    fileserver_send->sendFile(ip,map.values().at(0));
    managesocket->write(QString(messageport+":messageport"+".\n").toUtf8());
    (*Map_File_MessageServer)[anofile_name]=this;

}

MessageServer::MessageServer(QString filename,Global_Parameters *parameters,QObject *parent)
    :global_parameters(parameters),QTcpServer (parent),filename(filename),sketchNum(0)
{
//    qDebug()<<"make a message server";

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

    connect(global_parameters->timer,SIGNAL(timeout()),this,SLOT(autoSave()));
    //autoSave();
    global_parameters->timer->start(3*60*1000);
}

void MessageServer::incomingConnection(int socketDesc)
{
//    qDebug()<<"a new connection";
    MessageSocket *messagesocket=new MessageSocket(socketDesc,global_parameters);


    QThread *thread=new QThread;

    messagesocket->moveToThread(thread);
    connect(thread,SIGNAL(started()),messagesocket,SLOT(MessageSocketSlot_start()));
    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_disconnected()),
            this,SLOT(MessageServerSlotAnswerMessageSocket_disconnected()));

    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_disconnected()),
            thread,SLOT(quit()));
    connect(thread,SIGNAL(finished()),messagesocket,SLOT(deleteLater()));
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));

    connect(messagesocket,SIGNAL(MessageSocketSignalToMessageServer_sendtoall(const QString &)),
            this,SLOT(MessageServerSlotAnswerMessageSocket_sendtoall(const QString &)));
    connect(this,SIGNAL(MessageServerSignal_sendtoall(const QString &)),messagesocket,
            SLOT(MessageSocketSlotAnswerToMessageServer_sendtoall(const QString &)));

    connect(messagesocket,SIGNAL(signal_addseg(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_addseg(QString)));
    connect(messagesocket,SIGNAL(signal_delseg(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_delseg(QString)));
    connect(messagesocket,SIGNAL(signal_addmarker(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_addmarker(QString)));
    connect(messagesocket,SIGNAL(signal_delmarker(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_delmarker(QString)));
    connect(messagesocket,SIGNAL(signal_retype(QString)),this,SLOT(MessageServerSlotAnswerMessageSocket_retype(QString)));

    thread->start();
}

void MessageServer::MessageServerSlotAnswerMessageSocket_retype( QString MSG)
{
    global_parameters->messageUsedIndex++;
    qDebug()<<"messageindex="<<global_parameters->messageUsedIndex;
        orderList.push_back(QDateTime::currentDateTimeUtc().toString("yyyy/MM/dd hh:mm:ss ")+MSG);
    QRegExp Reg("/retype:(.*)__(.*)");
    QString delseg;
    QString username;
    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1).trimmed();
        delseg=Reg.cap(2).trimmed();
    }
     QStringList delMSGs = delseg.split("_",QString::SkipEmptyParts);
     qDebug()<<"delMSGs:"<<delMSGs;
     if(delMSGs.size()<1) return;

     for(int i=0;i<delMSGs.size();i++)
     {
         QString tempNode=delMSGs.at(i);
         QStringList tempNodeList=tempNode.split(" ",QString::SkipEmptyParts);

         if(tempNodeList.size()<4) return ;
         float x=tempNodeList.at(0).toFloat();
         float y=tempNodeList.at(1).toFloat();
         float z=tempNodeList.at(2).toFloat();
         int type=tempNodeList.at(3).toInt();


         for (int j=0;j<sketchedNTList.size();j++)
         {
//             qDebug()<<"in Sssds";
             NeuronTree NT=sketchedNTList.at(j);
             NeuronSWC ss=NT.listNeuron.at(NT.listNeuron.size()-2);
             NeuronSWC ss0=NT.listNeuron.at(1);

             if(sqrt(pow(ss.x-x,2)+pow(ss.y-y,2)+pow(ss.z-z,2))<=0.01||sqrt(pow(ss0.x-x,2)+pow(ss0.y-y,2)+pow(ss0.z-z,2))<=0.01)
             {
//                 qDebug()<<"in reypr";
                 sketchedNTList.removeAt(j);
                 NeuronTree newTempNT;
                 newTempNT.listNeuron.clear();
                 newTempNT.hashNeuron.clear();

                 newTempNT.name=NT.name;
                 for(int k=0;k<NT.listNeuron.size();k++)
                 {
                     NeuronSWC S_temp;
                     S_temp.n=NT.listNeuron.at(k).n;
                     S_temp.type=type;
                     S_temp.x=NT.listNeuron.at(k).x;
                     S_temp.y=NT.listNeuron.at(k).y;
                     S_temp.z=NT.listNeuron.at(k).z;
                     S_temp.r=NT.listNeuron.at(k).r;

                         S_temp.pn=NT.listNeuron.at(k).pn;
                     S_temp.level=NT.listNeuron.at(k).level;
                     S_temp.creatmode=NT.listNeuron.at(k).creatmode;
                     S_temp.timestamp=NT.listNeuron.at(k).timestamp;
                     S_temp.tfresindex=NT.listNeuron.at(k).tfresindex;
                     newTempNT.listNeuron.append(S_temp);
                     newTempNT.hashNeuron.insert(S_temp.n,newTempNT.listNeuron.size()-1);
                 }
                 sketchedNTList.push_front(newTempNT);break;

             }
         }
     }
//     global_parameters->messageUsedIndex++;
     qDebug()<<"retype end";
}

void MessageServer::MessageServerSlotAnswerMessageSocket_sendtoall(const QString &msg)
{
    emit MessageServerSignal_sendtoall(msg);
}

void MessageServer::MessageServerSlotAnswerMessageSocket_disconnected()
{
//    qDebug()<<"socket disconnected ,userNUM--";

        QRegExp fileExp("(.*).ano");
        if(fileExp.indexIn(filename)!=-1)
        {
            QString tempname ="./clouddata/"+fileExp.cap(1)+".ano";
            QFile *f=new QFile(tempname);
            if(f->exists())
            {
                f->remove();
                delete f;
                f=new QFile(tempname+".eswc");
                if(f->exists())
                {
                    f->remove();
                    delete f;
                    f=new QFile(tempname+".apo");
                    if(f->exists())
                    {
                        f->remove();
                        delete f;
                    }
                }

            }
            QFile anofile(tempname);
            anofile.open(QIODevice::WriteOnly);
            QString str1="APOFILE="+fileExp.cap(1)+".ano.apo";
            QString str2="SWCFILE="+fileExp.cap(1)+".ano.eswc";

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
            writeESWC_file("./clouddata/"+fileExp.cap(1)+".ano.eswc",global_parameters->wholeNT);
            writeAPO_file("./clouddata/"+fileExp.cap(1)+".ano.apo",global_parameters->wholePoint);
        }

        //write log
        if(!QDir("./orderfile").exists()) QDir("./").mkdir("orderfile");
        QFile f("./orderfile/"+filename+".txt");

        if(f.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))
        {
            QTextStream txtoutput(&f);

            while (!orderList.isEmpty()) {

                txtoutput<<orderList.at(0)<<endl;
                orderList.removeFirst();
            }
            txtoutput<<endl;
        }
        f.close();

        if(!QDir("./removelog").exists()) QDir("./").mkdir("removelog");

        V_NeuronSWC_list testVNL;
        if(QFile("./removelog/"+filename+".swc").exists())
        {
            NeuronTree remove_stroed=readSWC_file("./removelog/"+filename+".swc");
            testVNL= NeuronTree__2__V_NeuronSWC_list(remove_stroed);
        }

        while(!removedNTList.isEmpty())
        {
            NeuronTree NT=removedNTList.at(0).NT;
            V_NeuronSWC seg=NeuronTree__2__V_NeuronSWC_list(NT).seg.at(0);
            for(int j=0;j<seg.row.size();j++)
            {
                seg.row.at(j).timestamp=removedNTList.at(0).time;
                seg.row.at(j).r+=removedNTList.at(0).id*100;
            }
            testVNL.seg.push_back(seg);
            removedNTList.removeAt(0);
        }

        NeuronTree tmp=V_NeuronSWC_list__2__NeuronTree(testVNL);
        writeESWC_file("./removelog/"+filename+".swc",tmp);
        QFile userInfoFile("./userInfo/userInfo.txt");
    if(userInfoFile.open(QIODevice::Truncate|QIODevice::Text|QIODevice::WriteOnly))
        {
            QTextStream __stream(&userInfoFile);
            global_parameters->lock_userInfo.lockForRead();
            for(quint64 i=0;i<global_parameters->userInfo->size();i++)
            {
                QString _=global_parameters->userInfo->keys().at(i)+" "+QString::number(global_parameters->userInfo->value(global_parameters->userInfo->keys().at(i)));
                __stream<<_<<endl;
            }
            global_parameters->lock_userInfo.unlock();
        }
        userInfoFile.close();
        emit MessageServerDeleted(filename);
        delete global_parameters;

        this->deleteLater();
        qDebug()<<"save successfully";
        return;
}

void MessageServer::MessageServerSlotAnswerMessageSocket_addseg(QString MSG)
{
//    qDebug()<<"MessageServerSlotAnswerMessageSocket_addseg";
//    qDebug()<<QDateTime::currentDateTimeUtc().toString("yyyy/MM/dd hh:mm:ss ")+MSG;
    global_parameters->messageUsedIndex++;
    qDebug()<<"messageindex="<<global_parameters->messageUsedIndex;
    orderList.push_back(QDateTime::currentDateTimeUtc().toString("yyyy/MM/dd hh:mm:ss ")+MSG);
    QRegExp Reg("/seg:(.*)__(.*)");
    QString seg;
    QString username;

    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1);
        qDebug()<< username;
        seg=Reg.cap(2).trimmed();
    }

    global_parameters->lock_clientsproperty.lockForRead();
    int colortype=21;
    for(int i=0;i<global_parameters->clientsproperty.size();i++)
    {
        if(global_parameters->clientsproperty.at(i).name==username)
        {
            colortype=global_parameters->clientsproperty.at(i).colortype;
//            qDebug()<<username<<":"<<colortype;
            break;
        }
    }
    global_parameters->lock_clientsproperty.unlock();

    QStringList qsl=QString(seg).trimmed().split("_",QString::SkipEmptyParts);
    if(qsl.size()<=1) return;
    NeuronTree newTempNT;
    newTempNT.listNeuron.clear();
    newTempNT.hashNeuron.clear();
    newTempNT.name  = "sketch_"+ QString("%1").arg(sketchNum++);

    QString head=qsl[0];

    for(int i=1;i<qsl.size();i++)
    {
        NeuronSWC S_temp;
        QStringList temp=qsl[i].trimmed().split(" ");

        if(head.trimmed().split(" ").at(0)=="TeraFly")
        {
            S_temp.n=i;
            S_temp.type=temp[1].toInt();
            S_temp.x=temp[2].toFloat();
            S_temp.y=temp[3].toFloat();
            S_temp.z=temp[4].toFloat();
            S_temp.r=username.toInt()*10+1;
            if(i==1)
                S_temp.pn=-1;
            else
                S_temp.pn=i-1;
            S_temp.level=0;
            S_temp.creatmode=0;
            S_temp.timestamp=time(NULL);
            S_temp.tfresindex=0;
            if(i==qsl.size()-1)
            {
                MessageServerSlotAnswerMessageSocket_addmarker(QString("/marker:%1__%2 %3 %4")
                  .arg(username).arg(temp[2].toFloat()).arg(temp[3].toFloat()).arg(temp[4].toFloat()),1);
            }
            if(i==1)
            {
                MessageServerSlotAnswerMessageSocket_delmarker(QString("/del_marker:%1__%2 %3 %4")
                  .arg(username).arg(temp[2].toFloat()).arg(temp[3].toFloat()).arg(temp[4].toFloat()),1);
            }

        }else if(head.trimmed().split(" ").at(0)=="TeraVR")
        {
            S_temp.n=temp[0].toLongLong();
            S_temp.type=temp[1].toInt();
            S_temp.x=temp[2].toFloat();
            S_temp.y=temp[3].toFloat();
            S_temp.z=temp[4].toFloat();

            S_temp.r=username.toInt()*10+2;
            S_temp.pn=temp[6].toLongLong();
            S_temp.level=0;
            S_temp.creatmode=0;
            S_temp.timestamp=time(NULL);
            S_temp.tfresindex=0;
            if(i==qsl.size()-1)
            {
                MessageServerSlotAnswerMessageSocket_addmarker(QString("/marker:%1__%2 %3 %4")
                  .arg(username).arg(temp[2].toFloat()).arg(temp[3].toFloat()).arg(temp[4].toFloat()),1);
            }
            if(i==1)
            {
                MessageServerSlotAnswerMessageSocket_delmarker(QString("/del_marker:%1__%2 %3 %4")
                  .arg(username).arg(temp[2].toFloat()).arg(temp[3].toFloat()).arg(temp[4].toFloat()),1);
            }
        }else if(head.trimmed().split(" ").at(0)=="TeraAI")
        {
            S_temp.n=i;
            S_temp.type=18;
            S_temp.x=temp[2].toFloat();
            S_temp.y=temp[3].toFloat();
            S_temp.z=temp[4].toFloat();
            S_temp.r=username.toInt()*10+3;
            if(i==1)
                S_temp.pn=-1;
            else
                S_temp.pn=i-1;
            S_temp.level=0;
            S_temp.creatmode=0;
            S_temp.timestamp=time(NULL);
            S_temp.tfresindex=0;
        }

        newTempNT.listNeuron.append(S_temp);
        newTempNT.hashNeuron.insert(S_temp.n,newTempNT.listNeuron.size()-1);
    }

    sketchedNTList.push_back(newTempNT);
//    global_parameters->messageUsedIndex++;
    qDebug()<<"add seg end==========================================";
}

void MessageServer::MessageServerSlotAnswerMessageSocket_delseg(QString MSG)
{
//    qDebug()<<"MessageServerSlotAnswerMessageSocket_delseg\n"<<MSG;
    /*MSG=QString("/del_curve:"+user + "__" + msg)*/
    global_parameters->messageUsedIndex++;
    qDebug()<<"messageindex="<<global_parameters->messageUsedIndex;
        orderList.push_back(QDateTime::currentDateTimeUtc().toString("yyyy/MM/dd hh:mm:ss ")+MSG);
    QRegExp Reg("/del_curve:(.*)__(.*)"); //msg=node 1_node 2_....
    QString delseg;
    QString username;
    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1).trimmed();
        delseg=Reg.cap(2).trimmed();
    }

    QStringList delMSGs = delseg.split("_",QString::SkipEmptyParts);

    if(delMSGs.size()<2) return;

    for(int i=1;i<delMSGs.size();i++)
    {
        QString tempNode=delMSGs.at(i);
        QStringList tempNodeList=tempNode.split(" ",QString::SkipEmptyParts);

        if(tempNodeList.size()<3) return ;
        float x=tempNodeList.at(0).toFloat();
        float y=tempNodeList.at(1).toFloat();
        float z=tempNodeList.at(2).toFloat();

        for (int j=0;j<sketchedNTList.size();j++)
        {
            NeuronTree NT=sketchedNTList.at(j);
            NeuronSWC ss=NT.listNeuron.at(NT.listNeuron.size()-2);
            NeuronSWC ss0=NT.listNeuron.at(1);

            if(sqrt(pow(ss.x-x,2)+pow(ss.y-y,2)+pow(ss.z-z,2))<=0.01||sqrt(pow(ss0.x-x,2)+pow(ss0.y-y,2)+pow(ss0.z-z,2))<=0.01)
            {
                RemoveInfo info;
                info.NT=NT;
                info.time=time(0);
                if(delMSGs[0]=="TeraFly")
                info.id=username.toInt()*10+1;
                else if(delMSGs[0]=="TeraVR")
                    info.id=username.toInt()*10+2;
                removedNTList.push_back(info);
                sketchedNTList.removeAt(j);break;
            }
        }
    }
//    global_parameters->messageUsedIndex++;
    qDebug()<<"MessageServerSlotAnswerMessageSocket_delseg end=============";
}

void MessageServer::MessageServerSlotAnswerMessageSocket_addmarker(QString MSG,bool flag)
{
    if(!flag)
        global_parameters->messageUsedIndex++;
    qDebug()<<"messageindex="<<global_parameters->messageUsedIndex;
//    qDebug()<<"MessageServerSlotAnswerMessageSocket_addmarker\n"<<MSG;
    /*MSG=QString("/marker:" +user+"__"+markermsg)*/
        orderList.push_back(QDateTime::currentDateTimeUtc().toString("yyyy/MM/dd hh:mm:ss ")+MSG);
    QRegExp Reg("/marker:(.*)__(.*)");
    QString markerpos;
    QString username;
    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1);
        markerpos=Reg.cap(2);
    }

    int type=2;
    QStringList markerMSGs=markerpos.trimmed().split(" ");
    if(markerMSGs.size()<3) return;
    float mx = markerMSGs.at(0).toFloat();
    float my = markerMSGs.at(1).toFloat();
    float mz = markerMSGs.at(2).toFloat();
    if(markerMSGs.size()==7)
        type=markerMSGs.at(6).toInt();

    CellAPO marker0;
    marker0.x=mx;marker0.y=my;marker0.z=mz;

    marker0.color.r=0;
    marker0.color.g=0;
    marker0.color.b=255;

//    qDebug()<<global_parameters->wholePoint.size();
    marker0.n=global_parameters->wholePoint[global_parameters->wholePoint.size()-1].n+1;//need do something

    marker0.volsize=314.159;
    marker0.orderinfo="";
    marker0.name="";
    marker0.comment="";

    for(int i=0;i<global_parameters->wholePoint.size();i++)
    {
        float dist = /*glm::*/sqrt((global_parameters->wholePoint.at(i).x-marker0.x)*(global_parameters->wholePoint.at(i).x-marker0.x)+
                               (global_parameters->wholePoint.at(i).y-marker0.y)*(global_parameters->wholePoint.at(i).y-marker0.y)+
                               (global_parameters->wholePoint.at(i).z-marker0.z)*(global_parameters->wholePoint.at(i).z-marker0.z));
//        qDebug()<<i<<" "<<global_parameters->wholePoint.at(i).x<<" "<<global_parameters->wholePoint.at(i).y<<" "<<global_parameters->wholePoint.at(i).z;
        if(dist<1)
        {

            global_parameters->wholePoint.removeAt(i);
            qDebug()<<"delete marker "<<global_parameters->wholePoint.size();
            return;
        }
    }
    const int neuron_type_color[ ][3] = {///////////////////////////////////////////////////////
            {255, 255, 255},  // white,   0-undefined
            {20,  20,  20 },  // black,   1-soma
            {200, 20,  0  },  // red,     2-axon
            {0,   20,  200},  // blue,    3-dendrite
            {200, 0,   200},  // purple,  4-apical dendrite
            //the following is Hanchuan's extended color. 090331
            {0,   200, 200},  // cyan,    5
            {220, 200, 0  },  // yellow,  6
            {0,   200, 20 },  // green,   7
            {188, 94,  37 },  // coffee,  8
            {180, 200, 120},  // asparagus,	9
            {250, 100, 120},  // salmon,	10
            {120, 200, 200},  // ice,		11
            {100, 120, 200},  // orchid,	12
        //the following is Hanchuan's further extended color. 111003
        {255, 128, 168},  //	13
        {128, 255, 168},  //	14
        {128, 168, 255},  //	15
        {168, 255, 128},  //	16
        {255, 168, 128},  //	17
        {168, 128, 255}, //	18
        {0, 0, 0}, //19 //totally black. PHC, 2012-02-15
        //the following (20-275) is used for matlab heat map. 120209 by WYN
        {0,0,131}, //20
        {0,0,135},
        {0,0,139},
        {0,0,143},
        {0,0,147},
        {0,0,151},
        {0,0,155},
        {0,0,159},
        {0,0,163},
        {0,0,167},
        {0,0,171},
        {0,0,175},
        {0,0,179},
        {0,0,183},
        {0,0,187},
        {0,0,191},
        {0,0,195},
        {0,0,199},
        {0,0,203},
        {0,0,207},
        {0,0,211},
        {0,0,215},
        {0,0,219},
        {0,0,223},
        {0,0,227},
        {0,0,231},
        {0,0,235},
        {0,0,239},
        {0,0,243},
        {0,0,247},
        {0,0,251},
        {0,0,255},
        {0,3,255},
        {0,7,255},
        {0,11,255},
        {0,15,255},
        {0,19,255},
        {0,23,255},
        {0,27,255},
        {0,31,255},
        {0,35,255},
        {0,39,255},
        {0,43,255},
        {0,47,255},
        {0,51,255},
        {0,55,255},
        {0,59,255},
        {0,63,255},
        {0,67,255},
        {0,71,255},
        {0,75,255},
        {0,79,255},
        {0,83,255},
        {0,87,255},
        {0,91,255},
        {0,95,255},
        {0,99,255},
        {0,103,255},
        {0,107,255},
        {0,111,255},
        {0,115,255},
        {0,119,255},
        {0,123,255},
        {0,127,255},
        {0,131,255},
        {0,135,255},
        {0,139,255},
        {0,143,255},
        {0,147,255},
        {0,151,255},
        {0,155,255},
        {0,159,255},
        {0,163,255},
        {0,167,255},
        {0,171,255},
        {0,175,255},
        {0,179,255},
        {0,183,255},
        {0,187,255},
        {0,191,255},
        {0,195,255},
        {0,199,255},
        {0,203,255},
        {0,207,255},
        {0,211,255},
        {0,215,255},
        {0,219,255},
        {0,223,255},
        {0,227,255},
        {0,231,255},
        {0,235,255},
        {0,239,255},
        {0,243,255},
        {0,247,255},
        {0,251,255},
        {0,255,255},
        {3,255,251},
        {7,255,247},
        {11,255,243},
        {15,255,239},
        {19,255,235},
        {23,255,231},
        {27,255,227},
        {31,255,223},
        {35,255,219},
        {39,255,215},
        {43,255,211},
        {47,255,207},
        {51,255,203},
        {55,255,199},
        {59,255,195},
        {63,255,191},
        {67,255,187},
        {71,255,183},
        {75,255,179},
        {79,255,175},
        {83,255,171},
        {87,255,167},
        {91,255,163},
        {95,255,159},
        {99,255,155},
        {103,255,151},
        {107,255,147},
        {111,255,143},
        {115,255,139},
        {119,255,135},
        {123,255,131},
        {127,255,127},
        {131,255,123},
        {135,255,119},
        {139,255,115},
        {143,255,111},
        {147,255,107},
        {151,255,103},
        {155,255,99},
        {159,255,95},
        {163,255,91},
        {167,255,87},
        {171,255,83},
        {175,255,79},
        {179,255,75},
        {183,255,71},
        {187,255,67},
        {191,255,63},
        {195,255,59},
        {199,255,55},
        {203,255,51},
        {207,255,47},
        {211,255,43},
        {215,255,39},
        {219,255,35},
        {223,255,31},
        {227,255,27},
        {231,255,23},
        {235,255,19},
        {239,255,15},
        {243,255,11},
        {247,255,7},
        {251,255,3},
        {255,255,0},
        {255,251,0},
        {255,247,0},
        {255,243,0},
        {255,239,0},
        {255,235,0},
        {255,231,0},
        {255,227,0},
        {255,223,0},
        {255,219,0},
        {255,215,0},
        {255,211,0},
        {255,207,0},
        {255,203,0},
        {255,199,0},
        {255,195,0},
        {255,191,0},
        {255,187,0},
        {255,183,0},
        {255,179,0},
        {255,175,0},
        {255,171,0},
        {255,167,0},
        {255,163,0},
        {255,159,0},
        {255,155,0},
        {255,151,0},
        {255,147,0},
        {255,143,0},
        {255,139,0},
        {255,135,0},
        {255,131,0},
        {255,127,0},
        {255,123,0},
        {255,119,0},
        {255,115,0},
        {255,111,0},
        {255,107,0},
        {255,103,0},
        {255,99,0},
        {255,95,0},
        {255,91,0},
        {255,87,0},
        {255,83,0},
        {255,79,0},
        {255,75,0},
        {255,71,0},
        {255,67,0},
        {255,63,0},
        {255,59,0},
        {255,55,0},
        {255,51,0},
        {255,47,0},
        {255,43,0},
        {255,39,0},
        {255,35,0},
        {255,31,0},
        {255,27,0},
        {255,23,0},
        {255,19,0},
        {255,15,0},
        {255,11,0},
        {255,7,0},
        {255,3,0},
        {255,0,0},
        {251,0,0},
        {247,0,0},
        {243,0,0},
        {239,0,0},
        {235,0,0},
        {231,0,0},
        {227,0,0},
        {223,0,0},
        {219,0,0},
        {215,0,0},
        {211,0,0},
        {207,0,0},
        {203,0,0},
        {199,0,0},
        {195,0,0},
        {191,0,0},
        {187,0,0},
        {183,0,0},
        {179,0,0},
        {175,0,0},
        {171,0,0},
        {167,0,0},
        {163,0,0},
        {159,0,0},
        {155,0,0},
        {151,0,0},
        {147,0,0},
        {143,0,0},
        {139,0,0},
        {135,0,0},
        {131,0,0},
        {127,0,0} //275
            };

    marker0.color.r=neuron_type_color[type][0];
    marker0.color.g=neuron_type_color[type][1];
    marker0.color.b=neuron_type_color[type][2];
    global_parameters->wholePoint.push_back(marker0);
    qDebug()<<"MessageServerSlotAnswerMessageSocket_addmarker end;";
//    global_parameters->messageUsedIndex++;


}
//not use it
void MessageServer::MessageServerSlotAnswerMessageSocket_delmarker(QString MSG,bool flag)
{
    if(!flag)
    global_parameters->messageUsedIndex++;
    qDebug()<<"messageindex="<<global_parameters->messageUsedIndex;
//    qDebug()<<"MessageServerSlotAnswerMessageSocket_delmarker\n"<<MSG;
    /*MSG=QString("/del_marker:" +user+" "+delmarkerpos )*/
        orderList.push_back(QDateTime::currentDateTimeUtc().toString("yyyy/MM/dd hh:mm:ss ")+MSG);
    QRegExp Reg("/del_marker:(.*)__(.*)");
    QString delmarkerpos;
    QString username;
    if(Reg.indexIn(MSG)!=-1)
    {
        username=Reg.cap(1);
        delmarkerpos=Reg.cap(2);
    }


    QStringList delmarkerPOS = delmarkerpos.trimmed().split(" ");
    if(delmarkerPOS.size()<3)
    {
            qDebug()<<"size < 4";
            return;
    }
//    QString user = delmarkerPOS.at(0);
    float mx = delmarkerPOS.at(0).toFloat();
    float my = delmarkerPOS.at(1).toFloat();
    float mz = delmarkerPOS.at(2).toFloat();

    for(int i=0;i<global_parameters->wholePoint.size();i++)
    {
//        qDebug()<<i<<" "<<global_parameters->wholePoint.at(i).x<<" "<<global_parameters->wholePoint.at(i).y<<" "<<global_parameters->wholePoint.at(i).z;
        CellAPO marker0=global_parameters->wholePoint.at(i);
        double dist=sqrt((mx-marker0.x)*(mx-marker0.x)+
                         (my-marker0.y)*(my-marker0.y)+(mz-marker0.z)*(mz-marker0.z));
//        if(global_parameters->global_scale&& dist<dist_thres/global_parameters->global_scale)
        if(dist<8.0)
        {

            global_parameters->wholePoint.removeAt(i);
//            qDebug()<<"delete marker "<<global_parameters->wholePoint.size();
            break;
        }
    }
//    global_parameters->messageUsedIndex++;


}

//
QMap<quint32 ,QString> MessageServer::autoSave()
{

    quint32 cnt=global_parameters->messageUsedIndex;
    QMap<quint32 ,QString> map;
    QDir rDir("./");
    if(!rDir.cd("autosave"))
    {
        rDir.mkdir("autosave");
    }
    QRegExp fileExp("(.*).ano");
    if(fileExp.indexIn(filename)!=-1)
    {
        QDateTime time=QDateTime::currentDateTime();
        QString strtime=time.toString("yyyy_MM_dd_hh_mm_ss");

        QString filebaseName=fileExp.cap(1);

        {
            QString tempname =filebaseName+"_stamp_autosave_"+strtime+QString::number(cnt);
            QFile anofile("./autosave/"+tempname+".ano");
            anofile.open(QIODevice::WriteOnly);
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
            writeESWC_file(QString("./autosave/%1.ano.eswc").arg(tempname),global_parameters->wholeNT);
            writeAPO_file(QString("./autosave/%1.ano.apo").arg(tempname),global_parameters->wholePoint);
            map[cnt]=tempname+".ano";
        }

        {
            QString tempname ="./clouddata/"+filebaseName+".ano";
//            QFile *f=new QFile(tempname);
//            if(f->exists())
//            {
//                f->remove();
//                delete f;
//                f=new QFile(tempname+".eswc");
//                if(f->exists())
//                {
//                    f->remove();
//                    delete f;
//                    f=new QFile(tempname+".apo");
//                    if(f->exists())
//                    {
//                        f->remove();
//                        delete f;
//                    }
//                }

//            }
            QFile anofile(tempname);
            anofile.open(QIODevice::WriteOnly|QIODevice::Truncate);
            QString str1="APOFILE="+filebaseName+".ano.apo";
            QString str2="SWCFILE="+filebaseName+".ano.eswc";

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
            writeESWC_file("./clouddata/"+fileExp.cap(1)+".ano.eswc",global_parameters->wholeNT);
            writeAPO_file("./clouddata/"+fileExp.cap(1)+".ano.apo",global_parameters->wholePoint);
        }
    }

    //write log
    if(!QDir("./orderfile").exists()) QDir("./").mkdir("orderfile");
    QFile f("./orderfile/"+filename+".txt");
    if(f.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))
    {
        QTextStream txtoutput(&f);
        while (!orderList.isEmpty()) {
            txtoutput<<orderList.at(0)<<endl;
            orderList.removeFirst();
        }
    }
    f.close();

    //
    if(!QDir("./removelog").exists()) QDir("./").mkdir("removelog");

    V_NeuronSWC_list testVNL;
    if(QFile("./removelog/"+filename+".swc").exists())
    {
        NeuronTree remove_stroed=readSWC_file("./removelog/"+filename+".swc");
        testVNL= NeuronTree__2__V_NeuronSWC_list(remove_stroed);
    }
    while(removedNTList.size()!=0)
    {
        NeuronTree NT=removedNTList.at(0).NT;
        V_NeuronSWC seg=NeuronTree__2__V_NeuronSWC_list(NT).seg.at(0);
        for(int j=0;j<seg.row.size();j++)
        {
            seg.row.at(j).timestamp=removedNTList.at(0).time;
            seg.row.at(j).r+=removedNTList.at(0).id*100;
        }
        testVNL.seg.push_back(seg);
        removedNTList.removeAt(0);
    }

    NeuronTree tmp=V_NeuronSWC_list__2__NeuronTree(testVNL);
    writeESWC_file("./removelog/"+filename+".swc",tmp);

    QFile userInfoFile("./userInfo/userInfo.txt");
    if(userInfoFile.open(QIODevice::Truncate|QIODevice::Text|QIODevice::WriteOnly))
        {
            QTextStream __stream(&userInfoFile);
            global_parameters->lock_userInfo.lockForRead();
            for(quint64 i=0;i<global_parameters->userInfo->size();i++)
            {
//                qDebug()<<i;
                QString _=global_parameters->userInfo->keys().at(i)+" "
                        +QString::number(global_parameters->userInfo->value(global_parameters->userInfo->keys().at(i)));
//                qDebug()<<_;
                __stream<<_<<endl;
            }
            global_parameters->lock_userInfo.unlock();
        }
    userInfoFile.close();
    qDebug()<<"aksdjhfkjhjfkh"<<cnt;
    return map;
}


