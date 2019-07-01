#include "MstPartialSegment.h"
#include <exception>
#include <algorithm>
#include <iostream>
#include <QtCore>
#include <gdal_priv.h>
#include <gdal_alg.h>
#include <ogrsf_frmts.h>
#include <ogr_api.h>
#include "graphkruskal.h"

class edge
{
public:
    edge(){}
    edge(unsigned nodeID1,unsigned nodeID2,float weight):_weight(weight),_nodeID1(nodeID1),_nodeID2(nodeID2){}
    bool operator<(const edge& other) const
    {
        return _weight<other._weight;
    }
    float GetWeight()const{return _weight;}
    unsigned GetNode1()const{return _nodeID1;}
    unsigned GetNode2()const{return _nodeID2;}
private:
    float _weight;
    unsigned _nodeID1;
    unsigned _nodeID2;
};

struct edge_comparator
{
    bool operator () (const edge& a, const edge& b) const
    {
        return a.GetWeight() < b.GetWeight();
    }
    edge min_value() const
    {
        return edge(0,0,std::numeric_limits<float>::min());
    }
    edge max_value() const
    {
        return edge(0,0,std::numeric_limits<float>::max());
    }
};

typedef stxxl::sorter<edge,edge_comparator> EdgeVector;

class GraphVertex
{
public:
    GraphVertex(unsigned xx=0,unsigned yy=0):x(xx),y(yy){}
    unsigned x,y;
};

class GraphEdge
{
public:
    float weight;
};

class MSTMethodPrivate
{
public:
    MSTMethodPrivate():_threshold(20),_minObjectSize(100),shield_0_255(false){}
    double _threshold;
    int _minObjectSize;
    bool shield_0_255;
};

MstPartialSegment::MstPartialSegment()
{

}

MstPartialSegment::~MstPartialSegment()
{

}

bool MstPartialSegment::Start()
{
    if(true != CheckAndInit())
    {
        return false;
    }

    GDALDataset* poSrcDS = static_cast<GDALDataset*>(m_pSrcDS);
    GDALDataset* poDstDS = static_cast<GDALDataset*>(m_pDstDS);

    clock_t timer = clock();
    unsigned memory_to_use = 500 * 1024 * 1024;
    EdgeVector *vecEdge = new EdgeVector(edge_comparator(),memory_to_use);

    qDebug()<<"Init EdgeVector cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();

}

bool MstPartialSegment::CheckAndInit()
{
    //2.Check Input Image
    if ( Q_NULLPTR == m_pSrcDS)
    {
        return false;
    }

    if (m_veclayerWeights.size()==0)
    {
        m_veclayerWeights.resize(m_pSrcDS->GetRasterCount());
        for(QVector<double>::Iterator iter = m_veclayerWeights.begin();iter != m_veclayerWeights.end();++iter)
        {
            *iter = 1./m_pSrcDS->GetRasterCount();
        }
    }

    //3.Init Output Image
    char** pszOptions = Q_NULLPTR;
    pszOptions = CSLSetNameValue(pszOptions,"COMPRESS","LZW");
    pszOptions = CSLSetNameValue(pszOptions,"PREDICTOR","2");
    pszOptions = CSLSetNameValue(pszOptions,"ZLEVEL","9");
    GDALDriver* poDriver = static_cast<GDALDriver*>(GDALGetDriverByName("GTiff"));
    GDALDataset* poDstDS = poDriver->Create(m_strMarkfilePath.toUtf8().constData(), m_nWidth, m_nHeight,1,GDT_Int32,pszOptions);
    if (poDstDS == Q_NULLPTR)
    {
        return false;
    }
    m_pDstDS = poDstDS;

    double adfGeoTransform[6];
    poDstDS->SetProjection(m_pSrcDS->GetProjectionRef());

    m_pSrcDS->GetGeoTransform(adfGeoTransform);

    double dP0 = adfGeoTransform[0] + adfGeoTransform[1] * m_nXOff + adfGeoTransform[2]*m_nYOff;
    double dP3 = adfGeoTransform[3] + adfGeoTransform[4] * m_nXOff + adfGeoTransform[5]*m_nYOff;

    adfGeoTransform[0] = dP0;
    adfGeoTransform[3] = dP3;

    poDstDS->SetGeoTransform(adfGeoTransform);
    return true;
}

bool MstPartialSegment::CreateEdgeWeights()
{

}

void MstPartialSegment::onComputeEdgeWeight(unsigned nodeID1, unsigned nodeID2, const QVector<double> &data1, const QVector<double> &data2, const QVector<double> &layerWeight, void *p)
{
    EdgeVector* vecEdge = (EdgeVector*)p;
    double  difs=0;
    for (int k=0;k<data1.size();++k)
    {
        difs += ((data1[k]-data2[k])*layerWeight[k])*((data1[k]-data2[k])*layerWeight[k]);
    }
    vecEdge->push(edge(nodeID1,nodeID2,(float)sqrt(difs)));
}
