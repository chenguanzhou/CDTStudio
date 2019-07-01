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

typedef struct tagVERTEX2D
{
    double x;
    double y;
}VERTEX2D;

void GetFlagMat(cv::Mat& src, OGRPolygon& polygon)
{
    for(int i=0;i<src.rows;++i)
    {
        for(int j=0;j<src.cols;++j)
        {
            OGRPoint ogrP(j,i);
            if(polygon.IsPointOnSurface(&ogrP))
            {
                src.at<uchar>(i,j)= 3;
            }
            else
            {
                src.at<uchar>(i,j)= 0;
            }
        }
    }
}

QgsPolygon grabcut(const QgsGeometry &polygon,QString imagePath)
{
    qDebug()<<"enter grabcut";
    QTime t;
    t.start();

    //init
    GDALAllRegister();
    GDALDataset *poSrcDS = (GDALDataset *)GDALOpen(imagePath.toUtf8().constData(),GA_ReadOnly);
    int _width      = poSrcDS->GetRasterXSize();
    int _heith      = poSrcDS->GetRasterYSize();
    if (poSrcDS == Q_NULLPTR)
    {
        qWarning()<<QObject::tr("Open Image File: %1 failed!").arg(imagePath);
        return QgsPolygon();
    }

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
//        qDebug()<< QString::number(i)<<"x: "+ QString::number(vecInputPoints[i].x)<<"y: "+ QString::number(vecInputPoints[i].y)<<endl;
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
    std::vector<int> vecImageData(nWidth * nHeight);
    for (int k=0;k<3;++k)
    {
        double minPix = std::numeric_limits<double>::max();
        double maxPix = std::numeric_limits<double>::min();
        poSrcDS->GetRasterBand(k+1)->RasterIO(GF_Read,nXOff,nYOff,nWidth,nHeight,&vecImageData[0],nWidth,nHeight,GDT_UInt32,0,0);
        for(int i=0;i<nHeight;i++)
        {
            for(int j=0;j<nWidth;j++)
            {
                double imagePix = vecImageData[i*nWidth+j];
                if(imagePix <minPix) minPix = imagePix;
                if(imagePix >maxPix) maxPix = imagePix;
            }
        }

        const double pixelmin = minPix;
        const double pixelmax = maxPix;
        const double ak = (double)255 / (double)(pixelmax - pixelmin);
        const double bk =-(double)255 * (double)pixelmin/(double)(pixelmax - pixelmin);

        for(int i=0;i<nHeight;i++)
        {
            for(int j=0;j<nWidth;j++)
            {
                double imagePix = vecImageData[i*nWidth+j];
                double pix = ak*imagePix+bk+0.5;
                if(pix>255) pix = 255;
                if(pix<0)   pix = 0;
                image.at<cv::Vec3b>(i,j)[k]= (uchar)pix;
            }
        }
    }

    cv::Rect rect;
    rect.x = rec_nWidth/12;
    rect.y = rec_nHeight/12;
    rect.width = rec_nWidth;
    rect.height = rec_nHeight;

    OGRLinearRing ring;
    cv::vector<cv::Point> pContourInputPoints(vecInputPoints.size());
    for (size_t i=0;i<vecInputPoints.size();++i)
    {
       cv::Point acvPoint;
       acvPoint.x = vecInputPoints[i].x -nXOff;
       acvPoint.y = vecInputPoints[i].y -nYOff;
       pContourInputPoints.push_back(acvPoint);

       qDebug()<< QString::number(i)<<"x: "+ QString::number(acvPoint.x)<<"y: "+ QString::number(acvPoint.y)<<endl;
       ring.addPoint(acvPoint.x , acvPoint.y);
    }

    ring.closeRings();

    OGRPolygon ogrPolygon;
    ogrPolygon.addRing(&ring);

    cv::Mat matMask = cv::Mat::zeros(nHeight,nWidth,CV_8U);
    GetFlagMat(matMask, ogrPolygon);

    cv::vector< cv::vector<cv::Point> > pContour(1);
    pContour.push_back(pContourInputPoints);

    qDebug()<<t.restart();

    cv::Mat mask = cv::Mat::zeros(nHeight,nWidth,CV_8U);
    mask.setTo(0);

    ///TODO:fix bug of cv::drawContours, can't draw contours to mask
    ///
//    cv::drawContours(mask, pContour, -1, cv::Scalar(3), CV_FILLED);
//    cv::imwrite("mask.jpg", mask);

    const cv::Scalar GREEN = cv::Scalar(0,255,0);
    cv::rectangle(image,rect,GREEN,2);
    cv::imwrite("test.jpg", image);

    qDebug()<<t.restart();

    cv::grabCut(image, matMask, rect, cv::Mat(), cv::Mat(), 3, cv::GC_INIT_WITH_MASK);

    cv::imwrite("result.jpg", matMask);
    qDebug()<<t.restart();

    cv::Mat res,binMask;
    cv::rectangle(image,rect,GREEN,2);

    binMask.create( matMask.size(), CV_8UC1 );
    binMask = matMask & 1;

    qDebug()<<"start find contours";
    std::vector< std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(binMask,contours,hierarchy,  CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    qDebug()<<"find contours done: "+QString::number(contours.size());
    std::vector<VERTEX2D> vecAllPoints;

    qDebug()<<"max size: "+QString::number(contours[0].size());
    int maxSize = contours[0].size();
//    int maxSize = 0 ;
//    foreach(std::vector<cv::Point>  contour,contours)
//    {
//        if(contour.size()>maxSize)
//        {
//            maxSize = contour.size();
//        }
//    }

    qDebug()<<"0";
    QgsPolygon exportPolygon;

//    foreach(std::vector<cv::Point>  contour,contours)
//    {
        vecAllPoints.clear();
        if((!contours.empty())&&(contours[0].size()==maxSize))
        {
            for(int i =0;i<contours[0].size();i++)
            {
                VERTEX2D Pt;
                Pt.x = contours[0].at(i).x;
                Pt.y = contours[0].at(i).y;
                vecAllPoints.push_back(Pt);
            }
            for (size_t i=0;i<vecAllPoints.size();++i)
            {
                vecAllPoints[i].x += nXOff;
                vecAllPoints[i].y += nYOff;
            }

            qDebug()<<"1";
            QgsPolyline polyline;
            for (size_t i=vecAllPoints.size()-1;i>0;--i)
            {
                QgsPoint ptTemp (padfTransform[0] + padfTransform[1]*vecAllPoints[i].x
                        + padfTransform[2]*vecAllPoints[i].y,
                        padfTransform[3] + padfTransform[4]*vecAllPoints[i].x
                        + padfTransform[5]*vecAllPoints[i].y);
                polyline.push_back(ptTemp);
            }

            qDebug()<<"2";
            QgsLineString *curve = new QgsLineString(polyline);
            exportPolygon.setExteriorRing(curve);
            qDebug()<<"3";
        }
//    }

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
//            qDebug()<<t.elapsed();

            qDebug()<<"a";
            QgsGeometry newPolygonGeom = QgsGeometry(snakePolygon.boundary());
            qDebug()<<"b";
            QgsFeature f(vectorLayer->fields(),0);
            f.setGeometry(newPolygonGeom);
            vectorLayer->startEditing();
            vectorLayer->addFeature(f);
            vectorLayer->commitChanges();
            qDebug()<<"c";
            canvas()->refresh();
        }
        mRubberBand->reset( QgsWkbTypes::PolygonGeometry );
        delete mRubberBand;
        mRubberBand = Q_NULLPTR;
    }
}
