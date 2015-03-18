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
    auto getAllFeatures = [](QgsVectorLayer *layer)->QList<QgsFeature>
    {
        QList<QgsFeature> features;
        QgsFeature f;
        QgsFeatureIterator iter = layer->getFeatures();
        while(iter.nextFeature(f))
        {
            features.push_back(f);
        }
        return features;
    };

    QList<QgsFeature> featuresT1 = getAllFeatures(layerT1);
    QList<QgsFeature> featuresT2 = getAllFeatures(layerT2);

    layerResult->startEditing();

    foreach (QgsFeature f1, featuresT1) {
        auto gT1 = f1.geometry();
        auto rectT1 = gT1->boundingBox();
        QString beforeCategory = f1.attribute(fieldNameT1).toString();
        foreach (QgsFeature f2, featuresT2) {
            auto gT2 = f2.geometry();
            auto rectT2 = gT2->boundingBox();

            if (rectT1.intersects(rectT2) && gT1->intersects(gT2))
            {
                //Intersect
                QString afterCategory = f2.attribute(fieldNameT2).toString();
                QgsGeometry *intersection = gT1->intersection(gT2);
                std::vector<QgsGeometry *> afterInter;
                getPolygonList(intersection,afterInter);
                foreach (QgsGeometry *g, afterInter) {
                    QgsFeature f(layerResult->pendingFields());
                    f.setGeometry(g);
                    f.setAttribute("before",beforeCategory);
                    f.setAttribute("after",afterCategory);
                    layerResult->addFeature(f,false);
                }
            }
        }
    }

    layerResult->updateExtents();
    layerResult->commitChanges();
}

void Overlay::getPolygonList(QgsGeometry *g,std::vector<QgsGeometry *> &list)
{
    switch (g->wkbType()) {
    case QGis::WKBPolygon:
        list.push_back(g);
        break;
    case QGis::WKBMultiPolygon:
        foreach (QgsPolygon p, g->asMultiPolygon()) {
            std::vector<QgsGeometry *> geos;
            getPolygonList(QgsGeometry::fromPolygon(p),geos);
            list.insert(list.end(),geos.begin(),geos.end());
        }
        break;
    case QGis::WKBUnknown:
        foreach (QgsGeometry *p, g->asGeometryCollection()) {
            std::vector<QgsGeometry *> geos;
            getPolygonList(p,geos);
            list.insert(list.end(),geos.begin(),geos.end());
        }

    default:
        break;
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(Overlay, Overlay)
#endif // QT_VERSION < 0x050000
