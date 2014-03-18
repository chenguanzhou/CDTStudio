#include "graphkruskal.h"
#include <QDebug>


GraphKruskal::GraphKruskal(unsigned elements)
    :elementCount(elements),num(elements),elts(NULL)
{
//        elts.resize(elements);
//        for (unsigned i = 0; i < elements; i++)
//        {
//            elts[i].p = i;//每个区域（集合）的初始根节点是它本身
//        }


    file.open();
    if (file.resize(elements*sizeof(GraphElement)) == false)
    {
        qDebug()<<"Resize Failed!";
        return;
    }
    elts = (GraphElement*) file.map(0, file.size());
    if (elts == NULL)
    {
        qDebug()<<"Map Failed!";
        return;
    }
    GraphElement* p = elts;
    for (unsigned i = 0; i < elements; ++i,++p)
    {
        p->rank = 0;
        p->p = i;//每个区域（集合）的初始根节点是它本身
        p->sw = 0;
        p->size = 1;
    }
}

GraphKruskal::~GraphKruskal()
{
    if (elts) file.unmap((uchar *)elts);
}

unsigned GraphKruskal::find( unsigned x )
{
    int y = x;

    while (y != elts[y].p)//p为x的父节点，不相等，说明x不是根节点
        y = elts[y].p;//找x的父节点的父节点，直到相等，说明找到了根节点

    elts[x].p = y;//将x的父节点设为找到的根节点，优化,提高查找速度
    return y;
}

unsigned GraphKruskal::join_band_sw( unsigned x,unsigned y,float edgeWeight )
{
    if (elts[x].rank > elts[y].rank)
        {
            elts[x].size += elts[y].size; //合并所得区域的大小
            elts[x].sw += elts[y].sw + edgeWeight;//合并所得区域的边权和
            elts[y].p = x;

            num--;//合并后区域数减一
            return x;
        }

        else
        {
            elts[y].size += elts[x].size;//区域大小
            elts[y].sw += elts[x].sw + edgeWeight;//组成该区域的边权和

            elts[x].p = y;

            if (elts[x].rank == elts[y].rank)
                elts[y].rank++;//同时将集合y的元素数加1
            num--;
            return y;
        }
}


//预测两个区域是否合并,控制边权和的大小

const float LOG20MULTI2 = 2*log(2/0.1f);
bool GraphKruskal::joinPredicate_sw(unsigned reg1, unsigned reg2, float th, float edgeWeight, int nPredict )
{
    GraphElement elt1 = elts[reg1];
    GraphElement elt2 = elts[reg2];

    float swreg1 = elt1.sw;
    unsigned size1=elt1.size;//区域1的像素数

    float swreg2 = elt2.sw;
    unsigned size2=elt2.size;//区域2的像素数

    float nedge1 = (float)size1-1+0.000001f;//区域1的边数
    float nedge2 = (float)size2-1+0.000001f;//区域2的边数

    double g=th;//255*sqrt(m_Bands);//

    float if1=(swreg1+edgeWeight)/(nedge1+2);//把当前边权加入区域1的边权后的区域边权均值
    float if2=(swreg2+edgeWeight)/(nedge2+2);//把当前边权加入区域2的边权后的区域边权均值


    float sn1 = (float)(g*sqrt(LOG20MULTI2/size1));
    float sn2 = (float)(g*sqrt(LOG20MULTI2/size2));


    bool bMerge=false;
    if (nPredict==0)//准则1
    {
        if(if1<=sn1 || if2<=sn2)//ok
            bMerge=true;
        else
            bMerge=false;
    }
    if (nPredict==1)//准则2
    {
        if ((edgeWeight<=sn1)||(edgeWeight<=sn2))//ok
            bMerge=true;
        else
            bMerge=false;
    }

    return bMerge;
}

void GraphKruskal::GetMapNodeidObjectid(GDALRasterBand *&poMaskBand, QMap<unsigned, unsigned> &mapRootidObjectid)
{
    mapRootidObjectid.clear();
    int width = poMaskBand->GetXSize();
    int height = poMaskBand->GetYSize();
    std::vector<uchar> mask(width);

    unsigned objectID = 0;
    unsigned index = 0;
    for(int i=0;i<height;++i)
    {
        poMaskBand->RasterIO(GF_Read,0,i,width,1,&mask[0],width,1,GDT_Byte,0,0);
        for (int j=0;j<width;++j,++index)
        {
            if (mask[j]==0)
                continue;
            unsigned RootNode=find(index);
            if (mapRootidObjectid.find(RootNode)==mapRootidObjectid.end())
            {
                mapRootidObjectid[RootNode] = objectID;
                objectID++;
            }
        }
    }
}
