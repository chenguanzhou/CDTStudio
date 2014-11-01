#include "polygonizer.h"
#include <QtCore>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <ogr_api.h>
#include <gdal_alg_priv.h>

Polygonizer::Polygonizer(QString flagPath,
                         QString shapefilePath,
                         QObject *parent) :
    CDTBaseThread(parent),
    _flagPath(flagPath),
    _shapefilePath(shapefilePath)
{
}

void Polygonizer::run()
{
    GDALAllRegister();
    OGRRegisterAll();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","YES");

    emit currentProgressChanged(tr("Polygonizing"));
    emit progressBarSizeChanged(0,100);

    GDALDataset *poFlagDS = (GDALDataset *)GDALOpen(_flagPath.toUtf8().constData(),GA_ReadOnly);
    if (poFlagDS == NULL)
    {
        emit showWarningMessage(tr("Open Flag Image ")+_flagPath+tr(" Failed!"));
        return;
    }

    OGRSFDriver* poOgrDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
    if (poOgrDriver == NULL)
    {
        emit showWarningMessage(tr("Get ESRI SHAPEFILE Driver Failed"));
        GDALClose(poFlagDS);
        return;
    }

    QFileInfo info(_shapefilePath);
    if (info.exists())
        poOgrDriver->DeleteDataSource(_shapefilePath.toUtf8().constData());

    OGRDataSource* poDstDataset = poOgrDriver->CreateDataSource(_shapefilePath.toUtf8().constData(),0);
    if (poDstDataset == NULL)
    {
        emit showWarningMessage(tr("Create Shapefile ")+_shapefilePath+tr(" Failed!"));
        GDALClose(poFlagDS);
        return;
    }

    OGRSpatialReference* pSpecialReference = new OGRSpatialReference(poFlagDS->GetProjectionRef());
    const char* layerName = "polygon";
    OGRLayer* poLayer = poDstDataset->CreateLayer(layerName,pSpecialReference,wkbPolygon,0);
    if (poLayer == NULL)
    {
        emit showWarningMessage(tr("Create Layer Failed!"));
        GDALClose(poFlagDS);
        OGRDataSource::DestroyDataSource( poDstDataset );
//        OGRCleanupAll();
        return;
    }

    OGRFieldDefn ofDef_DN( "GridCode", OFTInteger );
    if ( (poLayer->CreateField(&ofDef_DN) != OGRERR_NONE) )
    {
        emit showWarningMessage(tr("Create Field Failed!"));
        GDALClose(poFlagDS);
        OGRDataSource::DestroyDataSource( poDstDataset );
//        OGRCleanupAll();
        return;
    }

    emit progressBarValueChanged(20);
    char** papszOptions = NULL;
    papszOptions = CSLSetNameValue(papszOptions,"8CONNECTED","8");
    GDALRasterBand *poFlagBand = poFlagDS->GetRasterBand(1);
    GDALRasterBand *poMaskBand = poFlagBand->GetMaskBand();
    CPLErr err = GDALPolygonize((GDALRasterBandH)poFlagBand,(GDALRasterBandH)poMaskBand,(OGRLayerH)poLayer,0,papszOptions,0,0);
    if (err != CE_None)
    {
        emit showWarningMessage(tr("Polygonization failed!"));
        GDALClose(poFlagDS);
        OGRDataSource::DestroyDataSource( poDstDataset );
        if (pSpecialReference) delete pSpecialReference;
//        OGRCleanupAll();
        return;
    }

    emit progressBarValueChanged(100);
    emit showNormalMessage(tr("Polygonization secceed!"));
    if (pSpecialReference) delete pSpecialReference;
    GDALClose(poFlagDS);
    OGRDataSource::DestroyDataSource( poDstDataset );
//    OGRCleanupAll();

    return;
}


