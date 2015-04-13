#include "graphkruskal.h"
#include <QDebug>


GraphKruskal::GraphKruskal(unsigned elements)
    :num(elements),elementCount(elements),elts(NULL)
{
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
        p->p = i;
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
    unsigned y = x;

    while (y != elts[y].p)
        y = elts[y].p;//Find root node

    elts[x].p = y;
    return y;
}

unsigned GraphKruskal::join_band_sw( unsigned x,unsigned y,float edgeWeight )
{
    if (elts[x].rank > elts[y].rank)
        {
            elts[x].size += elts[y].size;
            elts[x].sw += elts[y].sw + edgeWeight;
            elts[y].p = x;

            num--;
            return x;
        }

        else
        {
            elts[y].size += elts[x].size;
            elts[y].sw += elts[x].sw + edgeWeight;

            elts[x].p = y;

            if (elts[x].rank == elts[y].rank)
                elts[y].rank++;
            num--;
            return y;
        }
}



const float LOG20MULTI2 = 2*log(2/0.1f);
bool GraphKruskal::joinPredicate_sw(unsigned reg1, unsigned reg2, float th, float edgeWeight, int nPredict )
{
    GraphElement elt1 = elts[reg1];
    GraphElement elt2 = elts[reg2];

    float swreg1 = elt1.sw;
    unsigned size1=elt1.size;

    float swreg2 = elt2.sw;
    unsigned size2=elt2.size;

    float nedge1 = (float)size1-1+0.000001f;
    float nedge2 = (float)size2-1+0.000001f;

    double g=th;//255*sqrt(m_Bands);//

    float if1=(swreg1+edgeWeight)/(nedge1+2);
    float if2=(swreg2+edgeWeight)/(nedge2+2);


    float sn1 = (float)(g*sqrt(LOG20MULTI2/size1));
    float sn2 = (float)(g*sqrt(LOG20MULTI2/size2));


    bool bMerge=false;
    if (nPredict==0)//Rule1
    {
        if(if1<=sn1 || if2<=sn2)//ok
            bMerge=true;
        else
            bMerge=false;
    }
    if (nPredict==1)//Rule2
    {
        if ((edgeWeight<=sn1)||(edgeWeight<=sn2))
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
