#ifndef CDTIMAGELAYER_H
#define CDTIMAGELAYER_H

#include <QtCore>
#include <QAction>
#include "cdtsegmentationlayer.h"
#include "cdtclassification.h"
#include "cdtprojecttreeitem.h"
#include "cdtbaseobject.h"


class CDTImageLayer:public CDTBaseObject
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
public:
    explicit CDTImageLayer(QObject *parent = 0);

    friend QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image);
    friend QDataStream &operator>>(QDataStream &in, CDTImageLayer &image);

    void setPath(const QString& path);
    void setName(const QString& name);
    inline QString path()const{return m_path;}
    inline QString name()const{return m_name;}    

    void addSegmentation(CDTSegmentationLayer* segmentation);

signals:
    void pathChanged(QString);
    void nameChanged(QString);
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

private:
    QString m_path;
    QString m_name;
    QVector<CDTSegmentationLayer *> segmentations;
    QAction* addSegmentationLayer;
    QAction* removeImage;
    QAction* removeAllSegmentations;
    QAction* actionRename;

    CDTProjectTreeItem* segmentationsroot;

};

QDataStream &operator<<(QDataStream &out, const CDTImageLayer &image);
QDataStream &operator>>(QDataStream &in, CDTImageLayer &image);
#endif // CDTIMAGELAYER_H
