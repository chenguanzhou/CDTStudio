#ifndef CDTBASELAYER_H
#define CDTBASELAYER_H

#include <QObject>
#include <QUuid>

class QAction;
class QStandardItem;
class QgsMapLayer;
class QgsMapCanvas;
class CDTProjectTreeItem;
class CDTFileSystem;
class CDTProjectLayer;

class CDTBaseLayer : public QObject
{
    Q_OBJECT
public:
    explicit CDTBaseLayer(QUuid uuid,QObject *parent = 0);
    virtual ~CDTBaseLayer();

    virtual void onContextMenuRequest(QWidget *parent);
//    QList<QStandardItem *> standardItems()const;
    QStandardItem   *standardKeyItem()const;
    QgsMapLayer     *canvasLayer()const;
    QgsMapCanvas    *canvas()const;
    inline QUuid    id()const{return uuid;}
    CDTProjectLayer      *rootProject()const;
    CDTFileSystem   *fileSystem()const;

signals:
    void appendLayers(QList<QgsMapLayer*> layer);
    void removeLayer(QList<QgsMapLayer*> layer);
    void layerChanged();
public slots:
    void setMapCanvas(QgsMapCanvas* canvas);
protected:
    QUuid               uuid;
    CDTProjectTreeItem  *keyItem;
//    CDTProjectTreeItem  *valueItem;
    QgsMapLayer         *mapCanvasLayer;
    QgsMapCanvas        *mapCanvas;

    QList<QList<QAction *> > actions;
};

#endif // CDTBASELAYER_H
