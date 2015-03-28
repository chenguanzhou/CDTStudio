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

    friend class CDTProjectWidget;
    friend class MainWindow;

    CDTProjectLayer     *rootProject()const;
    CDTProjectTreeItem  *keyItem()const;
    QStandardItem       *standardKeyItem()const;
    QgsMapLayer         *canvasLayer()const;
    QgsMapCanvas        *canvas()const;
    CDTFileSystem       *fileSystem()const;
    inline QUuid        id()const{return uuid;}
    virtual QString     name()const = 0;
    QString             tableName() const;

    ///Find and return the ancestor object of the this class's instace, whose name is className.
    QObject *getAncestor(const char* className);

protected:
    virtual void        onContextMenuRequest(QWidget *parent);

    QList<QList<QAction *> > allActions()const;

protected slots:
    void setID(QUuid id);
    void setKeyItem(CDTProjectTreeItem *item);
    void setActions(QList<QList<QAction *> > actions);
    void setCanvasLayer(QgsMapLayer* layer);
    void setMapCanvas(QgsMapCanvas* canvas);

signals:
    void appendLayers(QList<QgsMapLayer*> layer);
    void removeLayer(QList<QgsMapLayer*> layer);
    void layerChanged();
public slots:


private slots:
    void onMenuAboutToHide();


private:
    QUuid               uuid;
    QString             tblName;
    CDTProjectTreeItem  *treeKeyItem;
    QgsMapLayer         *mapCanvasLayer;
    QgsMapCanvas        *mapCanvas;

    QList<QList<QAction *> > actions;
};

#endif // CDTBASELAYER_H
