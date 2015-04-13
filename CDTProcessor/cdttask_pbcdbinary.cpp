#include "cdttask_pbcdbinary.h"
#include <QtCore>
#include <QtXml>
#include <QtXmlPatterns>
#include <gdal_priv.h>
#include "cdttaskmanager.h"
#include "cdtprocessorapplication.h"
#include "cdtpbcddiffinterface.h"
#include "cdtpbcdmergeinterface.h"
#include "cdtautothresholdinterface.h"
#include "messagehandler.h"

extern QList<CDTPBCDDiffInterface *>       pbcdDiffPlugins;
extern QList<CDTPBCDMergeInterface *>      pbcdMergePlugins;
extern QList<CDTAutoThresholdInterface *>  autoThresholdPlugins;

CDTTask_PBCDBinary::CDTTask_PBCDBinary(QString id,QDomDocument params, QObject *parent) :
    CDTTask(id,params,parent),poT1DS(NULL),poT2DS(NULL),diffPlugin(NULL),
    mergePlugin(NULL),autoThresholdPlugin(NULL),isDoubleThreshold(false)
{
    GDALAllRegister();
}

void CDTTask_PBCDBinary::start()
{
    //1.Check Params and init
    if (validateParams()==false)
    {
        error(tr("XML file validate failed!"));
        return;
    }


    QString errorText = readParams();
    if (!errorText.isEmpty())
    {
        error(errorText);
        return;
    }

    int nXSize = poT1DS->GetRasterXSize();
    int nYSize = poT1DS->GetRasterYSize();
    if (nXSize != poT2DS->GetRasterXSize() || nYSize != poT2DS->GetRasterYSize() )
    {
        error(tr("Two images have different size"));
        return;
    }

    GDALDataType dataType = poT1DS->GetRasterBand(1)->GetRasterDataType();
    if (dataType != poT2DS->GetRasterBand(1)->GetRasterDataType())
    {
        error(tr("Two images have different data type"));
        return;
    }
//    int dataSize = GDALGetDataTypeSize(dataType)/8;

//    int bandCount1 = poT1DS->GetRasterCount();
//    int bandCount2 = poT2DS->GetRasterCount();


    //2.Create Diff Image
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
    if (poDriver == NULL)
    {
        error(tr("No GeoTiff Driver!"));
        return;
    }

    QString diffPath = qApp->getTempFileName(".tif");
    GDALDataset *poDiffDS = (GDALDataset *)(poDriver->Create(diffPath.toUtf8().constData(),
                                                             nXSize,nYSize,bandPairs.size(),GDT_Float32,NULL));
    if (poDiffDS == NULL)
    {
        error(tr("Create diff image failed!"));
        return;
    }
    double geoTransform[6];
    poT1DS->GetGeoTransform(geoTransform);
    poDiffDS->SetGeoTransform(geoTransform);
    poDiffDS->SetProjection(poT1DS->GetProjectionRef());

    //3.Generate diff images;
    GDALDataset *poT1AveDS = NULL;
    GDALDataset *poT2AveDS = NULL;
    QList<QPair<GDALRasterBand*,GDALRasterBand*> > poBands;
    for (int i=0;i<bandPairs.size();++i)
    {
        GDALRasterBand *poBand1,*poBand2;
        QPair<uint,uint> pair = bandPairs.value(i);
        uint bandID1 = pair.first;
        uint bandID2 = pair.second;
        if (bandID1 == 0)
        {
            if (poT1AveDS == NULL)
                poT1AveDS = generateAveImage(poDriver,poT1DS);
            poBand1 = poT1AveDS->GetRasterBand(1);
        }
        else
            poBand1 = poT1DS->GetRasterBand(bandID1);
        if (bandID2 == 0)
        {
            if (poT2AveDS == NULL)
                poT2AveDS = generateAveImage(poDriver,poT2DS);
            poBand2 = poT2AveDS->GetRasterBand(1);
        }
        else
            poBand2 = poT2DS->GetRasterBand(bandID2);
        poBands.append(qMakePair(poBand1,poBand2));
    }

    errorText = diffPlugin->generateDiffImage(poBands,poDiffDS);
    if (!errorText.isEmpty())
    {
        error(errorText);
        return;
    }    

    //4.Merge
    GDALDataset *poMergeDS = NULL;
    if (isDoubleThreshold)
    {
        poMergeDS = poDiffDS;
    }
    else
    {
        diffPath = qApp->getTempFileName(".tif");
        qDebug()<<"mergedPath:"<<diffPath;
        poMergeDS = (GDALDataset *)(poDriver->Create(diffPath.toUtf8().constData(),
                                                                 nXSize,nYSize,1,GDT_Float32,NULL));
        if (poMergeDS == NULL)
        {
            error(tr("Create merge image failed!"));
            return;
        }

        poMergeDS->SetGeoTransform(geoTransform);
        poMergeDS->SetProjection(poT1DS->GetProjectionRef());

        merge(poDiffDS,poMergeDS);
    }

    //5.Threshold
    if (!autoThreshold.isEmpty())//Auto
    {
        GDALRasterBand *poMergedBand = poMergeDS->GetRasterBand(1);
        double dMinMax[2] = {0.0,255.0};
        poMergedBand->ComputeRasterMinMax(false,dMinMax);
        qDebug()<<"min:"<<dMinMax[0];
        qDebug()<<"max:"<<dMinMax[1];

        if (isDoubleThreshold)
        {
            QVector<int> histogramPositive(256),histogramNegetive(256);
            poMergedBand->GetHistogram(0,dMinMax[1],256,&histogramPositive[0],false,false,NULL,NULL);
            poMergedBand->GetHistogram(dMinMax[0],0,256,&histogramNegetive[0],false,false,NULL,NULL);
            positiveThreshold = autoThresholdPlugin->autoThreshold(histogramPositive)*(dMinMax[1]-0)/256.;
            negetiveThreshold = autoThresholdPlugin->autoThreshold(histogramNegetive)*(0-dMinMax[0])/256.+dMinMax[0];
            qDebug()<<"double threshold:";
            qDebug()<<QString("Min: %1\tMax: %2\tPosetiveThreshold: %3\tNegetiveThreshold: %4")
                      .arg(dMinMax[0]).arg(dMinMax[1]).arg(positiveThreshold).arg(negetiveThreshold);
        }
        else
        {
            QVector<int> histogram(256);
            poMergedBand->GetHistogram(dMinMax[0],dMinMax[1],256,&histogram[0],false,false,NULL,NULL);
            positiveThreshold = autoThresholdPlugin->autoThreshold(histogram)*(dMinMax[1]-dMinMax[0])/256.+dMinMax[0];
            qDebug()<<"single threshold:";
            qDebug()<<QString("Min: %1\tMax: %2\tThreshold: %3").arg(dMinMax[0]).arg(dMinMax[1]).arg(positiveThreshold);
        }
    }

    //6.Close

    if (poDiffDS)GDALClose(poDiffDS);
    if (!isDoubleThreshold)GDALClose(poMergeDS);

    QByteArray result;
    QDataStream out(&result,QFile::WriteOnly);

    QVariantMap params;
    params.insert("positiveThreshold",positiveThreshold);
    params.insert("negetiveThreshold",negetiveThreshold);
    params.insert("diffPath",diffPath);
    out<<params;

    emit taskCompleted(id,result);


//    qApp->returnDebugMessage("start");
//    info.status = CDTTaskInfo::PROCESSING;
//    for(int i=0;i<100;i+=1)
//    {
//        info.currentProgress = i;
//        info.totalProgress = i;
//        emit taskInfoUpdated(id,info);
//    }

//    info.currentProgress = info.totalProgress = 100;
//    info.status = CDTTaskInfo::COMPLETED;
//    emit taskInfoUpdated(id,info);
}

bool CDTTask_PBCDBinary::validateParams()
{
    QFile fileSchema(":/xsd/PBCD_Binary.xsd");
    fileSchema.open(QFile::ReadOnly);
    const QByteArray schemaData = fileSchema.readAll();
    const QByteArray instanceData = params.toString(4).toUtf8();

    MessageHandler messageHandler;
    QXmlSchema schema;
    schema.setMessageHandler(&messageHandler);
    schema.load(schemaData);

    bool errorOccurred = false;
    if (!schema.isValid()) {
        errorOccurred = true;
    } else {
        QXmlSchemaValidator validator(schema);
        if (!validator.validate(instanceData))
            errorOccurred = true;
    }

    return !errorOccurred;
}

QString CDTTask_PBCDBinary::readParams()
{
    QDomElement params_root = params.firstChildElement().firstChildElement().firstChildElement();

    //1.images
    QDomElement images = params_root.firstChildElement("images");
    QString t1Path = images.firstChildElement("t1").attribute("path");
    QString t2Path = images.firstChildElement("t2").attribute("path");
    if (t1Path.isEmpty() || t2Path.isEmpty())
        return tr("Fetch images' path failed!");

    poT1DS = (GDALDataset*)GDALOpen(t1Path.toUtf8().constData(),GA_ReadOnly);
    poT2DS = (GDALDataset*)GDALOpen(t2Path.toUtf8().constData(),GA_ReadOnly);
    if (poT1DS == NULL || poT2DS==NULL)
        return tr("Open images failed!");

    //2.bands
    QDomElement bands = params_root.firstChildElement("bands");
    bandPairs.clear();
    QDomElement bandPair = bands.firstChildElement("band_pair");
    while(!bandPair.isNull())
    {
        QStringList pair = bandPair.text().split("->");
        uint t1band = 0,t2band = 0;
        if (pair[0]!="ave")
            t1band = pair[0].right(1).toInt();
        if (pair[1]!="ave")
            t2band = pair[1].right(1).toInt();
        bandPairs.append(qMakePair(t1band,t2band));//ave-0

        bandPair = bandPair.nextSiblingElement("band_pair");
    }
    isDoubleThreshold = bandPairs.count()<=1;

    //3.radiometric_correction
    QDomElement radio = params_root.firstChildElement("radiometric_correction");
    if (radio.attribute("valid").toLower()=="false")
    {
        radiometricCorrectionMethod = QString::null;
    }
    else
    {
        radiometricCorrectionMethod = radio.text();
    }

    //4.diff_method
    QDomElement diff = params_root.firstChildElement("diff_method");
    diffMethod = diff.attribute("name");
    foreach (CDTPBCDDiffInterface *plugin, pbcdDiffPlugins) {
        if (plugin->methodName()==diffMethod)
        {
            diffPlugin = plugin;
        }
    }
    if (diffPlugin == NULL)
        return tr("Plugin named %1 is not found").arg(diffMethod);

    //5.merge_method
    if (!isDoubleThreshold)
    {
        QDomElement merge = params_root.firstChildElement("merge_method");
        mergeMethod = merge.attribute("name");
        if (!mergeMethod.isEmpty())
        {
            foreach (CDTPBCDMergeInterface *plugin, pbcdMergePlugins) {
                if (plugin->methodName()==mergeMethod)
                {
                    mergePlugin = plugin;
                }
            }
            if (mergePlugin == NULL)
                return tr("Plugin named %1 is not found").arg(mergeMethod);
        }
    }

    //6.threshold
    QDomElement threshold = params_root.firstChildElement("threshold");
    if (threshold.attribute("type").toLower()=="auto")
    {
        autoThreshold = threshold.text();
        foreach (CDTAutoThresholdInterface *plugin, autoThresholdPlugins) {
            if (plugin->methodName()==autoThreshold)
            {
                autoThresholdPlugin = plugin;
            }
        }
        if (autoThresholdPlugin == NULL)
            return tr("Plugin named %1 is not found").arg(autoThreshold);
    }
    else
    {
        autoThreshold = QString::null;
        if (isDoubleThreshold)
        {
            QStringList thres = threshold.text().split(";");
            double t1 = thres[0].toDouble();
            double t2 = thres[1].toDouble();
            positiveThreshold = qMax(t1,t2);
            negetiveThreshold = qMin(t1,t2);
        }
        else
        {
            positiveThreshold = threshold.text().toDouble();
        }
    }
    return QString::null;
}

GDALDataset *CDTTask_PBCDBinary::generateAveImage(GDALDriver *poDriver,GDALDataset *poSrcDS)
{
    int nXSize = poSrcDS->GetRasterXSize();
    int nYSize = poSrcDS->GetRasterYSize();
    GDALDataType dataType = poT1DS->GetRasterBand(1)->GetRasterDataType();
    int dataSize = GDALGetDataTypeSize(dataType)/8;

    GDALDataset *poAveDS = (GDALDataset *)(poDriver->Create(qApp->getTempFileName(".tif").toUtf8().constData(),
                                                             nXSize,nYSize,1,GDT_Float32,NULL));
    if (poAveDS == NULL)
    {
        error(tr("Create average image failed!"));
        return NULL;
    }

    uchar* buffer = new uchar[dataSize];
    for (int i=0;i<nYSize;++i)
    {
        for (int j=0;j<nXSize;++j)
        {
            float ave = 0;
            for (int k=0;k<poSrcDS->GetRasterCount();++k)
            {
                poSrcDS->GetRasterBand(k+1)->RasterIO(GF_Read,j,i,1,1,buffer,1,1,dataType,0,0);
                ave += SRCVAL(buffer,dataType,0);
            }
            ave /= poSrcDS->GetRasterCount();
            poAveDS->GetRasterBand(1)->RasterIO(GF_Write,j,i,1,1,&ave,1,1,GDT_Float32,0,0);
        }
    }
    delete []buffer;
    return poAveDS;
}

void CDTTask_PBCDBinary::merge(GDALDataset *poDS,GDALDataset *poMerged)
{
    int nXSize = poDS->GetRasterXSize();
    int nYSize = poDS->GetRasterYSize();
    int bandCount = poDS->GetRasterCount();

    QVector<float> buffer(bandCount);
    for (int i=0;i<nYSize;++i)
    {
        for(int j=0;j<nXSize;++j)
        {
            poDS->RasterIO(GF_Read,j,i,1,1,&buffer[0],1,1,GDT_Float32,bandCount,NULL,0,0,0);
            float result = mergePlugin->merge(buffer);
            poMerged->GetRasterBand(1)->RasterIO(GF_Write,j,i,1,1,&result,1,1,GDT_Float32,0,0);
        }
    }
}

