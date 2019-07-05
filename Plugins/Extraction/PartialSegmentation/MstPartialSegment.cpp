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
    MSTMethodPrivate():_threshold(64),_minObjectSize(24),shield_0_255(false){}
    double _threshold;
    int _minObjectSize;
    bool shield_0_255;
};

MstPartialSegment::MstPartialSegment():pData(new MSTMethodPrivate)
{

}

MstPartialSegment::~MstPartialSegment()
{

}

bool MstPartialSegment::Start()
{
    clock_t tSatart = clock();

    if(!CheckAndInit())
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

    CreateEdgeWeights(vecEdge);

    qDebug()<<"Create Edge Weights cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();
    qDebug()<<"Sort the edges";

    vecEdge->sort();

    qDebug()<<"Sort Edge Weights cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();

    GraphKruskal* graph = Q_NULLPTR;

    qDebug()<<"Merging: "+QString::number(vecEdge->size());
    ObjectMerge(graph, vecEdge, m_nWidth*m_nHeight, pData->_threshold);

    qDebug()<<"Merge Object cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();

    qDebug()<<"Eliminate small object";
    EliminateSmallArea(graph,vecEdge,pData->_minObjectSize);

    qDebug()<<"Eliminate Small Area cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();

    vecEdge->clear();
    delete vecEdge;

    qDebug()<<"Generating index";
    //    std::map<unsigned,unsigned> mapRootidObjectid;
    QMap<unsigned,unsigned> mapRootidObjectid;

    graph->GetMapNodeidObjectid((void*)m_pPolygon, mapRootidObjectid);

    qDebug()<<"Get Map Nodeid Objectid cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();

    qDebug()<<"Generating result";
    GenerateFlagImage(graph,mapRootidObjectid);
    delete graph;

    qDebug()<<"Generate Flag Image cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();

    GDALClose(poSrcDS);
    GDALClose(poDstDS);

    qDebug()<<"Polygonizing";
    Polygonize();

    qDebug()<<"mst segmentation cost: "<<(clock()-tSatart)/(double)CLOCKS_PER_SEC<<"s";
}

bool MstPartialSegment::CheckAndInit()
{
    //1.Get xoff and yoff and width and height
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float maxY = std::numeric_limits<float>::min();

    OGRPolygon* pPolygon = (OGRPolygon*)m_pPolygon;
    OGRLinearRing* pOgrLinerRing = pPolygon->getExteriorRing();
    int nRingPointCount = pOgrLinerRing->getNumPoints();
    if( 0 >= nRingPointCount)
    {
        return false;
    }

    OGRRawPoint* ringPoints = new OGRRawPoint[nRingPointCount];
    pOgrLinerRing->getPoints(ringPoints);

    for (size_t i=0;i < nRingPointCount;++i)
    {
        if (ringPoints[i].x<minX)
            minX = ringPoints[i].x;
        if (ringPoints[i].x>maxX)
            maxX = ringPoints[i].x;
        if (ringPoints[i].y<minY)
            minY = ringPoints[i].y;
        if (ringPoints[i].y>maxY)
            maxY = ringPoints[i].y;
    }

    m_nXOff = minX;
    m_nYOff = minY;
    m_nWidth = maxX - minX + 1;
    m_nHeight = maxY - minY + 1;

    qDebug()<<"nXOff: "+QString::number(m_nXOff);
    qDebug()<<"nYOff: "+QString::number(m_nYOff);
    qDebug()<<"nWidth: "+QString::number(m_nWidth);
    qDebug()<<"nHeight: "+QString::number(m_nHeight);


    //2.Check Input Image
    if ( Q_NULLPTR == m_pSrcDS)
    {
        return false;
    }

    GDALDataset* pSrcDS = (GDALDataset*)m_pSrcDS;
    if (m_layerWeights.size()==0)
    {
        m_layerWeights.resize(pSrcDS->GetRasterCount());
        for(QVector<double>::Iterator iter = m_layerWeights.begin();iter != m_layerWeights.end();++iter)
        {
            *iter = 1./pSrcDS->GetRasterCount();
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

    poDstDS->CreateMaskBand(GMF_PER_DATASET);

    const int nMaskHasValue = 0;
    const int nMaskNullValue = 1;

    GDALRasterBand* poBand = poDstDS->GetRasterBand(1)->GetMaskBand();
    for(int i=0,index =0;i<m_nHeight; ++i)
    {
        for(int j=0;j<m_nWidth; ++j,++index)
        {
            OGRPoint pPoint(j + m_nXOff, i + m_nYOff);
            if (!pPolygon->IsPointOnSurface(&pPoint))
            {
                poBand->RasterIO(GF_Write,j,i,1,1,(int *)&nMaskHasValue,1,1,GDT_Int32,0,0);
            }
            else
            {
                 poBand->RasterIO(GF_Write,j,i,1,1,(int *)&nMaskNullValue,1,1,GDT_Int32,0,0);
            }

        }
    }

    double adfGeoTransform[6];
    poDstDS->SetProjection(pSrcDS->GetProjectionRef());

    pSrcDS->GetGeoTransform(adfGeoTransform);

    double dP0 = adfGeoTransform[0] + adfGeoTransform[1] * m_nXOff + adfGeoTransform[2]*m_nYOff;
    double dP3 = adfGeoTransform[3] + adfGeoTransform[4] * m_nXOff + adfGeoTransform[5]*m_nYOff;

    adfGeoTransform[0] = dP0;
    adfGeoTransform[3] = dP3;

    poDstDS->SetGeoTransform(adfGeoTransform);
    return true;
}

bool MstPartialSegment::CreateEdgeWeights(void *p)
{
    EdgeVector* vecEdge = (EdgeVector*)p;
    vecEdge->clear();

    GDALDataset* poSrcDS = (GDALDataset*)m_pSrcDS;

    unsigned nBandCount = poSrcDS->GetRasterCount();
    GDALDataType gdalDataType = poSrcDS->GetRasterBand(1)->GetRasterDataType();
    unsigned pixelSize = GDALGetDataTypeSize(gdalDataType)/8;

    QVector<double> buffer1(nBandCount);
    QVector<double> buffer2(nBandCount);

    std::vector<unsigned char*> lineBufferUp(nBandCount);
    std::vector<unsigned char*> lineBufferDown(nBandCount);

    for (unsigned k=0;k<nBandCount;++k)
    {
        lineBufferUp[k] = new unsigned char[m_nWidth*pixelSize];
        lineBufferDown[k] = new unsigned char[m_nWidth*pixelSize];
    }

    for (unsigned k=0;k<nBandCount;++k)
    {
        poSrcDS->GetRasterBand(k+1)->RasterIO(GF_Read,m_nXOff,m_nYOff,m_nWidth,1,lineBufferUp[k],m_nWidth,1,gdalDataType,0,0);
    }
    const int progressGap = (m_nHeight-1)/100;

    for (unsigned y=0; y<m_nHeight-1; ++y)
    {
        for (unsigned k=0;k<nBandCount;++k)
        {
             poSrcDS->GetRasterBand(k+1)->RasterIO(GF_Read,m_nXOff, m_nYOff+y+1, m_nWidth,1,lineBufferDown[k],m_nWidth,1,gdalDataType,0,0);
        }

        for(unsigned x=0; x<m_nWidth; ++x)
        {

            unsigned nodeID1 = y * m_nWidth + x;
            unsigned nodeIDNextLIne = nodeID1 + m_nWidth;
            if (x < m_nWidth-1 )
            {
//                if (maskUp[x+1]!=0)
//                {
                    for(unsigned k=0;k<nBandCount;++k)
                    {
                        buffer1[k] = SRCVAL(lineBufferUp[k],gdalDataType,x);
                        buffer2[k] = SRCVAL(lineBufferUp[k],gdalDataType,x+1);
                    }
                    onComputeEdgeWeight(nodeID1,nodeID1+1,buffer1,buffer2,m_layerWeights,vecEdge);
//                }
            }

            if(y < m_nHeight-1)
            {
//                if (maskDown[x]!=0)
//                {
                    for(unsigned k=0;k<nBandCount;++k)
                    {
                        buffer1[k] = SRCVAL(lineBufferUp[k],gdalDataType,x);
                        buffer2[k] = SRCVAL(lineBufferDown[k],gdalDataType,x);
                    }
                    onComputeEdgeWeight(nodeID1,nodeIDNextLIne,buffer1,buffer2,m_layerWeights,vecEdge);
//                }
            }
        }

        std::vector<unsigned char*> tempBuffer = lineBufferDown;
        lineBufferDown = lineBufferUp;
        lineBufferUp = tempBuffer;

//        uchar* p    = maskUp;
//        maskUp      = maskDown;
//        maskDown    = p;
    }

    for (unsigned k=0;k<nBandCount;++k)
    {
        delete [](lineBufferUp[k]);
        delete [](lineBufferDown[k]);
    }
//    delete []maskUp;
//    delete []maskDown;

    return true;
}

bool MstPartialSegment::ObjectMerge(GraphKruskal *&graph, void *p, int num_vertices, float threshold)
{
    EdgeVector* vecEdge = (EdgeVector*)p;
    graph = new GraphKruskal(num_vertices);

    //    boost::progress_display pd(vecEdge.size(),std::cout,"merging\n","","");
    //    for (unsigned k = 0; k < vecEdge.size(); ++k)
    int barSize = vecEdge->size();
    qDebug()<<"ObjectMerge : vecEdge->size()"+QString::number(barSize);
//    emit progressBarSizeChanged(0,barSize);
    int i=0;
    const int progressGap = barSize/100;

    while(!vecEdge->empty())
    {
        edge edge_temp = *(*vecEdge);

        unsigned a = graph->find(edge_temp.GetNode1());
        unsigned b = graph->find(edge_temp.GetNode2());

//        qDebug()<<"a: "+ QString::number(a)+"b: "+ QString::number(b)+"weight: "<< QString::number(edge_temp.GetWeight());


        int nPredict = 0;
        if ((a != b) && graph->joinPredicate_sw(a,b,(float)threshold,edge_temp.GetWeight(),nPredict))
        {
            graph->join_band_sw(a,b,edge_temp.GetWeight());
        }
        ++(*vecEdge);
//        if (i%progressGap==0)
//            qDebug()<<QString::number(i);
        //            emit progressBarValueChanged(i);
        ++i;
        //        ++pd;
    }
//    emit progressBarValueChanged(barSize);
    return true;
}

bool MstPartialSegment::EliminateSmallArea(GraphKruskal *&graph, void *p, double _minObjectSize)
{
    EdgeVector* vecEdge = (EdgeVector*)p;
    vecEdge->rewind();
    int barSize = vecEdge->size();
    int i=0;
    const int progressGap = barSize/100;
    qDebug()<<"EliminateSmallArea : vecEdge->size() "+QString::number(barSize);
    qDebug()<<"EliminateSmallArea : _minObjectSize "+QString::number(_minObjectSize);

    //    boost::progress_display pd(vecEdge.size(),std::cout,"eliminating small area\n","","");
    //    for (unsigned k = 0; k < vecEdge.size(); ++k)
    while(!vecEdge->empty())
    {
        edge edge_temp = *(*vecEdge);
        unsigned a = graph->find(edge_temp.GetNode1());
        unsigned b = graph->find(edge_temp.GetNode2());

        if ((a != b) && ((graph->size(a) <_minObjectSize) || (graph->size(b) < _minObjectSize)) )
        {
            graph->join_band_sw(a,b,edge_temp.GetWeight());
        }
        ++(*vecEdge);
        //        ++pd;
        ++i;
    }

    return true;
}

bool MstPartialSegment::GenerateFlagImage(GraphKruskal *&graph, const QMap<unsigned, unsigned> &mapRootidObjectid)
{
    GDALDataset* poDstDS = (GDALDataset*)m_pDstDS;
    OGRPolygon* pPolygon = (OGRPolygon*)m_pPolygon;

    GDALRasterBand* poFlagBand = poDstDS->GetRasterBand(1);


    for(int i=0,index =0;i<m_nHeight; ++i)
    {
        for(int j=0;j<m_nWidth; ++j,++index)
        {
            int objectID = 0;
            OGRPoint pPoint(j + m_nXOff, i + m_nYOff);
            if (pPolygon->IsPointOnSurface(&pPoint))
            {
                int root = graph->find(index);
                objectID = mapRootidObjectid.value(root);
            }
            poFlagBand->RasterIO(GF_Write,j,i,1,1,(int *)&objectID,1,1,GDT_Int32,0,0);
        }
    }

    return true;
}

bool MstPartialSegment::Polygonize()
{
    GDALDataset *poFlagDS = (GDALDataset *)GDALOpen(m_strMarkfilePath.toUtf8().constData(),GA_ReadOnly);
    if (poFlagDS == Q_NULLPTR)
    {
        return false;
    }

    GDALDriver * poOgrDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
    if (poOgrDriver == Q_NULLPTR)
    {
        GDALClose(poFlagDS);
        return false;
    }

    QFileInfo info(m_strShapefilePath);
    if (info.exists())
        poOgrDriver->Delete(m_strShapefilePath.toUtf8().constData());

    GDALDataset* poDstDataset = poOgrDriver->Create(m_strShapefilePath.toUtf8().constData(),0,0,0,GDT_Unknown,NULL);
    if (poDstDataset == Q_NULLPTR)
    {
        GDALClose(poFlagDS);
        return false;
    }

    OGRSpatialReference pSpecialReference;
    pSpecialReference.SetProjection(poFlagDS->GetProjectionRef());

    const char* layerName = "polygon";
    OGRLayer* poLayer = poDstDataset->CreateLayer(layerName,&pSpecialReference,wkbPolygon,0);
    if (poLayer == Q_NULLPTR)
    {
        GDALClose(poFlagDS);
        GDALClose( poDstDataset );
        return false;
    }

    OGRFieldDefn ofDef_DN( "GridCode", OFTInteger );
    if ( (poLayer->CreateField(&ofDef_DN) != OGRERR_NONE) )
    {
        GDALClose(poFlagDS);
        GDALClose( poDstDataset );
        return false;
    }

    char** papszOptions = Q_NULLPTR;
    papszOptions = CSLSetNameValue(papszOptions,"8CONNECTED","8");
    GDALRasterBand *poFlagBand = poFlagDS->GetRasterBand(1);
    GDALRasterBand *poMaskBand = poFlagBand->GetMaskBand();
    CPLErr err = GDALPolygonize((GDALRasterBandH)poFlagBand,(GDALRasterBandH)poMaskBand,(OGRLayerH)poLayer,0,papszOptions,0,0);
    if (err != CE_None)
    {
        GDALClose(poFlagDS);
        GDALClose( poDstDataset );
        return false;
    }

    GDALClose(poFlagDS);
    GDALClose(poDstDataset);
    return true;
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
