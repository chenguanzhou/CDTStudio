#include "gdal2opencv.h"
#include <gdal_priv.h>

GDAL2OpenCV::GDAL2OpenCV()
{
    GDALAllRegister();
}

cv::Mat GDAL2OpenCV::ds2mat(GDALDataset* poSrcDS)
{
    if (poSrcDS == NULL)
        return cv::Mat();

    int width = poSrcDS->GetRasterXSize();
    int height = poSrcDS->GetRasterYSize();
    int bandCount = poSrcDS->GetRasterCount();
    GDALDataType type = poSrcDS->GetRasterBand(1)->GetRasterDataType();
    int bytePerPixel = GDALGetDataTypeSizeBytes(type);

    std::vector<cv::Mat> mats(bandCount);
    for (int k=0;k<bandCount;++k)
    {
        cv::Mat &mat = mats[k];
        mat = cv::Mat(height,width,gdal2opencv(type,1));
        for (int i=0;i<height;++i)
        {
            poSrcDS->GetRasterBand(k+1)->RasterIO(GF_Read,0,i,width,1,mat.ptr<uchar>(i),width,1,type,0,0);
        }
    }

    cv::Mat res;
    cv::merge(mats,res);
    return res;
}

int GDAL2OpenCV::gdal2opencv( const GDALDataType& gdalType, const int& channels ){

    switch( gdalType ){

        /// UInt8
        case GDT_Byte:
            if( channels == 1 ){ return CV_8UC1; }
            if( channels == 3 ){ return CV_8UC3; }
            if( channels == 4 ){ return CV_8UC4; }
            else { return CV_8UC(channels); }
            return -1;

        /// UInt16
        case GDT_UInt16:
            if( channels == 1 ){ return CV_16UC1; }
            if( channels == 3 ){ return CV_16UC3; }
            if( channels == 4 ){ return CV_16UC4; }
            else { return CV_16UC(channels); }
            return -1;

        /// Int16
        case GDT_Int16:
            if( channels == 1 ){ return CV_16SC1; }
            if( channels == 3 ){ return CV_16SC3; }
            if( channels == 4 ){ return CV_16SC4; }
            else { return CV_16SC(channels); }
            return -1;

        /// UInt32
        case GDT_UInt32:
        case GDT_Int32:
            if( channels == 1 ){ return CV_32SC1; }
            if( channels == 3 ){ return CV_32SC3; }
            if( channels == 4 ){ return CV_32SC4; }
            else { return CV_32SC(channels); }
            return -1;

        case GDT_Float32:
            if( channels == 1 ){ return CV_32FC1; }
            if( channels == 3 ){ return CV_32FC3; }
            if( channels == 4 ){ return CV_32FC4; }
            else { return CV_32FC(channels); }
            return -1;

        case GDT_Float64:
            if( channels == 1 ){ return CV_64FC1; }
            if( channels == 3 ){ return CV_64FC3; }
            if( channels == 4 ){ return CV_64FC4; }
            else { return CV_64FC(channels); }
            return -1;

        default:
            std::cout << "Unknown GDAL Data Type" << std::endl;
            std::cout << "Type: " << GDALGetDataTypeName(gdalType) << std::endl;
            return -1;
    }

    return -1;
}
