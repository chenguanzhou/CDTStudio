#ifndef CDTIMAGELAYER_H
#define CDTIMAGELAYER_H

#include "cdtbaselayer.h"

class QAction;
class CDTProjectTreeItem;
class CDTExtractionLayer;
class CDTSegmentationLayer;
class CategoryInformation;

typedef QList<CategoryInformation> CDTCategoryInformationList;

class CDTImageLayer:public CDTBaseLayer
{
    Q_OBJECT
    Q_CLASSINFO("CDTImageLayer","Image")
    Q_CLASSINFO("tableName","imagelayer")
    Q_PROPERTY(QString Source READ path DESIGNABLE true USER true)
public:
    explicit CDTImageLayer(QUuid uuid, QObject *parent = 0);
    ~CDTImageLayer();

    friend QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image);
    friend QDataStream &operator>>(QDataStream &in, CDTImageLayer &image);
    friend class CDTSegmentationLayer;

    void setNameAndPath(const QString& name,const QString& path);
    void setCategoryInfo(const CDTCategoryInformationList& info);
    QString path()const;
    int bandCount()const;    

    static QList<CDTImageLayer *> getLayers();
    static CDTImageLayer *getLayer(const QUuid& id);

signals:
    void removeImageLayer(CDTImageLayer*);

public slots:
//    void setRenderer();
    void addExtraction();
    void addSegmentation();    
    void remove();
    void removeExtraction(CDTExtractionLayer*);
    void removeAllExtractionLayers();
    void removeSegmentation(CDTSegmentationLayer*);
    void removeAllSegmentationLayers();
    void setLayerOpacity(int opacity);

    void redBandChanged(int bandIDFrom0);
    void greenBandChanged(int bandIDFrom0);
    void blueBandChanged(int bandIDFrom0);
private:        
    void addExtraction(CDTExtractionLayer* extraction);
    void addSegmentation(CDTSegmentationLayer* segmentation);

    void updateMultiBandRenderer();

private:
    QVector<CDTExtractionLayer *>   extractions;
    QVector<CDTSegmentationLayer *> segmentations;

    CDTProjectTreeItem *segmentationsRoot;
    CDTProjectTreeItem *extractionRoot;

    QWidget *multibandSelectionWidget;

    int rBandID,gBandID,bBandID;

    static QList<CDTImageLayer *> layers;
};

QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image);
QDataStream &operator>>(QDataStream &in, CDTImageLayer &image);


class CategoryInformation
{
public:
    CategoryInformation(QUuid uuid=QUuid(),QString name=QString(),QColor clr=QColor())
        :id(uuid),categoryName(name),color(clr){}
    QUuid id;
    QString categoryName;
    QColor color;
};

QDataStream &operator <<(QDataStream &out,const CategoryInformation &categoryInformation);
QDataStream &operator >>(QDataStream &in, CategoryInformation &categoryInformation);
#endif // CDTIMAGELAYER_H
