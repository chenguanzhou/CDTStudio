#include "cdtsnakemaptool.h"
#include <QtCore>
#include <qgsgeometry.h>
#include <qgsrubberband.h>
#include <qgsmapcanvas.h>
#include <qgsvectorlayer.h>
#include <qgsvectordataprovider.h>
#include <qgis.h>
#include <QMouseEvent>
#include <gdal_priv.h>
#include "StatisticSnake.h"

QgsPolygon snake(const QgsPolygon &polygon,QString imagePath)
{
    //init
    GDALAllRegister();
    GDALDataset *poSrcDS = (GDALDataset *)GDALOpen(imagePath.toUtf8().constData(),GA_ReadOnly);
    if (poSrcDS == NULL)
    {
        qWarning()<<QObject::tr("Open Image File: %1 failed!").arg(imagePath);
        return QgsPolygon();
    }
    int nBandCount = poSrcDS->GetRasterCount();
//    GDALDataType type = poSrcDS->GetRasterBand(1)->GetRasterDataType();
//    int dataSize = GDALGetDataTypeSize(type)/8;

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

    std::vector<uchar> vecImageData(nWidth*nHeight);
    if (nBandCount==1)
    {
        poSrcDS->GetRasterBand(1)->RasterIO(GF_Read,nXOff,nYOff,nWidth,nHeight,&vecImageData[0],nWidth,nHeight,GDT_Byte,0,0);
    }
    else
    {
        std::vector<int> vecImageTemp(nWidth*nHeight,0);
        for (int k=0;k<nBandCount;++k)
        {
            poSrcDS->GetRasterBand(k+1)->RasterIO(GF_Read,nXOff,nYOff,nWidth,nHeight,&vecImageData[0],nWidth,nHeight,GDT_Byte,0,0);
            for(int i=0;i<nWidth*nHeight;++i)
            {
                vecImageTemp[i] += vecImageData[i];
            }
        }
        for (int k=0;k<nBandCount;++k)
        {
            for(int i=0;i<nWidth*nHeight;++i)
            {
                vecImageData[i] = vecImageTemp[i]/nBandCount;
            }
        }
    }

    for (size_t i=0;i<vecInputPoints.size();++i)
    {
        vecInputPoints[i].x -= nXOff;
        vecInputPoints[i].y -= nYOff;
    }

    CStatisticSnake snake;
    snake.Initialize(&vecImageData[0],nWidth,nHeight,nWidth,vecInputPoints);
    snake.GetStatisticSnake();

    std::vector<VERTEX2D> vecExportPoints = snake.m_seedArray;
    for (size_t i=0;i<vecExportPoints.size();++i)
    {
        vecExportPoints[i].x += nXOff;
        vecExportPoints[i].y += nYOff;
    }

    QgsPolyline polyline;
    for (size_t i=0;i<vecExportPoints.size();++i)
    {
        QgsPoint ptTemp;
        ptTemp.set(
                padfTransform[0] + padfTransform[1]*vecExportPoints[i].x
                + padfTransform[2]*vecExportPoints[i].y,
                padfTransform[3] + padfTransform[4]*vecExportPoints[i].x
                + padfTransform[5]*vecExportPoints[i].y);
        polyline.push_back(ptTemp);
    }
    QgsPolygon exportPolygon;
    exportPolygon.push_back(polyline);
    return exportPolygon;


//    return polygon;
}

CDTSnakeMapTool::CDTSnakeMapTool(QgsMapCanvas *canvas) :
    QgsMapTool(canvas),
    mRubberBand(NULL),
    vectorLayer(NULL)
{
    mCursor = Qt::ArrowCursor;
}

CDTSnakeMapTool::~CDTSnakeMapTool()
{
    delete mRubberBand;
}

void CDTSnakeMapTool::canvasPressEvent( QMouseEvent * e )
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

            QgsPolygon snakePolygon = snake(polygon,imagePath);
            QgsGeometry* newPolygonGeom = QgsGeometry::fromPolygon(snakePolygon);
            QgsFeature f(vectorLayer->pendingFields(),0);
            f.setGeometry(newPolygonGeom);
            vectorLayer->beginEditCommand( "snake" );
            qDebug()<<vectorLayer->addFeature(f);
            vectorLayer->endEditCommand();
            canvas()->refresh();
        }
        mRubberBand->reset( QGis::Polygon );
        delete mRubberBand;
        mRubberBand = 0;
    }
}

void CDTSnakeMapTool::canvasMoveEvent( QMouseEvent * e )
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
