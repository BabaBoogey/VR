#include <QCoreApplication>
#include "basic_c_fun/basic_surf_objs.h"
#include "neuron_editing/neuron_format_converter.h"
#include <QtGlobal>
#include "cac_complex.h"
#include <array>
using std::vector;
vector <int> border1;
vector <int> border2;
int main(int argc, char *argv[])
{

//    if(argc<3) exit(-1);

//    QString swcName1=QString(argv[1]);
//    QString swcName2=QString(argv[2]);
    QStringList list={"18454_00003.swc","18454_00004.swc",
                      "18454_00057.swc","18454_00058.swc",
                     "18457_00055.swc","18457_00056.swc",
                     "18457_00061.swc","18457_00062.swc",
                     "18457_00117.swc","18457_00121.swc"};

        QString swcName="/Users/huanglei/Desktop/1/";
        int width=13;


            qDebug()<<"---------------------------------"<<"width="+QString::number(width)+"-----------------------------------";
            for(int i=0;i<5;i++)
            {
                QString swcName1=swcName+list[2*i];
                QString swcName2=swcName+list[2*i+1];


//                qInfo()<<"input1:"<<swcName1<<Qt::endl<<"input2:"<<swcName2;

                auto nt1=readSWC_file(swcName1);
                auto nt2=readSWC_file(swcName2);

                border1=getMaxBorder(nt1);
                border2=getMaxBorder(nt2);
//                qInfo()<<"input1:"<<border1<<Qt::endl<<"input2"<<border2;

                vector<vector<vector<char>>> M=vector<vector<vector<char>>>(max(border1[0],border2[0]),vector<vector<char>>(max(border1[1],border2[1]),
                        vector<char>(max(border1[2],border2[2]),0)));
                for(int i=0;i<max(border1[0],border2[0]);i++)
                    for(int j=0;j<max(border1[1],border2[1]);j++)
                        for(int k=0;k<max(border1[2],border2[2]);k++)
                        {
                            if(M[i][j][k]!=0) qDebug()<<"0:error";
                        }
                qDebug()<<list[2*i];
                int cntA=renderSWC2Matrix(nt1,M,width);
                qDebug()<<list[2*i+1];
                int cntB=renderSWC2Matrix(nt2,M,width);


                int cnt=0;
                int cntk=0;
                for(int i=0;i<max(border1[0],border2[0]);i++)
                    for(int j=0;j<max(border1[1],border2[1]);j++)
                        for(int k=0;k<max(border1[2],border2[2]);k++)
                        {
                            if(M[i][j][k]!=0) cnt+=1;
                            if(M[i][j][k]!=0&&M[i][j][k]!=1) cntk+=1;
                        }
                qDebug()<<list[2*i]<<" "<<list[2*i+1]<<":"<<width;
                qDebug()<<cntk<<"/"<<cnt<<"="<<1.0*cntk/cnt;
                qDebug()<<cntk<<"/"<<cntA<<"="<<1.0*cntk/cntA;
                qDebug()<<cntk<<"/"<<cntB<<"="<<1.0*cntk/cntB;
                qDebug()<<"----------------------------------------------------------------------";
            }



    exit(-1);
//    return a.exec();
}






