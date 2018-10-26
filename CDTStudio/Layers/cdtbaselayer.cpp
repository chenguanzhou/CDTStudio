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

    connect(this,SIGNAL(nameChanged(QString)),this,SIGNAL(layerChanged()));
}

CDTBaseLayer::~CDTBaseLayer()
{

}

CDTProjectLayer *CDTBaseLayer::rootProject() const
{
    QObject *obj = (QObject *)this;
    while (obj->parent())
        obj = obj->parent();
    return (CDTProjectLayer *)obj;
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

CDTFileSystem *CDTBaseLayer::fileSystem() const
{
    return rootProject()->fileSystem;
}

QUuid CDTBaseLayer::id() const{return uuid;}

QString CDTBaseLayer::name() const
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare(QString("select name from %1 where id =?").arg(tableName()));
    query.addBindValue(id().toString());
    query.exec();
    query.next();
    return query.value(0).toString();
}

QString CDTBaseLayer::tableName() const
{
    int index = metaObject()->indexOfClassInfo("tableName");
    if (index == -1)
    {
        qCritical("Get table name of layer:%1 failed!",metaObject()->className());
        return QString();
    }
    return metaObject()->classInfo(index).value();
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

QList<QList<QAction *> > CDTBaseLayer::allActions() const
{
    return actions;
}

void CDTBaseLayer::onContextMenuRequest(QWidget *parent)
{
    QMenu* menu =new QMenu(parent);
    connect(menu,SIGNAL(aboutToHide()),SLOT(onMenuAboutToHide()));

    if (!widgetActions.isEmpty())
    {
        QWidgetAction *widgetAction = new QWidgetAction(this);
        QWidget *widget = new QWidget(parent);
        QFormLayout *layout = new QFormLayout(widget);
        layout->setMargin(3);
        widget->setLayout(layout);
        foreach (auto pair, widgetActions) {
            layout->addRow(pair.first,pair.second);
        }
        widgetAction->setDefaultWidget(widget);
        menu->addAction(widgetAction);
        menu->addSeparator();
    }

    foreach (QList<QAction *> list, actions) {
        menu->addActions(list);
        menu->addSeparator();
    }
    menu->exec(QCursor::pos());
}

void CDTBaseLayer::setName(const QString &name)
{
    if (this->name() == name)
        return;

    if (name.isEmpty())
        return;

    bool ret = false;
    QSqlQuery query(QSqlDatabase::database("category"));
    ret = query.prepare(QString("UPDATE %1 set name = ? where id =?").arg(tableName()));
    if (ret==false) return;
    query.bindValue(0,name);
    query.bindValue(1,this->id().toString());
    ret = query.exec();
    if (ret==false) return;

    keyItem()->setText(name);
    emit nameChanged(name);
}

void CDTBaseLayer::rename()
{
    bool ok;
    QString text = QInputDialog::getText(
                NULL, tr("Input New Name"),
                tr("Rename:"), QLineEdit::Normal,
                this->name(), &ok);
    if (ok && !text.isEmpty())
        setName(text);
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

void CDTBaseLayer::setWidgetActions(QList<QPair<QLabel *, QWidget *> > actions)
{
    this->widgetActions = actions;
}

void CDTBaseLayer::setCanvasLayer(QgsMapLayer *layer, bool addToLegend)
{
    if(layer == NULL)
        return;

    if (mapCanvasLayer)
    {
//        QgsMapLayerRegistry::instance()->removeMapLayer(mapCanvasLayer->id());
        delete mapCanvasLayer;
    }

    mapCanvasLayer = layer;
//    QgsMapLayerRegistry::instance()->addMapLayer(mapCanvasLayer,addToLegend);
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
