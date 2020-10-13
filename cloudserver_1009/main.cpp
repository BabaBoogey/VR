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
    QString swcName1="/Users/huanglei/Desktop/1/1.swc";
    QString swcName2="/Users/huanglei/Desktop/1/2.swc";
    qInfo()<<"input1:"<<swcName1<<Qt::endl<<"input2:"<<swcName2;

    auto nt1=readSWC_file(swcName1);
    auto nt2=readSWC_file(swcName2);

    border1=getMaxBorder(nt1);
    border2=getMaxBorder(nt2);
    qInfo()<<"input1:"<<border1<<Qt::endl<<"input2"<<border2;

    vector<vector<vector<char>>> M=vector<vector<vector<char>>>(max(border1[0],border2[0]),vector<vector<char>>(max(border1[1],border2[1]),
            vector<char>(max(border1[2],border2[2]),0)));

    int width=1;
    renderSWC2Matrix(nt1,M,width);
    renderSWC2Matrix(nt2,M,width);


    int cnt=0;
    int cntk=0;
    for(int i=0;i<max(border1[0],border2[0]);i++)
        for(int j=0;j<max(border1[1],border2[1]);j++)
            for(int k=0;k<max(border1[2],border2[2]);k++)
            {
                if(M[i][j][k]!=0) cnt+=1;
                if(M[i][j][k]!=0&&M[i][j][k]!=1) cntk+=1;
            }

    qDebug()<<cntk<<"/"<<cnt<<"="<<1.0*cntk/cnt;
    exit(-1);
//    return a.exec();
}






