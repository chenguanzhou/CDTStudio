#include "overlay.h"
#include "qgsgeometry.h"

Overlay::Overlay(QObject *parent)
    :CDTVectorChangeDetectionInterface(parent)
{
}

Overlay::~Overlay()
{
}

QString Overlay::methodName() const
{
    return tr("Overlay");
}

void Overlay::detect(
        QgsVectorLayer *layerT1,
        QgsVectorLayer *layerT2,
        QgsVectorLayer *layerResult,
        QString fieldNameT1,
        QString fieldNameT2)
{
    QgsFeatureIterator iterT1 = layerT1->getFeatures();
    QgsFeatureIterator iterT2 = layerT2->getFeatures();

    qDebug()<<"hehe";
    QgsFeature fT1,fT2;
    while(iterT1.nextFeature(fT1))
    {
        QgsGeometry *geoT1 = fT1.geometry();
        QgsRectangle rectT1 = geoT1->boundingBox();

        iterT2.rewind();
        while(iterT2.nextFeature(fT2))
        {
            QgsGeometry *geoT2 = fT2.geometry();
            QgsRectangle rectT2 = geoT2->boundingBox();
            if (!rectT1.intersects(rectT2))//No intersect for their bounding box
                continue;
            if (!geoT1->intersects(geoT2))//No intersect for their own
                continue;
            QgsGeometry *intersection = geoT1->intersection(geoT2);
            qDebug()<<intersection->type();
        }
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Overlay, Overlay)
#endif // QT_VERSION < 0x050000
