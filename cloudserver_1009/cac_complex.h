#ifndef CAC_COMPLEX_H
#define CAC_COMPLEX_H
#include "basic_c_fun/basic_surf_objs.h"
#include "neuron_editing/neuron_format_converter.h"
#include <QtGlobal>
#include <vector>
extern vector <int> border1;
extern vector <int> border2;
std::vector<int> getMaxBorder(const NeuronTree & T)
{
    int max_x=0;
    int max_y=0;
    int max_z=0;

    for(auto &node:T.listNeuron)
    {
        if(node.x>=max_x) max_x=node.x;
        if(node.y>=max_y) max_y=node.y;
        if(node.z>=max_z) max_z=node.z;
    }

    return {max_x,max_y,max_z};
}

void renderSWC2Matrix(NeuronTree &T,vector<vector<vector<char>>> &RESM,int d=1)
{
    vector<vector<vector<char>>> M=vector<vector<vector<char>>>(max(border1[0],border2[0]),vector<vector<char>>(max(border1[1],border2[1]),
            vector<char>(max(border1[2],border2[2]),0)));
    auto segments=NeuronTree__2__V_NeuronSWC_list(T);
    auto pp=segments.seg.size();
        int cnt=0;
    for(pp=0;pp<segments.seg.size();pp++)
    {

        auto &seg=segments.seg[pp];
//        qDebug()<<pp<<"/"<<segments.seg.size();
        for(auto &node:seg.row)
        {
            if(node.parent==-1) continue;

            int pn=node.parent;
            int x1,x2,y1,y2,z1,z2;

            x1=node.x;
            y1=node.y;
            z1=node.z;

            x2=seg.row[pn].x;
            y2=seg.row[pn].y;
            z2=seg.row[pn].z;

//            qDebug()<<x1<<","<<y1<<","<<z1;
            double dx=(x2-x1);
            double dy=(y2-y1);
            double dz=(z2-z1);

            for(double bias=0.000;bias<=1;bias+=0.001)
            {
//                qDebug()<<bias<<dx<<dy<<dz;
                long long  x=x1+bias*dx;
                long long  y=y1+bias*dy;
                long long  z=z1+bias*dz;

                for(int i=-d;i<=d;i+=1)
                {
                    if(x+i<0) continue;
                    if(x+i>=max(border1[0],border2[0])) continue;
                    for(int j=-d;j<=d;j+=1)
                    {
                        if(y+j<0) continue;
                        if(y+j>=max(border1[1],border2[1])) continue;
                        for(int k=-d;k<=d;k+=1)
                        {
                            if(z+k<0) continue;
                            if(z+k>=max(border1[2],border2[2])) continue;
//                            qDebug()<<x+i<<" "<<y+j<<" "<<z+k<< z<<" "<<k;

                            M[x+i][y+j][z+k]=1;
//                            cnt+=1;.
//                            qDebug()<<"cnt:"<<cnt;
                        }
                    }
                }
            }
        }
    }
    cnt=0;
    for(int i=0;i<max(border1[0],border2[0]);i++)
    {
        for(int j=0;j<max(border1[1],border2[1]);j++)
        {
            for(int k=0;k<max(border1[2],border2[2]);k++)
            {
                if(M[i][j][k]!=0)
                {
                    RESM[i][j][k]+=1;
                    cnt+=1;
                }
            }
        }
    }
    qDebug()<<cnt;
    qDebug()<<"-----------------------------------------------------";
}

#endif // CAC_COMPLEX_H
