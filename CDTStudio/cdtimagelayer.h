#ifndef CDTIMAGELAYER_H
#define CDTIMAGELAYER_H

#include <QtCore>
#include <QAction>
#include "cdtsegmentationlayer.h"
#include "cdtclassification.h"
#include "cdtprojecttreeitem.h"
#include "cdtbaseobject.h"
#include "cdtsampledockwidget.h"

class CDTImageLayer:public CDTBaseObject
{
    Q_OBJECT
public:
    explicit CDTImageLayer(QUuid uuid, QObject *parent = 0);
    ~CDTImageLayer();

    friend QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image);
    friend QDataStream &operator>>(QDataStream &in, CDTImageLayer &image);
    friend class CDTTrainingSamplesForm;
    friend class CDTSegmentationLayer;

    void setName(const QString& name);
    void setNameAndPath(const QString& name,const QString& path);
    void setCategoryInfo(const CDTCategoryInformationList& info);
    QString path()const;
    QString name()const;
    int bandCount()const;

    void addSegmentation(CDTSegmentationLayer* segmentation);

    static QList<CDTImageLayer *> getLayers();
    static CDTImageLayer *getLayer(const QUuid& id);

signals:
    void imageLayerChanged();
    void removeImageLayer(CDTImageLayer*);
public slots:
    void updateTreeModel(CDTProjectTreeItem* parent);
    void onContextMenuRequest(QWidget *parent);
    void addSegmentation();
    void remove();
    void removeSegmentation(CDTSegmentationLayer*);
    void removeAllSegmentationLayers();
    void onActionRename();
    void onActionCategoryInformation();

private:        
//    QString m_path;
//    QString m_name;
    QVector<CDTSegmentationLayer *> segmentations;

    QAction* addSegmentationLayer;
    QAction* removeImage;
    QAction* removeAllSegmentations;
    QAction* actionRename;
    QAction* actionCategoryInformation;

    CDTProjectTreeItem* segmentationsroot;
    CDTTrainingSamplesForm* trainingForm;

    static QList<CDTImageLayer *> layers;
};



QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image);
QDataStream &operator>>(QDataStream &in, CDTImageLayer &image);
#endif // CDTIMAGELAYER_H
