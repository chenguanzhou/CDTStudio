#include "cdtgrabcutmaptool.h"
#include <QtCore>
#include <qgsgeometry.h>
#include <qgsrubberband.h>
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include <qgsvectordataprovider.h>
#include <qgis.h>
#include <QMouseEvent>
#include <gdal_priv.h>
#include <opencv2/opencv.hpp>

typedef struct tagVERTEX2D
{
    double x;
    double y;
}VERTEX2D;

QgsPolygon grabcut(const QgsPolygon &polygon,QString imagePath)
{
    QTime t;
    t.start();

    //init
    GDALAllRegister();
    GDALDataset *poSrcDS = (GDALDataset *)GDALOpen(imagePath.toUtf8().constData(),GA_ReadOnly);
    int _width      = poSrcDS->GetRasterXSize();
    int _heith      = poSrcDS->GetRasterYSize();
    if (poSrcDS == NULL)
    {
        qWarning()<<QObject::tr("Open Image File: %1 failed!").arg(imagePath);
        return QgsPolygon();
    }
    int _bandCount = poSrcDS->GetRasterCount();

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
    QVector<QgsPoint> originalPoints = polygon[0];
    foreach (QgsPoint pt, originalPoints) {
        VERTEX2D newPt;
        GDALApplyGeoTransform(padfTransform_i,pt.x(),pt.y(),&newPt.x,&newPt.y);
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
    std::vector<int> vecImageData(nWidth*nHeight);
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

    const cv::Scalar GREEN = cv::Scalar(0,255,0);
    cv::Mat mask = cv::Mat::zeros(nHeight,nWidth,CV_8U);
    mask.setTo(0);

    std::vector<cv::Point> pContourInputPoints;
    std::vector< std::vector<cv::Point> > pContour;

    for (size_t i=0;i<vecInputPoints.size();++i)
    {
       cv::Point acvPoint;
       acvPoint.x=vecInputPoints[i].x -nXOff;
       acvPoint.y=vecInputPoints[i].y -nYOff;
       pContourInputPoints.push_back(acvPoint);
    }
    pContour.push_back(pContourInputPoints);


    qDebug()<<t.restart();

    cv::drawContours(mask,pContour,0,cv::Scalar(3),CV_FILLED);

    qDebug()<<t.restart();

    cv::grabCut(image,mask,rect,cv::Mat(),cv::Mat(),3,cv::GC_INIT_WITH_MASK);

    qDebug()<<t.restart();

    cv::Mat res,binMask;
    cv::rectangle(image,rect,GREEN,2);

    binMask.create( mask.size(), CV_8UC1 );
    binMask = mask & 1;

    std::vector< std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(binMask,contours,hierarchy,  CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    std::vector<VERTEX2D> vecAllPoints;

    QgsPolygon exportPolygon;

    int maxSize = 0 ;
    foreach(std::vector<cv::Point>  contour,contours)
    {
        if(contour.size()>maxSize)
        {
            maxSize = contour.size();
        }
    }
    foreach(std::vector<cv::Point>  contour,contours)
    {
        vecAllPoints.clear();
        if((!contours.empty())&&(contour.size()==maxSize)){
            for(int i =0;i<contour.size();i++){
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
            for (size_t i=vecAllPoints.size()-1;i>0;--i)
            {
                QgsPoint ptTemp;
                ptTemp.set(
                        padfTransform[0] + padfTransform[1]*vecAllPoints[i].x
                        + padfTransform[2]*vecAllPoints[i].y,
                        padfTransform[3] + padfTransform[4]*vecAllPoints[i].x
                        + padfTransform[5]*vecAllPoints[i].y);
                polyline.push_back(ptTemp);
            }

            exportPolygon.push_back(polyline);
        }
    }

    qDebug()<<t.restart();
    return exportPolygon;

}

CDTGrabcutMapTool::CDTGrabcutMapTool(QgsMapCanvas *canvas) :
    QgsMapTool(canvas),
    mRubberBand(NULL),
    vectorLayer(NULL)
{
    mCursor = Qt::ArrowCursor;
}

CDTGrabcutMapTool::~CDTGrabcutMapTool()
{
    delete mRubberBand;
}

void CDTGrabcutMapTool::canvasMoveEvent(QgsMapMouseEvent *e)
{
    if ( mRubberBand == NULL )
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
    if ( mRubberBand == NULL )
    {
        mRubberBand = new QgsRubberBand( mCanvas, QGis::Polygon );
        mRubberBand->setColor(QColor(Qt::red));
    }
    if ( e->button() == Qt::LeftButton )
    {
        mRubberBand->addPoint( toMapCoordinates( e->pos() ) );
    }
    else if ( e->button() == Qt::RightButton )
    {
        if ( mRubberBand->numberOfVertices() > 2 )
        {
            QgsGeometry* polygonGeom = mRubberBand->asGeometry();
            QgsPolygon polygon = polygonGeom->asPolygon();
            delete polygonGeom;

            QTime t;
            t.start();
            QgsPolygon snakePolygon = grabcut(polygon,imagePath);
            qDebug()<<t.elapsed();

            QgsGeometry* newPolygonGeom = QgsGeometry::fromPolygon(snakePolygon);
            QgsFeature f(vectorLayer->pendingFields(),0);
            f.setGeometry(newPolygonGeom);
            vectorLayer->startEditing();
            qDebug()<<vectorLayer->addFeature(f);
//            qDebug()<<newPolygonGeom->area();
            vectorLayer->commitChanges();
            canvas()->refresh();
        }
        mRubberBand->reset( QGis::Polygon );
        delete mRubberBand;
        mRubberBand = 0;
    }
}
