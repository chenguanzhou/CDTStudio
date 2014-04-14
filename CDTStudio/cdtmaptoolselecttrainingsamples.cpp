#include "cdtmaptoolselecttrainingsamples.h"
#include "qgsrubberband.h"
#include <QtCore>
#include "qgsvectorlayer.h"
#include "cdttrainingsamplesform.h"
#include "cdtimagelayer.h"

CDTMapToolSelectTrainingSamples::CDTMapToolSelectTrainingSamples(QgsMapCanvas *canvas) :
    QgsMapTool(canvas),
    mapCanvas(canvas),
    segmentationLayer(NULL),
    trainingSampleForm(NULL)
{
}

void CDTMapToolSelectTrainingSamples::setSegmentationLayer(CDTSegmentationLayer *layer)
{
    if (layer == NULL) return;

    samples.clear();
    segmentationLayer = layer;
    trainingSampleForm = layer->trainingForm();

    updateRubber();
}

void CDTMapToolSelectTrainingSamples::canvasMoveEvent(QMouseEvent *e)
{
    if (( e->buttons() & Qt::LeftButton ) )
    {
        mDragging = true;
        // move map and other canvas items
        mCanvas->panAction( e );
    }
}

void CDTMapToolSelectTrainingSamples::canvasReleaseEvent(QMouseEvent *e)
{
    if ( e->button() == Qt::LeftButton )
    {
        if ( mDragging )
        {
            mCanvas->panActionEnd( e->pos() );
            mDragging = false;
        }
        else // add pan to mouse cursor
        {
            // transform the mouse pos to map coordinates
            QgsPoint center = mCanvas->getCoordinateTransform()->toMapPoint( e->x(), e->y() );
            mCanvas->setExtent( QgsRectangle( center, center ) );
            mCanvas->refresh();
        }
    }
    else if (e->button()==Qt::RightButton)
    {
        QgsVectorLayer* vlayer = NULL;
        if ( !mapCanvas->currentLayer()
             || ( vlayer = qobject_cast<QgsVectorLayer *>( mapCanvas->currentLayer() ) ) == NULL )
            return;

        QRect selectRect( 0, 0, 0, 0 );
        int boxSize = 1;
        selectRect.setLeft  ( e->pos().x() - boxSize );
        selectRect.setRight ( e->pos().x() + boxSize );
        selectRect.setTop   ( e->pos().y() - boxSize );
        selectRect.setBottom( e->pos().y() + boxSize );

        const QgsMapToPixel* transform = mapCanvas->getCoordinateTransform();
        QgsPoint ll = transform->toMapCoordinates( selectRect.left(), selectRect.bottom() );
        QgsPoint ur = transform->toMapCoordinates( selectRect.right(), selectRect.top() );

        QgsPolyline points;
        points.push_back(ll);
        points.push_back(QgsPoint( ur.x(), ll.y() ));
        points.push_back(ur);
        points.push_back(QgsPoint( ll.x(), ur.y() ));

        QgsPolygon polygon;
        polygon.push_back(points);
        QgsGeometry selectGeom = *(QgsGeometry::fromPolygon(polygon) );

        if ( mapCanvas->mapRenderer()->hasCrsTransformEnabled() )
        {
            QgsCoordinateTransform ct( mapCanvas->mapRenderer()->destinationCrs(), vlayer->crs() );
            selectGeom.transform( ct );
        }

        QgsFeatureIterator fit = vlayer->getFeatures( QgsFeatureRequest().setFilterRect( selectGeom.boundingBox() ).setFlags( QgsFeatureRequest::ExactIntersect ) );
        QgsFeature f;
        qint64 closestFeatureId = 0;
        bool foundSingleFeature = false;
        double closestFeatureDist = std::numeric_limits<double>::max();
        while ( fit.nextFeature( f ) )
        {
            QgsGeometry* g = f.geometry();
            if ( !selectGeom.intersects( g ) )
                continue;
            foundSingleFeature = true;
            double distance = g->distance( selectGeom );
            if ( distance <= closestFeatureDist )
            {
                closestFeatureDist = distance;
                closestFeatureId = f.id();
            }
        }

        if ( foundSingleFeature )
            addSingleSample( closestFeatureId );

    }
}

void CDTMapToolSelectTrainingSamples::updateRubber()
{
    foreach (QgsRubberBand* band, rubberBands) {
        delete band;
    }
    rubberBands.clear();

    QgsVectorLayer* vlayer = NULL;
    if ( !mapCanvas->currentLayer()
         || ( vlayer = qobject_cast<QgsVectorLayer *>( mapCanvas->currentLayer() ) ) == NULL )
        return;

    QStringList keys = samples.keys();
    foreach (QString key, keys) {
        QList<qint64> ids = samples.values(key);
        QgsRubberBand *rubberBand = new QgsRubberBand(mapCanvas,QGis::Polygon);
        vlayer->removeSelection();
        vlayer->setSelectedFeatures(ids.toSet());
        QgsFeatureList features = vlayer->selectedFeatures();
        vlayer->removeSelection();

        QColor color /*= trainingSampleForm->getColor(key)*/;
        rubberBand->setColor(color);
        rubberBand->setBrushStyle(Qt::Dense3Pattern);

        foreach (QgsFeature feature, features) {
            rubberBand->addGeometry(feature.geometry(),vlayer);
        }

        rubberBands<<rubberBand;
    }

}

void CDTMapToolSelectTrainingSamples::addSingleSample(qint64 id)
{    
//    QString currentCategoryName = trainingSampleForm->currentName();
//    if (currentCategoryName.isNull())
//        return;

//    if (samples.contains(currentCategoryName,id))
//        samples.remove(currentCategoryName,id);
//    else
//        samples.insert(currentCategoryName,id);
//    updateRubber();
}
