#ifndef CDTBASEOBJECT_H
#define CDTBASEOBJECT_H

#include <QObject>
#include <QUuid>
class CDTProjectTreeItem;
class CDTFileSystem;
class CDTProject;
class QStandardItem;
class QgsMapLayer;
class QgsMapCanvas;

class CDTBaseObject : public QObject
{
    Q_OBJECT
public:
    explicit CDTBaseObject(QUuid uuid,QObject *parent = 0);
    virtual ~CDTBaseObject();

    virtual void onContextMenuRequest(QWidget *parent) = 0;
    QList<QStandardItem *> standardItems()const;
    QgsMapLayer     *canvasLayer()const;
    QgsMapCanvas    *canvas()const;
    inline QUuid    id()const{return uuid;}
    CDTProject      *rootProject()const;
    CDTFileSystem   *fileSystem()const;

signals:
    void appendLayers(QList<QgsMapLayer*> layer);
    void removeLayer(QList<QgsMapLayer*> layer);
public slots:
    void setMapCanvas(QgsMapCanvas* canvas);
protected:
    QUuid               uuid;
    CDTProjectTreeItem  *keyItem;
    CDTProjectTreeItem  *valueItem;
    QgsMapLayer         *mapCanvasLayer;
    QgsMapCanvas        *mapCanvas;
};

#endif // CDTBASEOBJECT_H
