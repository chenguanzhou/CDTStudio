#include "cdtdecomposeobjecthelper.h"
#include <gdal_priv.h>
#include <QtCore>

class CDTDecomposeObjectHelperPrivate{
    CDTDecomposeObjectHelperPrivate(QString directory)
        :isValid(false),dir(directory),poImageDS(NULL),poFlagDS(NULL)
    {
    }
    friend class CDTDecomposeObjectHelper;
    bool            isValid;
    GDALDataset*    poImageDS;
    GDALDataset*    poFlagDS;
    QString dir;
};

class ObjectExtentInfo{
public:
    int min_x,min_y,max_x,max_y;
};

CDTDecomposeObjectHelper::CDTDecomposeObjectHelper(QString imagePath,
        QString markfilePath,
        QString dir,
        bool withMask,
        QObject *parent)
    : p(new CDTDecomposeObjectHelperPrivate(dir)), CDTBaseThread(parent)
{
    this->withMask = withMask;
    GDALAllRegister();
    OGRRegisterAll();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","YES");

    p->poImageDS = (GDALDataset*)GDALOpen(imagePath.toUtf8().constData(),GA_ReadOnly);
    if (p->poImageDS == NULL)
        return;

    p->poFlagDS = (GDALDataset*)GDALOpen(markfilePath.toUtf8().constData(),GA_ReadOnly);
    if (p->poFlagDS == NULL)
        return;


    p->isValid = true;
}

CDTDecomposeObjectHelper::~CDTDecomposeObjectHelper()
{
    GDALClose(p->poImageDS);
    GDALClose(p->poFlagDS);
}

bool CDTDecomposeObjectHelper::isValid()
{
    return p->isValid;
}

void CDTDecomposeObjectHelper::run()
{
    int width      = p->poImageDS->GetRasterXSize();
    int height     = p->poImageDS->GetRasterYSize();
    int bandCount  = p->poImageDS->GetRasterCount();
    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");


    //1. Get objects' extent
    auto flagBand = p->poFlagDS->GetRasterBand(1);
    QMap<int, ObjectExtentInfo> objectExtentInfoMap;
    QVector<int> buffer(width);

    emit progressBarSizeChanged(0,height);
    for (int i=0;i<height;++i)
    {
        flagBand->RasterIO(GF_Read,0,i,width,1,&buffer[0],width,1,GDT_Int32,0,0);
        for (int j=0;j<width;++j)
        {
            int objID = buffer[j];
            if (objectExtentInfoMap.contains(objID))
            {
                if (objectExtentInfoMap[objID].max_x<j)
                    objectExtentInfoMap[objID].max_x = j;
                if (objectExtentInfoMap[objID].min_x>j)
                    objectExtentInfoMap[objID].min_x = j;
                if (objectExtentInfoMap[objID].max_y<i)
                    objectExtentInfoMap[objID].max_y = i;
                if (objectExtentInfoMap[objID].min_y>i)
                    objectExtentInfoMap[objID].min_y = i;
            }
            else
            {
                ObjectExtentInfo objInfo;
                objInfo.max_x = objInfo.min_x = j;
                objInfo.max_y = objInfo.min_y = i;
                objectExtentInfoMap.insert(objID,objInfo);
            }
        }

        emit progressBarValueChanged(i);
    }
    emit progressBarValueChanged(height);

    //2. Export
    emit progressBarSizeChanged(0,objectExtentInfoMap.size());
    int index = 0;
    foreach (int objID, objectExtentInfoMap.keys()) {
        ObjectExtentInfo objInfo = objectExtentInfoMap[objID];
        int nXOff = objInfo.min_x;
        int nYOff = objInfo.min_y;
        int nXSize = objInfo.max_x-nXOff+1;
        int nYSize = objInfo.max_y-nYOff+1;
        QVector<uchar> bufferSrc(nXSize*nYSize);
        QVector<int> bufferFlag(nXSize*nYSize);

        QString fileName = p->dir + "/" + QString::number(objID) + ".tif";


        if (withMask)
        {
            GDALDataset *poDstDS = poDriver->Create( fileName.toUtf8().constData(), nXSize, nYSize, 1, GDT_Byte, NULL);
            flagBand->RasterIO(GF_Read,nXOff,nYOff,nXSize,nYSize,&bufferFlag[0],nXSize,nYSize,GDT_Int32,0,0);

            for (int i=0;i<bufferSrc.size();++i)
            {
                if (bufferFlag[i] != objID)
                    bufferSrc[i] = 0;
                else
                    bufferSrc[i] = 255;
            }

            poDstDS->GetRasterBand(1)->RasterIO(GF_Write,0,0,nXSize,nYSize,&bufferSrc[0],nXSize,nYSize,GDT_Byte,0,0);

            GDALClose(poDstDS);
        }
        else
        {
            GDALDataset *poDstDS = poDriver->Create( fileName.toUtf8().constData(), nXSize, nYSize, bandCount, GDT_Byte, NULL);
            for (int k=0;k<bandCount;++k)
            {
                p->poImageDS->GetRasterBand(k+1)->RasterIO(GF_Read,nXOff,nYOff,nXSize,nYSize,&bufferSrc[0],nXSize,nYSize,GDT_Byte,0,0);
                poDstDS->GetRasterBand(k+1)->RasterIO(GF_Write,0,0,nXSize,nYSize,&bufferSrc[0],nXSize,nYSize,GDT_Byte,0,0);
            }

//            flagBand->RasterIO(GF_Read,nXOff,nYOff,nXSize,nYSize,&bufferFlag[0],nXSize,nYSize,GDT_Int32,0,0);
//            for (int k=0;k<bandCount;++k)
//            {
//                p->poImageDS->GetRasterBand(k+1)->RasterIO(GF_Read,nXOff,nYOff,nXSize,nYSize,&bufferSrc[0],nXSize,nYSize,GDT_Byte,0,0);


//                for (int i=0;i<bufferSrc.size();++i)
//                {
//                    if (bufferFlag[i] != objID)
//                        bufferSrc[i] = 0;
//                }

//                poDstDS->GetRasterBand(k+1)->RasterIO(GF_Write,0,0,nXSize,nYSize,&bufferSrc[0],nXSize,nYSize,GDT_Byte,0,0);
//            }

            GDALClose(poDstDS);
        }


        emit progressBarValueChanged(index++);
    }
    emit progressBarValueChanged(objectExtentInfoMap.size());
    emit completed();
}
