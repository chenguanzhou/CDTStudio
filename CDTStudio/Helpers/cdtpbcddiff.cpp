#include "cdtpbcddiff.h"
#include <gdal_priv.h>
#include "cdtpbcddiffinterface.h"
#include "cdtpbcdmergeinterface.h"

CDTPBCDDiff::CDTPBCDDiff(const QString &T1Path, const QString &T2Path, QList<QPair<uint, uint> > BandPairs, CDTPBCDDiffInterface *DiffPlugin, CDTPBCDMergeInterface *MergePlugin, QObject *parent)
    :CDTBaseThread(parent),
      t1Path(T1Path),
      t2Path(T2Path),
      bandPairs(BandPairs),
      diffPlugin(DiffPlugin),
      mergePlugin(MergePlugin),
      completed(false)
{
    GDALAllRegister();
}

CDTPBCDDiff::~CDTPBCDDiff()
{

}

void CDTPBCDDiff::run()
{
    //Get temp file
    auto GetTempFile = [](QString suffix){
        return QDir::tempPath() + "/" + QUuid::createUuid().toString() + suffix;
    };

    //A Lambda Functor to Open GDALDataset
    auto OpenGDALDataset = [&](const QString& path)->GDALDataset*{
        GDALDataset* poDS = (GDALDataset*)GDALOpen(path.toUtf8().constData(),GA_ReadOnly);
        if (poDS==NULL)
            throw tr("Open File: %1 Failed!").arg(path);
        return poDS;
    };

    //A Lambda Functor to Create GeoTiff Image GDALDataset
    auto CreateTiffDataset = [&](const QString& path,int width,int height,int bandCount,GDALDataType type)->GDALDataset*{
        GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
        if (poDriver == NULL)
            throw tr("No GeoTiff Driver!");
        char** pszOptions = NULL;
//        pszOptions = CSLSetNameValue(pszOptions,"COMPRESS","DEFLATE");
//        pszOptions = CSLSetNameValue(pszOptions,"PREDICTOR","1");
//        pszOptions = CSLSetNameValue(pszOptions,"ZLEVEL","9");
        GDALDataset *poDS = (GDALDataset *)
                (poDriver->Create(path.toUtf8().constData(),width,height,bandCount,type,pszOptions));
        if (poDS == NULL)
            throw tr("Create diff image: %1 failed!").arg(path);
        return poDS;
    };

    //A Lambda Functor to Get/Create Ave Image
    auto GetAveImage = [&](GDALDataset *&poAveDS,QString avePath,GDALDataset *poSrcDS)->GDALRasterBand*{
        if (poAveDS) //Existed
            return poAveDS->GetRasterBand(1);
        else //Create
        {                        
            const int width = poSrcDS->GetRasterXSize();
            const int height = poSrcDS->GetRasterYSize();
            const int bandCount = poSrcDS->GetRasterCount();
            const GDALDataType dataType = poSrcDS->GetRasterBand(1)->GetRasterDataType();
            const int dataSize = GDALGetDataTypeSize(dataType)/8;
            poAveDS = CreateTiffDataset(avePath.toUtf8().constData(),width,height,1,GDT_Float32);
            GDALRasterBand *poAveBand = poAveDS->GetRasterBand(1);

            int nXBlocks, nYBlocks, nXBlockSize, nYBlockSize;
            int iXBlock, iYBlock;
            poAveBand->GetBlockSize( &nXBlockSize, &nYBlockSize );
            nXBlocks = (width + nXBlockSize - 1) / nXBlockSize;
            nYBlocks = (height + nYBlockSize - 1) / nYBlockSize;
            QVector<float> dataOut(nXBlockSize * nYBlockSize,0);

            for( iYBlock = 0; iYBlock < nYBlocks; ++iYBlock )
            {
                int nYOff = iYBlock*nYBlockSize;

                for( iXBlock = 0; iXBlock < nXBlocks; ++iXBlock )
                {
                    int nXValid, nYValid;
                    if( (iXBlock+1) * nXBlockSize > width )
                        nXValid = width - iXBlock * nXBlockSize;
                    else
                        nXValid = nXBlockSize;
                    if( (iYBlock+1) * nYBlockSize > height )
                        nYValid = height - iYBlock * nYBlockSize;
                    else
                        nYValid = nYBlockSize;

                    int nXOff = iXBlock*nXBlockSize;

                    dataOut.fill(0);
                    QVector<uchar> buffer(nXValid*nYValid*dataSize);
                    for(int k=0;k<bandCount;++k)
                    {
                        poSrcDS->GetRasterBand(k+1)->RasterIO(GF_Read,nXOff,nYOff,nXValid,nYValid,&buffer[0],nXValid,nYValid,dataType,0,0);
                        for( int iY = 0; iY < nYValid; ++iY)
                        {
                            for( int iX = 0; iX < nXValid; ++iX)
                            {
                                dataOut[iY*nXBlockSize+iX] += SRCVAL(&buffer[0],dataType,iY*nXValid+iX);
                            }
                        }
                    }
                    std::for_each(dataOut.begin(),dataOut.end(),[bandCount](float &val){
                        val /= bandCount;
                    });
                    poAveBand->WriteBlock(iXBlock,iYBlock,&dataOut[0]);
                }
            }
            return poAveBand;
        }
    };

    //A Lambda Functor to Merge
    auto Merge = [](GDALDataset *poDS,GDALDataset *poMerged,CDTPBCDMergeInterface *mergePlugin){
        int width = poDS->GetRasterXSize();
        int height = poDS->GetRasterYSize();
        int bandCount = poDS->GetRasterCount();

        GDALRasterBand *poBand = poMerged->GetRasterBand(1);

        QVector<float> buffer(bandCount*width);
        QVector<float> result_buffer(width);
        for (int i=0;i<height;++i)
        {
            poDS->RasterIO(GF_Read,0,i,width,1,&buffer[0],width,1,GDT_Float32,bandCount,NULL,sizeof(float)*bandCount,sizeof(float)*bandCount*width,sizeof(float));
            QVector<float>::const_iterator iter_in = buffer.begin();
            QVector<float>::iterator iter_out = result_buffer.begin();

            for (int j=0;j<width;++j,++iter_out,iter_in += bandCount)
            {
                float result = mergePlugin->merge(iter_in,iter_in + bandCount);
                *iter_out = result;
            }
            poBand->RasterIO(GF_Write,0,i,width,1,&result_buffer[0],width,1,GDT_Float32,0,0);
        }
    };

    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset *poT1DS = NULL;
    GDALDataset *poT2DS = NULL;
    GDALDataset *poDiffDS = NULL;
    GDALDataset *poMergeDS = NULL;
    GDALDataset *poT1AveDS = NULL;
    GDALDataset *poT2AveDS = NULL;

    QString T1AvePath = GetTempFile(".tif");
    QString T2AvePath = GetTempFile(".tif");
    QString diffPath = GetTempFile(".tif");
    QString mergePath = GetTempFile(".tif");

    QTime time;
    time.start();
    try
    {
        QTime t;
        t.start();
        //1. Init
        if (bandPairs.count()==0)
            throw tr("No band pair selected!");

        if (diffPlugin==NULL)
            throw tr("No PBCD Diff plugin found!");

        if (bandPairs.count()>1 && mergePlugin==NULL)
            throw tr("No PBCD Merge plugin found!");

        poT1DS = OpenGDALDataset(t1Path);
        poT2DS = OpenGDALDataset(t2Path);

        const int width = poT1DS->GetRasterXSize();
        const int height = poT1DS->GetRasterYSize();
        if (width != poT2DS->GetRasterXSize() || height != poT2DS->GetRasterYSize() )
            throw tr("Different size of the two images");

        const GDALDataType dataType = poT1DS->GetRasterBand(1)->GetRasterDataType();
        if (dataType != poT2DS->GetRasterBand(1)->GetRasterDataType())
            throw tr("Two images have different data type");

        const char *projectionRef = poT1DS->GetProjectionRef();

        qDebug()<<"Init:"<<t.restart()<<"ms";
        //2. Create Diff Image
        poDiffDS = CreateTiffDataset(diffPath,width,height,bandPairs.count(),GDT_Float32);
        double geoTransform[6];
        poT1DS->GetGeoTransform(geoTransform);
        poDiffDS->SetGeoTransform(geoTransform);
        poDiffDS->SetProjection(projectionRef);

        qDebug()<<"Create Diff Image:"<<t.restart()<<"ms";
        //3. Generate Diff Image
        QList<QPair<GDALRasterBand*,GDALRasterBand*> > poBands;
        for (int i=0;i<bandPairs.size();++i)
        {
            GDALRasterBand *poBand1,*poBand2;
            QPair<uint,uint> pair = bandPairs.value(i);
            uint bandID1 = pair.first;
            uint bandID2 = pair.second;
            if (bandID1 == 0)//Ave
                poBand1 = GetAveImage(poT1AveDS,T1AvePath,poT1DS);
            else
                poBand1 = poT1DS->GetRasterBand(bandID1);
            if (bandID2 == 0)
            {
                poBand2 = GetAveImage(poT2AveDS,T2AvePath,poT2DS);
            }
            else
                poBand2 = poT2DS->GetRasterBand(bandID2);
            poBands.append(qMakePair(poBand1,poBand2));
        }
        qDebug()<<"Get all bands:"<<t.restart()<<"ms";

        QString errorMSG = diffPlugin->generateDiffImage(poBands,poDiffDS);
        if (!errorMSG.isEmpty())
            throw errorMSG;

        qDebug()<<"Generate Diff Image:"<<t.restart()<<"ms";
        //4. Merge
        if (bandPairs.count()<=1)//Double threshold
        {
            poMergeDS = poDiffDS;
            outputPath = diffPath;
        }
        else//Single threshold
        {
            poMergeDS = CreateTiffDataset(mergePath.toUtf8().constData(),width,height,1,GDT_Float32);
            poMergeDS->SetGeoTransform(geoTransform);
            poMergeDS->SetProjection(projectionRef);

            Merge(poDiffDS,poMergeDS,mergePlugin);
            outputPath = mergePath;
        }
        completed = true;

        qDebug()<<"Merge:"<<t.elapsed()<<"ms";
    }
    catch(const QString& msg)
    {
        emit showWarningMessage(msg);
        completed = false;
    }

    qDebug()<<"Totally cost "<<time.elapsed()<<"ms";
    //finally
    if (poT1DS) GDALClose(poT1DS);
    if (poT2DS) GDALClose(poT2DS);
    if (poDiffDS) GDALClose(poDiffDS);
    if (poMergeDS!=poDiffDS && poDiffDS!=NULL) GDALClose(poMergeDS);
    if (poT1AveDS) GDALClose(poT1AveDS);
    if (poT2AveDS) GDALClose(poT2AveDS);

    if (outputPath!=diffPath)
        poDriver->Delete(diffPath.toUtf8().constData());
    if (outputPath!=mergePath)
        poDriver->Delete(mergePath.toUtf8().constData());
    poDriver->Delete(T1AvePath.toUtf8().constData());
    poDriver->Delete(T2AvePath.toUtf8().constData());
}

bool CDTPBCDDiff::isCompleted() const
{
    return completed;
}

