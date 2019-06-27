#include "slicinterface.h"
#include <exception>
#include <algorithm>
#include <iostream>
#include <QtCore>
#include <gdal_priv.h>
#include <gdal_alg.h>
#include <ogrsf_frmts.h>
#include <ogr_api.h>
#include <opencv2/opencv.hpp>

#include "slic.h"
#include "gdal2opencv.h"

class SLICMethodPrivate
{
public:
    SLICMethodPrivate():_objectCount(10),_compactness(30),_algorith(SLICInterface::SLIC){}
    double _objectCount;
    int _compactness;
    SLICInterface::Algorithm _algorith;
};

SLICInterface::SLICInterface(QObject *parent)
    :CDTSegmentationInterface(parent),
      pData(new SLICMethodPrivate)
{
    GDALAllRegister();
}

SLICInterface::~SLICInterface()
{
}

QString SLICInterface::segmentationMethod() const
{
    return QString("SLIC");
}

void SLICInterface::startSegmentation()
{
    //Add Georefference
    GDALDataset *poSrcDS = (GDALDataset *)GDALOpen(inputImagePath.toUtf8().constData(),GA_ReadOnly);
    auto src = GDAL2OpenCV::ds2mat(poSrcDS);

    //Generate SLIC
//    auto src = cv::imread(inputImagePath.toLocal8Bit().constData());
    auto slic = cv::ximgproc::createSuperpixelSLIC(src,pData->_algorith,pData->_objectCount,pData->_compactness);
    slic->iterate();
    cv::Mat labels;
    slic->getLabels(labels);

    GDALDataset *poMarkDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(markfilePath.toUtf8().constData(),labels.cols,labels.rows,1,GDT_Int32,Q_NULLPTR);
    double adfGeoTransform[6];
    poSrcDS->GetGeoTransform(adfGeoTransform);
    poMarkDS->SetGeoTransform(adfGeoTransform);
    poMarkDS->SetProjection(poSrcDS->GetProjectionRef());
    GDALClose(poSrcDS);

    GDALRasterBand *poFlagBand = poMarkDS->GetRasterBand(1);
    for (int i=0;i<labels.rows;++i)
    {
        poFlagBand->RasterIO(GF_Write,0,i,labels.cols,1,labels.ptr<int>(i),labels.cols,1,GDT_Int32,0,0);
    }



    //Polygonlization
    GDALDriver * poOgrDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
    if (poOgrDriver == Q_NULLPTR)
    {
        GDALClose(poMarkDS);
        return;
    }
    QFileInfo info(shapefilePath);
    if (info.exists())
        poOgrDriver->Delete(shapefilePath.toUtf8().constData());
    GDALDataset* poDstDS = poOgrDriver->Create(shapefilePath.toUtf8().constData(),0,0,0,GDT_Unknown,Q_NULLPTR);
    if (poDstDS == Q_NULLPTR)
    {
        GDALClose(poMarkDS);
        return;
    }

//    OGRSpatialReference* pSpecialReference = new OGRSpatialReference(poMarkDS->GetProjectionRef());
    OGRSpatialReference pSpecialReference;
    pSpecialReference.SetProjection(poMarkDS->GetProjectionRef());

    const char* layerName = "polygon";
    OGRLayer* poLayer = poDstDS->CreateLayer(layerName,&pSpecialReference,wkbPolygon,Q_NULLPTR);
    if (poLayer == Q_NULLPTR)
    {
        GDALClose(poMarkDS);
        GDALClose( poDstDS );
        return;
    }

    OGRFieldDefn ofDef_DN( "GridCode", OFTInteger );
    if ( (poLayer->CreateField(&ofDef_DN) != OGRERR_NONE) )
    {
        GDALClose(poMarkDS);
        GDALClose( poDstDS );
        return;
    }


    char** papszOptions = Q_NULLPTR;
    papszOptions = CSLSetNameValue(papszOptions,"8CONNECTED","8");
    GDALRasterBand *poMaskBand = poFlagBand->GetMaskBand();
    CPLErr err = GDALPolygonize((GDALRasterBandH)poFlagBand,(GDALRasterBandH)poMaskBand,(OGRLayerH)poLayer,0,papszOptions,Q_NULLPTR,Q_NULLPTR);
    if (err != CE_None)
    {
        GDALClose(poMarkDS);
        GDALClose( poDstDS );
        return;
    }

    GDALClose(poMarkDS);
    GDALClose(poDstDS);
}

double SLICInterface::objectCount() const
{
    return pData->_objectCount;
}

int SLICInterface::compactness() const
{
    return pData->_compactness;
}

SLICInterface::Algorithm SLICInterface::algorith() const
{
    return pData->_algorith;
}

void SLICInterface::setObjectCount(double val)
{
    pData->_objectCount = val;
}

void SLICInterface::setCompactness(int val)
{
    pData->_compactness = val;
}

void SLICInterface::setAlgorith(SLICInterface::Algorithm val)
{
    pData->_algorith = val;
}




#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(SLIC, SLICInterface)
#endif // QT_VERSION < 0x050000
