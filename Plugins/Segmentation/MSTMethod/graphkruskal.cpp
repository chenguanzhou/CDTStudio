#include "graphkruskal.h"
#include <QDebug>


GraphKruskal::GraphKruskal(unsigned elements)
    :elementCount(elements),elts(Q_NULLPTR)
{
    file.open();
    if (file.resize(elements*sizeof(GraphElement)) == false)
    {
        qDebug()<<"Resize Failed!";
        return;
    }
    elts = (GraphElement*) file.map(0, file.size());
    if (elts == Q_NULLPTR)
    {
        qDebug()<<"Map Failed!";
        return;
    }
    GraphElement* p = elts;
    for (unsigned i = 0; i < elements; ++i,++p)
    {
//        p->rank = 0;
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
    {
        y = elts[y].p;
    }
    elts[x].p = y;
    return y;
}

unsigned GraphKruskal::join_band_sw( unsigned x,unsigned y,float edgeWeight )
{
    GraphElement &eltx = *(elts + x);
    GraphElement &elty = *(elts + y);

    if (eltx.size > elty.size)
    {
        eltx.size += elty.size;
        eltx.sw += elty.sw + edgeWeight;
        elty.p = x;

        return x;
    }

    else
    {
        elty.size += eltx.size;
        elty.sw += eltx.sw + edgeWeight;

        eltx.p = y;

//        if (eltx.rank == elty.rank)
//            elty.rank++;

        return y;
    }
}

const float LOG20MULTI2 = 2*log(20.f);
bool GraphKruskal::joinPredicate_sw(unsigned reg1, unsigned reg2, float th, float edgeWeight, int nPredict )
{
    const GraphElement &elt1 = *(elts + reg1);
    const GraphElement &elt2 = *(elts + reg2);

    float sn1 = (float)(th*sqrt(LOG20MULTI2/elt1.size));
    float sn2 = (float)(th*sqrt(LOG20MULTI2/elt2.size));

    if (nPredict==0)//Rule1
    {
        float if1=(elt1.sw+edgeWeight)/(elt1.size+1);
        float if2=(elt2.sw+edgeWeight)/(elt2.size+1);
        return if1<=sn1 || if2<=sn2;
    }
    if (nPredict==1)//Rule2
    {
        return (edgeWeight<=sn1)||(edgeWeight<=sn2);
    }

    return false;
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

//            if (mapRootidObjectid.find(RootNode)==mapRootidObjectid.end())
            if (!mapRootidObjectid.contains(RootNode))
            {
                mapRootidObjectid[RootNode] = objectID;
                objectID++;
            }
        }
    }
}
