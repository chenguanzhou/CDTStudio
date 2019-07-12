#include "cdtgrabcutmaptool.h"

#include <qgsgeometry.h>
#include <qgsrubberband.h>
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include <qgsvectordataprovider.h>
#include <qgis.h>
#include <qgspolygon.h>
#include <qgslinestring.h>
#include <qgsmapmouseevent.h>
#include <gdal_priv.h>
#include <opencv2/opencv.hpp>

bool FindContours(QVector<bool> &vecMark, std::vector<cv::Point>&  contour, int &nX, int &nY, cv::Mat cvMat)
{
    int nWidth = cvMat.cols;
    ///left
    int nNextX = nX -1;
    int nNextY = nY;
    if( MARK_VALUE == cvMat.at<uchar>(nNextY,nNextX) && MARK_VALUE != cvMat.at<uchar>(nNextY - 1,nNextX) && !vecMark[nNextY * nWidth + nNextX])
    {
        vecMark[nNextY * nWidth + nNextX] = true;

        cv::Point p(nNextX, nNextY);
        contour.push_back(p);
        nX = nNextX;
        nY = nNextY;
        return true;
    }

    ///right
    nNextX = nX + 1;
    nNextY = nY;
    bool bRightIn =  MARK_VALUE == cvMat.at<uchar>(nY -1, nX)  && MARK_VALUE != cvMat.at<uchar>(nY, nX) && !vecMark[nNextY * nWidth + nNextX];
    if(bRightIn )
    {
        vecMark[nNextY * nWidth + nNextX] = true;
        cv::Point p(nNextX, nNextY);
        contour.push_back(p);
        nX = nNextX;
        nY = nNextY;
        return true;
    }

    ///Up
    nNextX = nX;
    nNextY = nY - 1;
    bool bUpIn = MARK_VALUE == cvMat.at<uchar>(nNextY, nNextX - 1) && MARK_VALUE != cvMat.at<uchar>(nNextY, nNextX ) && !vecMark[nNextY * nWidth + nNextX];
    if(bUpIn)
    {
        vecMark[nNextY * nWidth + nNextX] = true;
        cv::Point p(nNextX, nNextY);
        contour.push_back(p);
        nX = nNextX;
        nY = nNextY;
        return true;
    }

    ///Down
    nNextX = nX;
    nNextY = nY + 1;
    bool bDownIn = MARK_VALUE!= cvMat.at<uchar>(nY, nX-1) &&MARK_VALUE == cvMat.at<uchar>(nY, nX)&& !vecMark[nNextY * nWidth + nNextX];
    if(bDownIn)
    {
        vecMark[nNextY * nWidth + nNextX] = true;
        cv::Point p(nNextX, nNextY);
        contour.push_back(p);
        nX = nNextX;
        nY = nNextY;
        return true;
    }

    return false;
}

typedef struct tagVERTEX2D
{
    double x;
    double y;
}VERTEX2D;

QgsPolygon grabcut(const QgsGeometry &polygon,QString imagePath)
{
    qDebug()<<"enter grabcut";
    QTime t;
    t.start();

    //init
    GDALAllRegister();
    GDALDataset *poSrcDS = (GDALDataset *)GDALOpen(imagePath.toUtf8().constData(),GA_ReadOnly);
    if (poSrcDS == Q_NULLPTR)
    {
        qWarning()<<QObject::tr("Open Image File: %1 failed!").arg(imagePath);
        return QgsPolygon();
    }

    int _width      = poSrcDS->GetRasterXSize();
    int _heith      = poSrcDS->GetRasterYSize();
    GDALDataType gdalDataType = poSrcDS->GetRasterBand(1)->GetRasterDataType();
    unsigned pixelSize = GDALGetDataTypeSize(gdalDataType)/8;

    double padfTransform[6] = {0,1,0,0,0,1};
    double padfTransform_i[6];
    poSrcDS->GetGeoTransform(padfTransform);
    double pStdGeoTransform[6] = {0,1,0,0,0,1};
    bool isTrans = false;
    for (int i=0;i<6;++i)
    {
        if (fabs(padfTransform[i]-pStdGeoTransform[i])>0.001)
        {
            isTrans = true;
            break;
        }
    }
    if (!isTrans)
    {
        padfTransform[5] = -1;
    }
    GDALInvGeoTransform(padfTransform,padfTransform_i);

    //transform
    std::vector<VERTEX2D> vecInputPoints;
    auto iter = polygon.vertices_begin();
    for(;iter!= polygon.vertices_end(); ++iter)
    {

        VERTEX2D newPt;
        GDALApplyGeoTransform(padfTransform_i,(*iter).x(),(*iter).y(),&newPt.x,&newPt.y);
        vecInputPoints.push_back(newPt);
    }

    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::min();
    double maxY = std::numeric_limits<double>::min();

    for (size_t i=0;i<vecInputPoints.size();++i)
    {
        if(vecInputPoints[i].x<0)
            vecInputPoints[i].x = 0;
        if(vecInputPoints[i].y<0)
            vecInputPoints[i].y = 0;
        if(vecInputPoints[i].x>_width)
            vecInputPoints[i].x = _width;
        if(vecInputPoints[i].y>_heith)
            vecInputPoints[i].y = _heith;

        if (vecInputPoints[i].x<minX)
            minX = vecInputPoints[i].x;
        if (vecInputPoints[i].x>maxX)
            maxX = vecInputPoints[i].x;
        if (vecInputPoints[i].y<minY)
            minY = vecInputPoints[i].y;
        if (vecInputPoints[i].y>maxY)
            maxY = vecInputPoints[i].y;
    }

    int rec_nXOff = (int)minX;
    int rec_nYOff = (int)minY;
    int rec_nWidth  = (int)maxX-rec_nXOff+1;
    int rec_nHeight = (int)maxY-rec_nYOff+1;

    int nXOff = rec_nXOff - rec_nWidth/12;
    int nYOff = rec_nYOff - rec_nHeight/12;
    int nWidth = rec_nWidth + rec_nWidth/6;
    int nHeight = rec_nHeight + rec_nHeight/6;

    if(nXOff<0)
        nXOff=0;
    if(nYOff<0)
        nYOff=0;
    if(_width<(nXOff+nWidth))
        nWidth = _width-nXOff;
    if(_heith<(nYOff+nHeight))
        nHeight = _heith-nYOff;

    cv::Mat  image = cv::Mat::zeros(nHeight,nWidth,CV_8UC3);
    std::vector<unsigned char*> vecImageData(3);
    for (unsigned k=0;k<3;++k)
    {
        vecImageData[k] = new unsigned char[nWidth * pixelSize];
    }

    for (int k=0; k<3; ++k)
    {
        double minPix = std::numeric_limits<double>::max();
        double maxPix = std::numeric_limits<double>::min();

        for(int i=0;i<nHeight;i++)
        {
            poSrcDS->GetRasterBand(k+1)->RasterIO(GF_Read,nXOff,nYOff+i,nWidth,1,vecImageData[k],nWidth,1,gdalDataType,0,0);

            for(int j=0;j<nWidth;j++)
            {
                double imagePix = SRCVAL(vecImageData[k],gdalDataType,j);
                if(imagePix <minPix) minPix = imagePix;
                if(imagePix >maxPix) maxPix = imagePix;
            }
        }

        const double pixelmin = minPix;
        const double pixelmax = maxPix;
        const double ak = (double)255 / (double)(pixelmax - pixelmin + 0.000001);
        const double bk =-(double)255 * (double)pixelmin/(double)(pixelmax - pixelmin + 0.000001);

        for(int i=0;i<nHeight;i++)
        {
            poSrcDS->GetRasterBand(k+1)->RasterIO(GF_Read,nXOff,nYOff+i,nWidth,1,vecImageData[k],nWidth,1,gdalDataType,0,0);

            for(int j=0;j<nWidth;j++)
            {
                double imagePix = SRCVAL(vecImageData[k],gdalDataType,j);
                double pix = ak*imagePix+bk+0.5;
                if(pix>255) pix = 255;
                if(pix<0)   pix = 0;
                image.at<cv::Vec3b>(i,j)[k]= (uchar)pix;
            }
        }
    }

    for (unsigned k=0;k<3;++k)
    {
        delete [](vecImageData[k]);
    }
    cv::imwrite("test.jpg", image);

    cv::Rect rect;
    rect.x = rec_nWidth/12;
    rect.y = rec_nHeight/12;
    rect.width = rec_nWidth;
    rect.height = rec_nHeight;


    OGRLinearRing* ring = (OGRLinearRing*)OGRGeometryFactory::createGeometry(wkbLinearRing);
//    OGRLinearRing ring;
    cv::vector<cv::Point> pContourInputPoints(vecInputPoints.size());
    for (size_t i=0;i<vecInputPoints.size();++i)
    {
       cv::Point acvPoint;
       acvPoint.x = vecInputPoints[i].x -nXOff;
       acvPoint.y = vecInputPoints[i].y -nYOff;
       pContourInputPoints.push_back(acvPoint);

       ring->addPoint(acvPoint.x , acvPoint.y);
    }

    OGRPolygon* ogrPolygon = (OGRPolygon*)OGRGeometryFactory::createGeometry(wkbPolygon);
//    OGRPolygon ogrPolygon;
    ogrPolygon->addRing(ring);
    ring->closeRings();


    cv::Mat matMask = cv::Mat::zeros(nHeight,nWidth,CV_8UC1);

    for(int i=0;i<nHeight;++i)
    {
        for(int j=0;j<nWidth;++j)
        {
            OGRPoint ogrP(j,i);
            if(ogrPolygon->IsPointOnSurface(&ogrP))
            {
                matMask.at<uchar>(i,j)= MARK_VALUE;
            }
            else
            {
                matMask.at<uchar>(i,j)= 0;
            }
        }
    }

//    cv::imwrite("mask0.jpg", matMask);

//    const cv::Scalar GREEN = cv::Scalar(0,255,0);
//    cv::rectangle(image,rect,GREEN,2);
//    cv::imwrite("test.jpg", image);

    qDebug()<<t.restart();

    cv::grabCut(image, matMask, rect, cv::Mat(), cv::Mat(), 3, cv::GC_INIT_WITH_MASK);

    qDebug()<<t.restart();

    cv::imwrite("grabcutbinMask.jpg", matMask);

    cv::Mat newMatMask = cv::Mat::zeros(nHeight,nWidth,CV_8UC1);

    for(int i = 0; i < matMask.rows; ++i)
    {
        for(int j = 0; j < matMask.cols; ++j)
        {
            if((uchar)MARK_VALUE == matMask.at<uchar>(i,j))
            {
                newMatMask.at<uchar>(i,j) = (uchar)MARK_VALUE;
            }
        }
    }

//    cv::imwrite("resultMask.bmp", newMatMask);
    qDebug()<<"write done";

    std::vector<cv::Point>  contour;
    QVector<bool> vecMark( matMask.rows*matMask.cols,false);
    bool bStop = false;
    for(int i = 0; i < matMask.rows; ++i)
    {
        for(int j = 0; j < matMask.cols; ++j)
        {
            if(newMatMask.at<uchar>(i,j) == (uchar)MARK_VALUE)
            {
                while(FindContours(vecMark,contour, j, i, newMatMask))
                {

                }
                bStop = true;
                break;
            }
        }
        if(bStop)
        {
            break;
        }
    }

    qDebug()<<contour.size();
//    cv::Mat res,binMask;
//    cv::rectangle(image,rect,GREEN,2);

//    binMask.create( matMask.size(), CV_8UC1 );
//    binMask = matMask & 3;

//    cv::imwrite("binMask.jpg", binMask);

//    cv::Mat res;
//    cv::threshold(matMask, res, 0, 255, CV_THRESH_OTSU);
//    cv::imwrite("otsuBinMask.jpg", res);


    std::vector<VERTEX2D> vecAllPoints;

    QgsPolygon exportPolygon;
    vecAllPoints.clear();

    for(int i =0;i<contour.size();i++)
    {
        VERTEX2D Pt;
        Pt.x = contour.at(i).x;
        Pt.y = contour.at(i).y;
        vecAllPoints.push_back(Pt);
    }
    for (size_t i=0;i<vecAllPoints.size();++i)
    {
        vecAllPoints[i].x += nXOff;
        vecAllPoints[i].y += nYOff;
    }

    QgsPolyline polyline;
    for(size_t i = vecAllPoints.size() -1;i != 0; --i)
    {
        QgsPoint ptTemp (padfTransform[0] + padfTransform[1]*vecAllPoints[i].x
                + padfTransform[2]*vecAllPoints[i].y,
                padfTransform[3] + padfTransform[4]*vecAllPoints[i].x
                + padfTransform[5]*vecAllPoints[i].y);
        polyline.push_back(ptTemp);
    }

    QgsLineString *curve = new QgsLineString(polyline);
    exportPolygon.setExteriorRing(curve);

    qDebug()<<t.restart();
    qDebug()<<"out grabcut";
    return exportPolygon;

}

CDTGrabcutMapTool::CDTGrabcutMapTool(QgsMapCanvas *canvas) :
    QgsMapTool(canvas),
    mRubberBand(Q_NULLPTR),
    vectorLayer(Q_NULLPTR)
{
    mCursor = Qt::ArrowCursor;
}

CDTGrabcutMapTool::~CDTGrabcutMapTool()
{
    delete mRubberBand;
}

void CDTGrabcutMapTool::canvasMoveEvent(QgsMapMouseEvent *e)
{
    if ( mRubberBand == Q_NULLPTR )
    {
        return;
    }
    if ( mRubberBand->numberOfVertices() > 0 )
    {
        mRubberBand->removeLastPoint( 0 );
        mRubberBand->addPoint( toMapCoordinates( e->pos() ) );
    }
}

void CDTGrabcutMapTool::canvasPressEvent(QgsMapMouseEvent *e)
{
    if ( mRubberBand == Q_NULLPTR )
    {
        mRubberBand = new QgsRubberBand( mCanvas, QgsWkbTypes::PolygonGeometry );
        mRubberBand->setColor(QColor(Qt::red));
    }
    if ( e->button() == Qt::LeftButton )
    {
        mRubberBand->addPoint( toMapCoordinates( e->pos() ) );
    }
    else if ( e->button() == Qt::RightButton )
    {
        qDebug()<<"right btn";
        if ( mRubberBand->numberOfVertices() > 2 )
        {
            QgsGeometry selectGeom = mRubberBand->asGeometry();

            QTime t;
            t.start();
            QgsPolygon snakePolygon = grabcut(selectGeom,imagePath);
            qDebug()<<t.elapsed();


            QgsGeometry newPolygonGeom = QgsGeometry(snakePolygon.boundary());
            QgsFeature f(vectorLayer->fields(),0);
            f.setGeometry(newPolygonGeom);
            vectorLayer->startEditing();
            vectorLayer->addFeature(f);
            vectorLayer->commitChanges();

            canvas()->refresh();
        }
        mRubberBand->reset( QgsWkbTypes::PolygonGeometry );
        delete mRubberBand;
        mRubberBand = Q_NULLPTR;
    }
}
