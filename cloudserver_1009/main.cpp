#include <QCoreApplication>
#include "basic_c_fun/basic_surf_objs.h"
#include "neuron_editing/neuron_format_converter.h"
#include <QtGlobal>
#include "cac_complex.h"
#include <array>
using std::vector;
int main(int argc, char *argv[])
{

//    if(argc<3) exit(-1);

//    QString swcName1=QString(argv[1]);
//    QString swcName2=QString(argv[2]);
    QString swcName1="/Users/huanglei/Desktop/NR_FARM/c3_stamp_2020_10_01_17_34.ano.eswc";
    QString swcName2="/Users/huanglei/Desktop/NR_FARM/c3_stamp_2020_10_01_17_34.ano.eswc";
    qInfo()<<"input1:"<<swcName1<<Qt::endl<<"input2:"<<swcName2;

    auto nt1=readSWC_file(swcName1);
    auto nt2=readSWC_file(swcName2);

    auto border1=getMaxBorder(nt1);
    auto border2=getMaxBorder(nt2);
    qInfo()<<"input1:"<<border1<<Qt::endl<<"input2"<<border2;

    unsigned long long length=max(max(max(border1[0],border2[0]),max(border1[1],border2[1])),max(border1[2],border2[2]));

    int width1=2;
    auto M1=broadMatrix(renderSWC2Matrix(nt1,length),length,width1);
    unsigned long cnt=0;

    auto M2=broadMatrix(renderSWC2Matrix(nt2,length),length,width1);
    int width2=1;
    auto MJ=broadMatrix(JIAO(M1,M2,length),length,width2);

    for(int i=0;i<length;i++)
        for(int j=0;j<length;j++)
            for(int k=0;k<length;k++)
                if(MJ[i][j][k]) cnt++;

    qInfo()<<"output:"<<cnt<<":"<<1.0*cnt*100/pow(length,3);
    exit(-1);
//    return a.exec();
}






