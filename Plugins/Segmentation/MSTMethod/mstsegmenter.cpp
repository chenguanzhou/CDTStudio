#include "mstsegmenter.h"


MSTSegmenter::MSTSegmenter(QObject *parent) :
    CDTSegmentationInterface(parent),
    formParams(new FormParams)
{
}

MSTSegmenter::~MSTSegmenter()
{    
    clear();
    delete formParams;
}

QString MSTSegmenter::segmentationMethod() const
{
    return QString("mst");
}

QWidget *MSTSegmenter::paramsInterface(QWidget *parent) const
{
    return NULL;
}

void MSTSegmenter::run()
{
    if (initialize() == false)
    {
        this->terminate();
        return;
    }

    GDALDataset* poSrcDS = (GDALDataset*)srcDS;
    GDALDataset* poDstDS = (GDALDataset*)dstDS;

    unsigned memory_to_use = 500 * 1024 * 1024;
    EdgeVector *vecEdge = new EdgeVector(edge_comparator(),memory_to_use);

    //    std::vector<edge> vecEdge;
    emit currentProgressChanged(tr("Create edges of graph"));
    _CreateEdgeWeights(vecEdge);

    //    stxxl::sort(vecEdge.begin(),vecEdge.end(),edge_comparator(),memory_to_use);
    emit currentProgressChanged(tr("Sort the edges"));

    vecEdge->sort();

    GraphKruskal* graph = NULL;
    emit currentProgressChanged(tr("Merging"));
    _ObjectMerge(graph,vecEdge,poSrcDS->GetRasterXSize()*poSrcDS->GetRasterYSize(),_threshold);
    emit currentProgressChanged(tr("Eliminate small object"));
    _EliminateSmallArea(graph,vecEdge,_minObjectSize);

    vecEdge->clear();
    delete vecEdge;

    emit currentProgressChanged(tr("Generating index"));
    std::map<unsigned,unsigned> mapRootidObjectid;

    GDALRasterBand* poMaskBand = poDstDS->GetRasterBand(1)->GetMaskBand();
    graph->GetMapNodeidObjectid(poMaskBand, mapRootidObjectid);

    emit currentProgressChanged(tr("Generating result"));
    _GenerateFlagImage(graph,mapRootidObjectid);
    delete graph;

    GDALClose(poSrcDS);
    GDALClose(poDstDS);
    Polygonizer* polygonizer = new Polygonizer(_outputImagePath,_shapefilePath);
    connect(polygonizer,SIGNAL(finished()),polygonizer,SLOT(deleteLater()));
    connect(polygonizer,SIGNAL(currentProgressChanged(QString)),this,SIGNAL(currentProgressChanged(QString)));
    connect(polygonizer,SIGNAL(progressBarSizeChanged(int,int)),this,SIGNAL(progressBarSizeChanged(int,int)));
    connect(polygonizer,SIGNAL(showNormalMessage(QString)),this,SIGNAL(showNormalMessage(QString)));
    connect(polygonizer,SIGNAL(showWarningMessage(QString)),this,SIGNAL(showWarningMessage(QString)));
    polygonizer->run();

    emit completed();
}

void MSTSegmenter::clear()
{
    GDALClose(poSrcDS);
    GDALClose(poDstDS);
}

bool MSTSegmenter::initialize()
{    
    clear();

    //1.Check Input Image
    poSrcDS = ( GDALDataset*)GDALOpen(_inputImagePath.toUtf8().constData(),GA_ReadOnly);
    if (poSrcDS == NULL)
    {
        emit showWarningMessage(tr("Open Image ")+_inputImagePath+tr(" Failed!"));
        return false;
    }

    if (layerWeights.size()==0)
    {
        layerWeights.resize(poSrcDS->GetRasterCount());
        std::for_each(layerWeights.begin(),layerWeights.end(),[&](double &data)
        {
            data = 1./poSrcDS->GetRasterCount();
        });
    }

    //2.Init Markfile
    GDALDriver* poDriver = (GDALDriver*)GDALGetDriverByName("GTiff");
    poDstDS = poDriver->Create(_markfilePath.toUtf8().constData(),poSrcDS->GetRasterXSize(),poSrcDS->GetRasterYSize(),1,GDT_Int32,NULL);
    if (poDstDS == NULL)
    {
        emit showWarningMessage(tr("Create Markfile ")+_markfilePath+tr(" Failed!"));
        return false;
    }

    //3.Init mask file in the markfile
    poDstDS->CreateMaskBand(GMF_PER_DATASET);
    GDALRasterBand* poBand = poDstDS->GetRasterBand(1)->GetMaskBand();
    for (int i=0;i<poDstDS->GetRasterYSize();++i)
    {
        std::vector<uchar> mask(poDstDS->GetRasterXSize(),1);
        if (poSrcDS->GetRasterBand(1)->GetRasterDataType() == GDT_Byte && shield_0_255)
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

    //4.GeoTransform
    double adfGeoTransform[6];
    poDstDS->SetProjection(poSrcDS->GetProjectionRef());
    poSrcDS->GetGeoTransform(adfGeoTransform);
    poDstDS->SetGeoTransform(adfGeoTransform);
    return true;
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(MSTMethod, MSTSegmenter)
#endif // QT_VERSION < 0x050000
