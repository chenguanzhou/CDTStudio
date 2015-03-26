#ifndef CDTPROJECTTREEITEM_H
#define CDTPROJECTTREEITEM_H

#include <QStandardItem>
#include "cdtbaselayer.h"
#include "qgsmapcanvas.h"

class CDTProjectTreeItem : public QStandardItem
{
public:
    enum CDTItemType{
        PROJECT_ROOT,
        IMAGE_ROOT,
        IMAGE,
        PIXEL_CHANGE_ROOT,
        PIXELCHANGE,
        VECTOR_CHANGE_ROOT,
        VECTOR_CHANGE,
        SEGMENTION_ROOT,
        SEGMENTION,
        EXTRACTION_ROOT,
        EXTRACTION,
        CLASSIFICATION_ROOT,
        CLASSIFICATION,
        METHOD_PARAMS,
        PARAM,
        VALUE
    };

    enum LayerType{
        EMPTY,
        GROUP,
        RASTER,
        VECTOR
    };

    CDTProjectTreeItem(CDTItemType tp,LayerType ly, const QString &text,CDTBaseLayer *crspdObject );
    CDTItemType getType()const {return itemType;}
    CDTBaseLayer* correspondingObject()const {return corrObject;}

    QgsMapLayer *mapLayer()const;
    void setMapLayer(QgsMapLayer* layer);


private:
    void initAlignment();
    void initFont();
    void initColor();
    void initCheckState();
    CDTItemType itemType;
    LayerType layerType;
    CDTBaseLayer* corrObject;
    QgsMapLayer* qgsMapLayer;
};

#endif // CDTPROJECTTREEITEM_H
