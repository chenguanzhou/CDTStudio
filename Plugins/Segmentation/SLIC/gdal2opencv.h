#ifndef GDAL2OPENCV_H
#define GDAL2OPENCV_H

#include <opencv2/opencv.hpp>
#include <gdal_priv.h>

class GDAL2OpenCV
{
public:
    GDAL2OpenCV();

    static cv::Mat ds2mat(GDALDataset *poSrcDS);

private:
    static int gdal2opencv(const GDALDataType &gdalType, const int &channels);
};

#endif // GDAL2OPENCV_H
