#ifndef CDTIMAGELAYER_H
#define CDTIMAGELAYER_H

#include "cdtbaselayer.h"
#include "cdttrainingsampledockwidget.h"

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
    Q_PROPERTY(QString Name READ name WRITE setName DESIGNABLE true USER true)
    Q_PROPERTY(QString Source READ path DESIGNABLE true USER true)
public:
    explicit CDTImageLayer(QUuid uuid, QObject *parent = 0);
    ~CDTImageLayer();

    friend QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image);
    friend QDataStream &operator>>(QDataStream &in, CDTImageLayer &image);
    friend class CDTSegmentationLayer;

    void setName(const QString& name);
    void setNameAndPath(const QString& name,const QString& path);
    void setCategoryInfo(const CDTCategoryInformationList& info);
    QString path()const;
    QString name()const;
    int bandCount()const;    

    static QList<CDTImageLayer *> getLayers();
    static CDTImageLayer *getLayer(const QUuid& id);

signals:
//    void imageLayerChanged();
    void removeImageLayer(CDTImageLayer*);

public slots:        
    void addExtraction();
    void addSegmentation();    
    void remove();
    void removeExtraction(CDTExtractionLayer*);
    void removeAllExtractionLayers();
    void removeSegmentation(CDTSegmentationLayer*);
    void removeAllSegmentationLayers();
    void rename();
//    void onContextMenuRequest(QWidget *parent);

private:        
    void addExtraction(CDTExtractionLayer* extraction);
    void addSegmentation(CDTSegmentationLayer* segmentation);

private:
    QVector<CDTExtractionLayer *>   extractions;
    QVector<CDTSegmentationLayer *> segmentations;

//    QAction *actionRename;
//    QAction *actionRemoveImage;
//    QAction *actionAddExtractionLayer;
//    QAction *actionRemoveAllExtractions;
//    QAction *actionAddSegmentationLayer;
//    QAction *actionRemoveAllSegmentations;

    CDTProjectTreeItem *segmentationsRoot;
    CDTProjectTreeItem *extractionRoot;

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
