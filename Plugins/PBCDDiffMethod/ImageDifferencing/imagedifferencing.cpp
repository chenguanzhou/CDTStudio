#include "imagedifferencing.h"
#include "gdal_priv.h"

ImageDifferencing::ImageDifferencing(QObject *parent) :
    CDTPBCDDiffInterface(parent)
{

}

QString ImageDifferencing::methodName() const
{
    return "Image Differencing";
}

QString ImageDifferencing::generateDiffImage(QList<QPair<GDALRasterBand *, GDALRasterBand *> > poBands, GDALDataset *outDiffDS)
{
    for (int i=0;i<poBands.size();++i)
    {
        GDALRasterBand* poBand1 = poBands.value(i).first;
        GDALRasterBand* poBand2 = poBands.value(i).second;
        GDALRasterBand* poBandDiff = outDiffDS->GetRasterBand(i+1);

        int width = poBand1->GetXSize();
        int height = poBand1->GetYSize();
        GDALDataType dataType = poBand1->GetRasterDataType();
        int dataSize = GDALGetDataTypeSize(dataType)/8;

//        CPLErr ret;
//        int nXBlocks, nYBlocks, nXBlockSize, nYBlockSize;
//        int iXBlock, iYBlock;
//        poBandDiff->GetBlockSize( &nXBlockSize, &nYBlockSize );
//        nXBlocks = (width + nXBlockSize - 1) / nXBlockSize;
//        nYBlocks = (height + nYBlockSize - 1) / nYBlockSize;

//        float* bufferRes = new float[nXBlockSize*nYBlockSize];
//        for( iYBlock = 0; iYBlock < nYBlocks; ++iYBlock )
//        {
//            int nYOff = iYBlock*nYBlockSize;

//            for( iXBlock = 0; iXBlock < nXBlocks; ++iXBlock )
//            {
//                int nXValid, nYValid;
//                if( (iXBlock+1) * nXBlockSize > width )
//                    nXValid = width - iXBlock * nXBlockSize;
//                else
//                    nXValid = nXBlockSize;
//                if( (iYBlock+1) * nYBlockSize > height )
//                    nYValid = height - iYBlock * nYBlockSize;
//                else
//                    nYValid = nYBlockSize;

//                int nXOff = iXBlock*nXBlockSize;

//                uchar* bufferT1 = new uchar[dataSize*nXValid*nYValid];
//                uchar* bufferT2 = new uchar[dataSize*nXValid*nYValid];
//                ret = poBand1->RasterIO(GF_Read,nXOff,nYOff,nXValid,nYValid,bufferT1,nXValid,nYValid,dataType,0,0);
//                if (ret != CE_None)
//                    return tr("Read image1 failed");
//                ret = poBand2->RasterIO(GF_Read,nXOff,nYOff,nXValid,nYValid,bufferT2,nXValid,nYValid,dataType,0,0);
//                if (ret != CE_None)
//                    return tr("Read image2 failed");

//                uchar *pT1 = bufferT1;
//                uchar *pT2 = bufferT2;
//                for( int iY = 0; iY < nYValid; ++iY)
//                {
//                    float *pRes = bufferRes + iY*nXBlockSize;
//                    for( int iX = 0; iX < nXValid; ++iX,++pRes,pT1+=dataSize,pT2+=dataSize)
//                    {
//                        *pRes = static_cast<float>(SRCVAL(pT1,dataType,0)) -
//                                static_cast<float>(SRCVAL(pT2,dataType,0));
//                    }
//                }
//                ret = poBandDiff->WriteBlock(iXBlock,iYBlock,bufferRes);
//                if (ret != CE_None)
//                    return tr("Write diff image failed");
//                delete []bufferT1;
//                delete []bufferT2;
//            }
//        }
//        delete []bufferRes;

        uchar* bufferT1 = new uchar[dataSize*width];
        uchar* bufferT2 = new uchar[dataSize*width];
        float* bufferRes = new float[width];
        CPLErr ret;
        for (int row = 0;row<height;++row)
        {
            ret = poBand1->RasterIO(GF_Read,0,row,width,1,bufferT1,width,1,dataType,0,0);
            if (ret != CE_None)
                return tr("Read image1 failed");

            ret = poBand2->RasterIO(GF_Read,0,row,width,1,bufferT2,width,1,dataType,0,0);
            if (ret != CE_None)
                return tr("Read image1 failed");

            uchar *pT1 = bufferT1;
            uchar *pT2 = bufferT2;
            float *pRes = bufferRes;
            for (int col = 0;col<width;++col,++pRes,pT1+=dataSize,pT2+=dataSize)
            {
                *pRes = static_cast<float>(SRCVAL(pT1,dataType,0)) -
                        static_cast<float>(SRCVAL(pT2,dataType,0));
            }
            ret = poBandDiff->RasterIO(GF_Write,0,row,width,1,bufferRes,width,1,GDT_Float32,0,0);
            if (ret != CE_None)
                return tr("Write diff image failed");
        }
        delete []bufferT1;
        delete []bufferT2;
        delete []bufferRes;
    }
    return QString::null;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Differencing, ImageDifferencing)
#endif // QT_VERSION < 0x050000
