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

QStandardItem *CDTBaseLayer::standardKeyItem() const
{
    return keyItem;
}

//QList<QStandardItem *> CDTBaseLayer::standardItems() const
//{
//    return QList<QStandardItem *>()<<(QStandardItem *)keyItem<<(QStandardItem *)valueItem;
//}

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
