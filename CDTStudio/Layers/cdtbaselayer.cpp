#include "cdtbaselayer.h"
#include "stable.h"
#include "cdtprojecttreeitem.h"
#include "cdtfilesystem.h"
#include "cdtprojectlayer.h"

CDTBaseLayer::CDTBaseLayer(QUuid uuid, QObject *parent) :
    QObject(parent),
    mapCanvasLayer(NULL),
    mapCanvas(NULL)
{
    this->uuid = uuid;
    if (parent) connect(this,SIGNAL(appendLayers(QList<QgsMapLayer*> )),parent,SIGNAL(appendLayers(QList<QgsMapLayer*>)));
    if (parent) connect(this,SIGNAL(removeLayer(QList<QgsMapLayer*> )),parent,SIGNAL(removeLayer(QList<QgsMapLayer*>)));
    if (parent) connect(this,SIGNAL(layerChanged()),parent,SIGNAL(layerChanged()));
    if (parent) mapCanvas = ((CDTBaseLayer*)parent)->mapCanvas;
}

CDTBaseLayer::~CDTBaseLayer()
{

}

void CDTBaseLayer::onContextMenuRequest(QWidget *parent)
{
    QMenu* menu =new QMenu(parent);    
    connect(menu,SIGNAL(aboutToHide()),SLOT(onMenuAboutToHide()));
    foreach (QList<QAction *> list, actions) {
        menu->addActions(list);
        menu->addSeparator();
    }
    menu->exec(QCursor::pos());
}

CDTProjectTreeItem *CDTBaseLayer::keyItem() const
{
    return treeKeyItem;
}

QStandardItem *CDTBaseLayer::standardKeyItem() const
{
    return treeKeyItem;
}

QgsMapLayer *CDTBaseLayer::canvasLayer() const
{
    return mapCanvasLayer;
}

QgsMapCanvas *CDTBaseLayer::canvas() const
{
    return mapCanvas;
}

CDTProjectLayer *CDTBaseLayer::rootProject() const
{
    QObject *obj = (QObject *)this;
    while (obj->parent())
        obj = obj->parent();
    return (CDTProjectLayer *)obj;
}

CDTFileSystem *CDTBaseLayer::fileSystem() const
{
    return rootProject()->fileSystem;
}

QString CDTBaseLayer::tableName() const
{
    int index = metaObject()->indexOfClassInfo("tableName");
    if (index == -1)
    {
        logger()->error("Get table name of layer:%1 failed!",metaObject()->className());
        return 0;
    }
    return metaObject()->classInfo(index).value();
}

QList<QList<QAction *> > CDTBaseLayer::allActions() const
{
    return actions;
}

QObject *CDTBaseLayer::getAncestor(const char *className)
{
    QObject *obj = this;
    while (obj)
    {
        if (qstrcmp(obj->metaObject()->className(),className)==0)
            return obj;
        obj = obj->parent();
    }
    return NULL;
}

void CDTBaseLayer::setID(QUuid id)
{
    uuid = id;
}

void CDTBaseLayer::setKeyItem(CDTProjectTreeItem *item)
{
    treeKeyItem = item;
}

void CDTBaseLayer::setActions(QList<QList<QAction *> > actions)
{
    this->actions = actions;
}

void CDTBaseLayer::setCanvasLayer(QgsMapLayer *layer)
{
    if(layer == NULL)
        return;

    if (mapCanvasLayer)
    {
        QgsMapLayerRegistry::instance()->removeMapLayer(mapCanvasLayer->id());
        delete mapCanvasLayer;
    }

    mapCanvasLayer = layer;
    QgsMapLayerRegistry::instance()->addMapLayer(mapCanvasLayer);
    treeKeyItem->setMapLayer(mapCanvasLayer);
}

void CDTBaseLayer::setMapCanvas(QgsMapCanvas *canvas)
{
    mapCanvas = canvas;
}

void CDTBaseLayer::onMenuAboutToHide()
{
    QMenu *menu = qobject_cast<QMenu *>(sender());
    if (menu)
    {
        foreach (QAction *action, menu->actions()) {
            QWidgetAction *wAction = qobject_cast<QWidgetAction*>(action);
            if (wAction)
            {
                wAction->requestWidget(NULL);
                QWidget *widget = wAction->defaultWidget();
                menu->removeAction(wAction);
                wAction->setDefaultWidget(widget);
            }
        }
    }
}
