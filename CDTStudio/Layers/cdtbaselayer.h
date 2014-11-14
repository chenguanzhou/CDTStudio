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
    CDTProjectLayer *rootProject()const;
    CDTFileSystem   *fileSystem()const;
    inline QUuid    id()const{return uuid;}

    ///Find and return the ancestor object of the this class's instace, whose name is className.
    QObject *getAncestor(const char* className);

signals:
    void appendLayers(QList<QgsMapLayer*> layer);
    void removeLayer(QList<QgsMapLayer*> layer);
    void layerChanged();
public slots:
    void setMapCanvas(QgsMapCanvas* canvas);

private slots:
    void onMenuAboutToHide();
protected:
    QUuid               uuid;
    CDTProjectTreeItem  *keyItem;
//    CDTProjectTreeItem  *valueItem;
    QgsMapLayer         *mapCanvasLayer;
    QgsMapCanvas        *mapCanvas;

    QList<QList<QAction *> > actions;
};

#endif // CDTBASELAYER_H
