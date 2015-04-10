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

        int nXSize = poBand1->GetXSize();
        int nYSize = poBand1->GetYSize();
        GDALDataType dataType = poBand1->GetRasterDataType();
        int dataSize = GDALGetDataTypeSize(dataType);

        uchar* bufferT1 = new uchar[dataSize];
        uchar* bufferT2 = new uchar[dataSize];
        CPLErr ret;
        for (int row = 0;row<nYSize;++row)
        {
            for (int col = 0;col<nXSize;++col)
            {
                ret = poBand1->RasterIO(GF_Read,col,row,1,1,bufferT1,1,1,dataType,0,0);
                if (ret != CE_None)
                    return tr("Read image1 failed");
                ret = poBand2->RasterIO(GF_Read,col,row,1,1,bufferT2,1,1,dataType,0,0);
                if (ret != CE_None)
                    return tr("Read image2 failed");

                float diff = static_cast<float>(SRCVAL(bufferT1,dataType,0)) -
                        static_cast<float>(SRCVAL(bufferT2,dataType,0));

                ret = poBandDiff->RasterIO(GF_Write,col,row,1,1,&diff,1,1,GDT_Float32,0,0);
                if (ret != CE_None)
                    return tr("Write diff image failed");
            }
        }
        delete []bufferT1;
        delete []bufferT2;
    }
    return QString::null;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Differencing, ImageDifferencing)
#endif // QT_VERSION < 0x050000
