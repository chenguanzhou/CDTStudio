#include "CdtPartialSegmentationMapTool.h"

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
                src.at<uchar>(i,j)= 42;
            }
            else
            {
                src.at<uchar>(i,j)= 0;
            }
        }
    }
}

QgsPolygon partialSegment(const QgsGeometry &polygon,QString imagePath)
{
    qDebug()<<"enter PartialSegmentat";
    QTime t;
    t.start();

    //init
    GDALAllRegister();
    GDALDataset *poSrcDS = (GDALDataset *)GDALOpen(imagePath.toUtf8().constData(),GA_ReadOnly);
    int _width      = poSrcDS->GetRasterXSize();
    int _height      = poSrcDS->GetRasterYSize();
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
        if(vecInputPoints[i].y>_height)
            vecInputPoints[i].y = _height;

        if (vecInputPoints[i].x<minX)
            minX = vecInputPoints[i].x;
        if (vecInputPoints[i].x>maxX)
            maxX = vecInputPoints[i].x;
        if (vecInputPoints[i].y<minY)
            minY = vecInputPoints[i].y;
        if (vecInputPoints[i].y>maxY)
            maxY = vecInputPoints[i].y;
    }

    int nXOff = (int)minX;
    int nYOff = (int)minY;
    int nWidth  = (int)maxX-nXOff+1;
    int nHeight = (int)maxY-nYOff+1;

    if(nXOff<0)
        nXOff=0;
    if(nYOff<0)
        nYOff=0;
    if(_width<(nXOff+nWidth))
        nWidth = _width-nXOff;
    if(_height<(nYOff+nHeight))
        nHeight = _height-nYOff;

    cv::Rect rect(nXOff, nYOff, nWidth, nHeight);
    OGRLinearRing ring;
    for (size_t i=0; i<vecInputPoints.size(); ++i)
    {
       ring.addPoint(vecInputPoints[i].x, vecInputPoints[i].y);
    }

    ring.closeRings();

    OGRPolygon ogrPolygon;
    ogrPolygon.addRing(&ring);

    cv::Mat matMask = cv::Mat::zeros(_height,_width,CV_8U);
    GetFlagMat(matMask, ogrPolygon);

    cv::rectangle(matMask,rect,cv::Scalar(20),2);

    imwrite("matMask.jpg", matMask);
    qDebug()<<"get matMask";

    QgsPolygon exportPolygon;

    qDebug()<<t.restart();
    return exportPolygon;
}

CDTPartialSegmentationMapTool::CDTPartialSegmentationMapTool(QgsMapCanvas *canvas) :
    QgsMapTool(canvas),
    mRubberBand(Q_NULLPTR),
    vectorLayer(Q_NULLPTR)
{
    mCursor = Qt::ArrowCursor;
}

CDTPartialSegmentationMapTool::~CDTPartialSegmentationMapTool()
{
    delete mRubberBand;
}

void CDTPartialSegmentationMapTool::canvasMoveEvent(QgsMapMouseEvent *e)
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

void CDTPartialSegmentationMapTool::canvasPressEvent(QgsMapMouseEvent *e)
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
            QgsPolygon snakePolygon = partialSegment(selectGeom,imagePath);
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
