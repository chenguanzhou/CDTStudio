#include "cdtmaptoolselecttrainingsamples.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "mainwindow.h"
#include "cdtcategorytoolbar.h"

CDTMapToolSelectTrainingSamples::CDTMapToolSelectTrainingSamples(QgsMapCanvas *canvas) :
    QgsMapTool(canvas),
    mapCanvas(canvas),
    toolBar(new CDTCategoryToolBar(tr("Select training samples"),MainWindow::getMainWindow())),
    model(new QSqlQueryModel(this))
{    
    toolBar->setFloatable(true);
    toolBar->setMovable(true);
    toolBar->show();
    toolBar->raise();

    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("CDTCategoryToolBar");
    QByteArray geometry = setting.value("Geometry").toByteArray();
    setting.endGroup();
    if (geometry.isEmpty())
    {
        QPoint ptGlobal = MainWindow::getCurrentMapCanvas()->mapToGlobal(MainWindow::getCurrentMapCanvas()->rect().center());
        toolBar->move(MainWindow::getMainWindow()->mapFromGlobal(ptGlobal));
    }
    else
        toolBar->restoreGeometry(geometry);

    comboBoxCategory = new QComboBox(toolBar);
    comboBoxCategory->setModel(model);

    QWidgetAction *actionLabel = new QWidgetAction(this);
    QWidgetAction *actionCombo = new QWidgetAction(this);
    actionLabel->setDefaultWidget(new QLabel(tr("Category"),toolBar));
    actionCombo->setDefaultWidget(comboBoxCategory);

    toolBar->addAction(actionLabel);
    toolBar->addAction(actionCombo);
    MainWindow::getMainWindow()->addToolBar(toolBar);
}

CDTMapToolSelectTrainingSamples::~CDTMapToolSelectTrainingSamples()
{
    clearRubberBand();
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("CDTCategoryToolBar");
    setting.setValue("Geometry",toolBar->saveGeometry());
    setting.endGroup();
    if (toolBar)
    {
        MainWindow::getMainWindow()->removeToolBar(toolBar);
        delete toolBar;
    }
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
//        if (!mReadOnly)
//        {
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

            if ( mapCanvas->mapSettings().hasCrsTransformEnabled() )
            {
                QgsCoordinateTransform ct( mapCanvas->mapSettings().destinationCrs(), vlayer->crs() );
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
                    closestFeatureId = f.attribute("GridCode").toInt();
                }
            }

            if ( foundSingleFeature )
                addSingleSample( closestFeatureId );
//        }
    }
}

void CDTMapToolSelectTrainingSamples::setSampleID(QUuid id)
{
    if (id.isNull()) return;    
    clearRubberBand();
    
    model->clear();
    model->setQuery(QString("select name,id from category where imageid = "
                            "(select imageid from segmentationlayer where id = "
                            "(select segmentationid from sample_segmentation "
                            "where id = '%1'))").arg(id),
                    QSqlDatabase::database("category"));

    sampleID = id;
    updateRubber();
}

//void CDTMapToolSelectTrainingSamples::setReadOnly(bool readOnly)
//{
//    mReadOnly = readOnly;
//}

void CDTMapToolSelectTrainingSamples::clearRubberBand()
{
    foreach (QgsRubberBand* band, rubberBands.values()) {
        mapCanvas->scene()->removeItem(band);
        if (band) delete band;
    }
    rubberBands.clear();
}

void CDTMapToolSelectTrainingSamples::updateRubber()
{
    //    clearRubberBand();

    QgsVectorLayer* vlayer = NULL;
    if ( !mapCanvas->currentLayer()
         || ( vlayer = qobject_cast<QgsVectorLayer *>( mapCanvas->currentLayer() ) ) == NULL )
    {
        qWarning()<<"Current Layer is not a vector layer!";
        return;
    }


    QSqlQuery query(QSqlDatabase::database("category"));
    QList<qint64> newRubberBandsID;

    query.prepare("select object_samples.objectid,category.color from object_samples natural join category where object_samples.categoryid = category.id and object_samples.sampleID = ?");
    query.bindValue(0,sampleID.toString());
    query.exec();
    while (query.next()) {
        qint64 objectID = query.value(0).toInt();
        newRubberBandsID<<objectID;
        if (!rubberBands.keys().contains(objectID))
        {
            QColor color = query.value(1).value<QColor>();
            QgsRubberBand *rubberBand = new QgsRubberBand(mapCanvas,QGis::Polygon);

            rubberBand->setColor(color);
            rubberBand->setBrushStyle(Qt::Dense3Pattern);
            QgsFeature f;
            QgsFeatureIterator features = vlayer->getFeatures(QgsFeatureRequest().setFilterExpression("GridCode="+QString::number(objectID)));
            while(features.nextFeature(f))
            {
                rubberBand->addGeometry(f.geometry(),vlayer);
            }
            rubberBands.insert(objectID,rubberBand);
        }
    }

    //    foreach (int64 objID, rubberBands.keys()) {
    QList<qint64> objects = rubberBands.keys();
    for (int i=0;i<objects.size();++i)
    {
        int64 objID = rubberBands.keys().at(i);
        if (!newRubberBandsID.contains(objID))
        {
            QgsRubberBand *band = rubberBands.value(objID);
            mapCanvas->scene()->removeItem(band);
            if (band) delete band;
            rubberBands.remove(objID);
        }
    }
    //    }
}

void CDTMapToolSelectTrainingSamples::addSingleSample(qint64 id)
{    
    if (sampleID.isNull())
    {
        qWarning()<<tr("No sample selected!");
        return;
    }

//    QUuid categoryID = MainWindow::getSampleDockWidget()->currentCategoryID();
//    if (categoryID.isNull())
//    {
//        qWarning()<<tr("No category selected!");
//        return;
//    }
    qDebug()<<"id:"<<id;
    QUuid categoryID = QUuid(model->data(model->index(comboBoxCategory->currentIndex(),1)).toString());
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("select * from object_samples where objectid = ?  and sampleID = ?");
    query.bindValue(0,id);
    //    query.bindValue(1,categoryID.toString());
    query.bindValue(1,sampleID.toString());
    query.exec();
    if (query.next())
    {//Exsist in table
        query.prepare("delete from object_samples where objectid = ?  and sampleID = ?");
        query.bindValue(0,id);
        //        query.bindValue(1,categoryID.toString());
        query.bindValue(1,sampleID.toString());
        query.exec();
    }
    else
    {//Not exist
        query.prepare("insert into object_samples values(?,?,?)");
        query.bindValue(0,id);
        query.bindValue(1,categoryID.toString());
        query.bindValue(2,sampleID.toString());
        query.exec();
    }

    updateRubber();
}
