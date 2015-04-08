#include "mstmethodinterface.h"
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

MSTMethodInterface::MSTMethodInterface(QObject *parent)
    :CDTSegmentationInterface(parent),
      pData(new MSTMethodPrivate)
{
}

MSTMethodInterface::~MSTMethodInterface()
{
}

QString MSTMethodInterface::segmentationMethod() const
{
    return QString("mst");
}

void MSTMethodInterface::startSegmentation()
{
    clock_t tSatart = clock();

    if (_CheckAndInit() == false)
    {
        return ;
    }

    GDALDataset* poSrcDS = (GDALDataset*)srcDS;
    GDALDataset* poDstDS = (GDALDataset*)dstDS;

    clock_t timer = clock();
    unsigned memory_to_use = 500 * 1024 * 1024;
    EdgeVector *vecEdge = new EdgeVector(edge_comparator(),memory_to_use);

    qDebug()<<"Init EdgeVector cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();

    emit currentProgressChanged(tr("Create edges of graph"));
    _CreateEdgeWeights(vecEdge);

    qDebug()<<"Create Edge Weights cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();
    emit currentProgressChanged(tr("Sort the edges"));

    vecEdge->sort();

    qDebug()<<"Sort Edge Weights cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();

    GraphKruskal* graph = NULL;
    emit currentProgressChanged(tr("Merging"));
    _ObjectMerge(graph,vecEdge,poSrcDS->GetRasterXSize()*poSrcDS->GetRasterYSize(),pData->_threshold);

    qDebug()<<"Merge Object cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();

    emit currentProgressChanged(tr("Eliminate small object"));
    _EliminateSmallArea(graph,vecEdge,pData->_minObjectSize);

    qDebug()<<"Eliminate Small Area cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();

    vecEdge->clear();
    delete vecEdge;

    emit currentProgressChanged(tr("Generating index"));
    //    std::map<unsigned,unsigned> mapRootidObjectid;
    QMap<unsigned,unsigned> mapRootidObjectid;

    GDALRasterBand* poMaskBand = poDstDS->GetRasterBand(1)->GetMaskBand();
    graph->GetMapNodeidObjectid(poMaskBand, mapRootidObjectid);

    qDebug()<<"Get Map Nodeid Objectid cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();

    emit currentProgressChanged(tr("Generating result"));
    _GenerateFlagImage(graph,mapRootidObjectid);
    delete graph;

    qDebug()<<"Generate Flag Image cost:"<<(clock()-timer)/(double)CLOCKS_PER_SEC<<"s";
    timer = clock();

    GDALClose(poSrcDS);
    GDALClose(poDstDS);

    emit currentProgressChanged(tr("Polygonizing"));
//    Polygonizer* polygonizer = new Polygonizer(_outputImagePath,_shapefilePath);
//    connect(polygonizer,SIGNAL(finished()),polygonizer,SLOT(deleteLater()));
//    connect(polygonizer,SIGNAL(currentProgressChanged(QString)),this,SIGNAL(currentProgressChanged(QString)));
//    connect(polygonizer,SIGNAL(progressBarValueChanged(int)),this,SIGNAL(progressBarValueChanged(int)));
//    connect(polygonizer,SIGNAL(progressBarSizeChanged(int,int)),this,SIGNAL(progressBarSizeChanged(int,int)));
//    connect(polygonizer,SIGNAL(showNormalMessage(QString)),this,SIGNAL(showNormalMessage(QString)));
//    connect(polygonizer,SIGNAL(showWarningMessage(QString)),this,SIGNAL(showWarningMessage(QString)));
//    polygonizer->run();
    _Polygonize();

    qDebug()<<"mst segmentation cost: "<<(clock()-tSatart)/(double)CLOCKS_PER_SEC<<"s";
}

double MSTMethodInterface::threshold() const
{
    return pData->_threshold;
}

int MSTMethodInterface::minObjectCount() const
{
    return pData->_minObjectSize;
}

bool MSTMethodInterface::shieldNulValue() const
{
    return pData->shield_0_255;
}

void MSTMethodInterface::setThreshold(double val)
{
    pData->_threshold = val;
}

void MSTMethodInterface::setMinObjectCount(int val)
{
    pData->_minObjectSize = val;
}

void MSTMethodInterface::setShieldNulValue(bool val)
{
    pData->shield_0_255 = val;
}

bool MSTMethodInterface::_CheckAndInit()
{
    //1.Check the parameters
    if (pData->_threshold<=0)
    {
        return false;
    }

    if (pData->_minObjectSize <= 0)
    {
        return false;
    }



    //2.Check Input Image
    GDALDataset* poSrcDS = ( GDALDataset*)GDALOpen(inputImagePath.toUtf8().constData(),GA_ReadOnly);
    if (poSrcDS == NULL)
    {
        return false;
    }
    srcDS = poSrcDS;

    if (_layerWeights.size()==0)
    {
        _layerWeights.resize(poSrcDS->GetRasterCount());
        //        std::for_each(_layerWeights.begin(),_layerWeights.end(),[&](double &data)
        //        {
        //            data = 1./poSrcDS->GetRasterCount();
        //        });
        for(QVector<double>::Iterator iter = _layerWeights.begin();iter != _layerWeights.end();++iter)
        {
            *iter = 1./poSrcDS->GetRasterCount();
        }
    }

    //3.Init Output Image
    GDALDriver* poDriver = (GDALDriver*)GDALGetDriverByName("GTiff");
    GDALDataset* poDstDS = poDriver->Create(markfilePath.toUtf8().constData(),poSrcDS->GetRasterXSize(),poSrcDS->GetRasterYSize(),1,GDT_Int32,NULL);
    if (poDstDS == NULL)
    {
        return false;
    }
    dstDS = poDstDS;

    poDstDS->CreateMaskBand(GMF_PER_DATASET);
    GDALRasterBand* poBand = poDstDS->GetRasterBand(1)->GetMaskBand();
    for (int i=0;i<poDstDS->GetRasterYSize();++i)
    {
        std::vector<uchar> mask(poDstDS->GetRasterXSize(),1);
        if (poSrcDS->GetRasterBand(1)->GetRasterDataType() == GDT_Byte && pData->shield_0_255)
        {
            uchar* image = new uchar[mask.size()];
            for (int k=0;k<poSrcDS->GetRasterCount();++k)
            {
                poSrcDS->GetRasterBand(k+1)->RasterIO(GF_Read,0,i,mask.size(),1,image,mask.size(),1,GDT_Byte,0,0);
                for (int j=0;j<mask.size();++j)
                {
                    if (image[j]==0 || image[j]==255)
                        mask[j]=0;
                }
            }
            delete []image;
        }

        poBand->RasterIO(GF_Write, 0,i,mask.size(),1,&mask[0],mask.size(),1,GDT_Byte,0,0);
    }

    double adfGeoTransform[6];
    poDstDS->SetProjection(poSrcDS->GetProjectionRef());
    poSrcDS->GetGeoTransform(adfGeoTransform);
    poDstDS->SetGeoTransform(adfGeoTransform);
    return true;
}

void MSTMethodInterface::onComputeEdgeWeight(unsigned nodeID1, unsigned nodeID2, const QVector<double> &data1, const QVector<double> &data2, const QVector<double> &layerWeight, void *p)
{
    EdgeVector* vecEdge = (EdgeVector*)p;
    double  difs=0;
    for (int k=0;k<data1.size();++k)
    {
        difs += ((data1[k]-data2[k])*layerWeight[k])*((data1[k]-data2[k])*layerWeight[k]);
    }
    vecEdge->push(edge(nodeID1,nodeID2,(float)sqrt(difs)));
}

bool MSTMethodInterface::_CreateEdgeWeights(void *p)
{
    EdgeVector* vecEdge = (EdgeVector*)p;
    vecEdge->clear();

    GDALDataset* poSrcDS = (GDALDataset*)srcDS;
    GDALDataset* poDstDS = (GDALDataset*)dstDS;
    unsigned nBandCount = poSrcDS->GetRasterCount();
    unsigned width = poSrcDS->GetRasterXSize();
    unsigned height= poSrcDS->GetRasterYSize();
    GDALDataType gdalDataType = poSrcDS->GetRasterBand(1)->GetRasterDataType();
    unsigned pixelSize = GDALGetDataTypeSize(gdalDataType)/8;

    QVector<double> buffer1(nBandCount);
    QVector<double> buffer2(nBandCount);

    std::vector<unsigned char*> lineBufferUp(nBandCount);
    std::vector<unsigned char*> lineBufferDown(nBandCount);

    GDALRasterBand* poMask = poDstDS->GetRasterBand(1)->GetMaskBand();
    uchar* maskUp   = new uchar[width];
    uchar* maskDown = new uchar[width];
    for (unsigned k=0;k<nBandCount;++k)
    {
        lineBufferUp[k] = new unsigned char[width*pixelSize];
        lineBufferDown[k] = new unsigned char[width*pixelSize];
    }

    for (unsigned k=0;k<nBandCount;++k)
    {
        poSrcDS->GetRasterBand(k+1)->RasterIO(GF_Read,0,0,width,1,lineBufferUp[k],width,1,gdalDataType,0,0);
    }
    poMask->RasterIO(GF_Read, 0,0,width,1,maskUp,width,1,GDT_Byte,0,0);


    emit progressBarSizeChanged(0,height-1);
    const int progressGap = (height-1)/100;


    for (unsigned y=0;y<height-1;++y)
    {
        for (unsigned k=0;k<nBandCount;++k)
            poSrcDS->GetRasterBand(k+1)->RasterIO(GF_Read,0,y+1,width,1,lineBufferDown[k],width,1,gdalDataType,0,0);
        poMask->RasterIO(GF_Read, 0,y+1,width,1,maskDown,width,1,GDT_Byte,0,0);

        for(unsigned x=0;x<width;++x)
        {
            if (maskUp[x]==0) continue;

            unsigned nodeID1 = y*width+x;
            unsigned nodeIDNextLIne = nodeID1+width;
            if (x < width-1 )
            {
                if (maskUp[x+1]!=0)
                {
                    for(unsigned k=0;k<nBandCount;++k)
                    {
                        buffer1[k] = SRCVAL(lineBufferUp[k],gdalDataType,x);
                        buffer2[k] = SRCVAL(lineBufferUp[k],gdalDataType,x+1);
                    }
                    //                    emit computeEdgeWeight(nodeID1,nodeID1+1,buffer1,buffer2,_layerWeights,vecEdge);
                    //                    ++edgeSize;
                    onComputeEdgeWeight(nodeID1,nodeID1+1,buffer1,buffer2,_layerWeights,vecEdge);
                }
            }

            if(y < height-1)
            {
                if (maskDown[x]!=0)
                {
                    for(unsigned k=0;k<nBandCount;++k)
                    {
                        buffer1[k] = SRCVAL(lineBufferUp[k],gdalDataType,x);
                        buffer2[k] = SRCVAL(lineBufferDown[k],gdalDataType,x);
                    }
                    //                    emit computeEdgeWeight(nodeID1,nodeID1+1,buffer1,buffer2,_layerWeights,vecEdge);
                    //                    ++edgeSize;
                    onComputeEdgeWeight(nodeID1,nodeIDNextLIne,buffer1,buffer2,_layerWeights,vecEdge);
                }
            }
        }

        std::vector<unsigned char*> tempBuffer = lineBufferDown;
        lineBufferDown = lineBufferUp;
        lineBufferUp = tempBuffer;

        uchar* p    = maskUp;
        maskUp      = maskDown;
        maskDown    = p;

        //        ++pd;
        if (y%progressGap==0)
            emit progressBarValueChanged(y);

    }


    emit progressBarValueChanged(height-1);

    for (unsigned k=0;k<nBandCount;++k)
    {
        delete [](lineBufferUp[k]);
        delete [](lineBufferDown[k]);
    }
    delete []maskUp;
    delete []maskDown;




    return true;
}

bool MSTMethodInterface::_ObjectMerge(GraphKruskal *&graph,
                                void *p,
                                unsigned num_vertices,
                                double threshold)
{
    EdgeVector* vecEdge = (EdgeVector*)p;
    graph = new GraphKruskal(num_vertices);

    //    boost::progress_display pd(vecEdge.size(),std::cout,"merging\n","","");
    //    for (unsigned k = 0; k < vecEdge.size(); ++k)
    int barSize = vecEdge->size();
    emit progressBarSizeChanged(0,barSize);
    int i=0;
    const int progressGap = barSize/100;

    while(!vecEdge->empty())
    {
        edge edge_temp = *(*vecEdge);
        unsigned a = graph->find(edge_temp.GetNode1());
        unsigned b = graph->find(edge_temp.GetNode2());

        int nPredict = 0;
        if ((a != b) && (graph->joinPredicate_sw(a,b,(float)threshold,edge_temp.GetWeight(),nPredict)==true))
        {
            graph->join_band_sw(a,b,edge_temp.GetWeight());
            graph->find(a);
        }
        ++(*vecEdge);
        if (i%progressGap==0)
            emit progressBarValueChanged(i);
        ++i;
        //        ++pd;
    }
    emit progressBarValueChanged(barSize);
    return true;
}

bool MSTMethodInterface::_EliminateSmallArea(GraphKruskal * &graph,
                                       void *p,
                                       double _minObjectSize)
{
    EdgeVector* vecEdge = (EdgeVector*)p;
    vecEdge->rewind();
    int barSize = vecEdge->size();
    int i=0;
    const int progressGap = barSize/100;
    emit progressBarSizeChanged(0,barSize);
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
            graph->find(a);
        }
        ++(*vecEdge);
        if (i%progressGap==0)
            emit progressBarValueChanged(i);
        //        ++pd;
        ++i;
    }
    emit progressBarValueChanged(barSize);

    return true;
}

bool MSTMethodInterface::_GenerateFlagImage(GraphKruskal *&graph,const QMap<unsigned, unsigned> &mapRootidObjectid)
{
    GDALDataset* poSrcDS = (GDALDataset*)srcDS;
    GDALDataset* poDstDS = (GDALDataset*)dstDS;

    unsigned width = poSrcDS->GetRasterXSize();
    unsigned height= poSrcDS->GetRasterYSize();

    //    boost::progress_display pd(height,std::cout,"generating result\n","","");

    const int progressGap = height/100;
    emit progressBarSizeChanged(0,height);
    GDALRasterBand* poFlagBand = poDstDS->GetRasterBand(1);
    GDALRasterBand* poMaskBand = poFlagBand->GetMaskBand();
    std::vector<uchar> mask(width);

    for(unsigned i=0,index =0;i<height;++i)
    {
        poMaskBand->RasterIO(GF_Read,0,i,width,1,&mask[0],width,1,GDT_Byte,0,0);
        for(unsigned j=0;j<width;++j,++index)
        {
            int objectID = 0;
            if (mask[j]!=0)
            {
                int root = graph->find(index);
                objectID = mapRootidObjectid.value(root);
            }
            poFlagBand->RasterIO(GF_Write,j,i,1,1,(int *)&objectID,1,1,GDT_Int32,0,0);
        }
        if (i%progressGap == 0)
            emit progressBarValueChanged(i);
        //        ++pd;
    }
    emit progressBarValueChanged(height);
    return true;
}

bool MSTMethodInterface::_Polygonize()
{
    emit progressBarSizeChanged(0,0);
    GDALDataset *poFlagDS = (GDALDataset *)GDALOpen(markfilePath.toUtf8().constData(),GA_ReadOnly);
    if (poFlagDS == NULL)
    {
        return false;
    }

    OGRSFDriver* poOgrDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
    if (poOgrDriver == NULL)
    {
        GDALClose(poFlagDS);
        return false;
    }

    QFileInfo info(shapefilePath);
    if (info.exists())
        poOgrDriver->DeleteDataSource(shapefilePath.toUtf8().constData());

    OGRDataSource* poDstDataset = poOgrDriver->CreateDataSource(shapefilePath.toUtf8().constData(),0);
    if (poDstDataset == NULL)
    {
        GDALClose(poFlagDS);
        return false;
    }

    OGRSpatialReference* pSpecialReference = new OGRSpatialReference(poFlagDS->GetProjectionRef());
    const char* layerName = "polygon";
    OGRLayer* poLayer = poDstDataset->CreateLayer(layerName,pSpecialReference,wkbPolygon,0);
    if (poLayer == NULL)
    {
        GDALClose(poFlagDS);
        OGRDataSource::DestroyDataSource( poDstDataset );
        return false;
    }

    OGRFieldDefn ofDef_DN( "GridCode", OFTInteger );
    if ( (poLayer->CreateField(&ofDef_DN) != OGRERR_NONE) )
    {
        GDALClose(poFlagDS);
        OGRDataSource::DestroyDataSource( poDstDataset );
        return false;
    }


    char** papszOptions = NULL;
    papszOptions = CSLSetNameValue(papszOptions,"8CONNECTED","8");
    GDALRasterBand *poFlagBand = poFlagDS->GetRasterBand(1);
    GDALRasterBand *poMaskBand = poFlagBand->GetMaskBand();
    CPLErr err = GDALPolygonize((GDALRasterBandH)poFlagBand,(GDALRasterBandH)poMaskBand,(OGRLayerH)poLayer,0,papszOptions,0,0);
    if (err != CE_None)
    {
        GDALClose(poFlagDS);
        OGRDataSource::DestroyDataSource( poDstDataset );
        if (pSpecialReference) delete pSpecialReference;
        return false;
    }

    if (pSpecialReference) delete pSpecialReference;
    GDALClose(poFlagDS);
    OGRDataSource::DestroyDataSource( poDstDataset );
    return true;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(MSTMethod, MSTMethodInterface)
#endif // QT_VERSION < 0x050000
