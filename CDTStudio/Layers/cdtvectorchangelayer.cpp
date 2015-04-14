#include "cdtvectorchangelayer.h"
#include "stable.h"
#include "cdtprojecttreeitem.h"
#include "cdtfilesystem.h"
#include "cdtvariantconverter.h"
#include "cdtprojectwidget.h"
#include "mainwindow.h"

CDTVectorChangeLayer::CDTVectorChangeLayer(QUuid uuid, QObject *parent)
    : CDTBaseLayer(uuid,parent)
{
    CDTProjectTreeItem *keyItem
            = new CDTProjectTreeItem(CDTProjectTreeItem::VECTOR_CHANGE,CDTProjectTreeItem::VECTOR,QString(),this);
    setKeyItem(keyItem);

    QAction *actionRename =
            new QAction(QIcon(":/Icons/Rename.png"),tr("Rename"),this);
    QAction *actionRemoveLayer =
            new QAction(QIcon(":/Icons/Remove.png"),tr("Remove Vector-based change detection layer"),this);
    QAction *actiobExport =
            new QAction(QIcon(":/Icons/Export.png"),tr("Export shapefile"),this);

    setActions(QList<QList<QAction *> >()
               <<(QList<QAction *>()<<actionRename<<actiobExport)
               <<(QList<QAction *>()<<actionRemoveLayer));

    connect(actionRename,SIGNAL(triggered()),SLOT(rename()));
    connect(actionRemoveLayer,SIGNAL(triggered()),SLOT(remove()));
    connect(actiobExport,SIGNAL(triggered()),SLOT(exportShapefile()));

    connect(this,SIGNAL(removeVectorChangeLayer(CDTVectorChangeLayer*)),this->parent(),SLOT(removeVectorChangeLayer(CDTVectorChangeLayer*)));
}

CDTVectorChangeLayer::~CDTVectorChangeLayer()
{

}

QString CDTVectorChangeLayer::shapefileID() const
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    query.exec("select shapefileid from vector_change where id ='" + this->id().toString() +"'");
    query.next();
    return query.value(0).toString();
}

void CDTVectorChangeLayer::exportShapefile()
{
    fileSystem()->exportFiles(shapefileID());
}

void CDTVectorChangeLayer::remove()
{
    emit removeVectorChangeLayer(this);
}

void CDTVectorChangeLayer::setOriginRenderer()
{
    setRenderer(changeViewRenderer());
}

void CDTVectorChangeLayer::setRenderer(QgsFeatureRendererV2* r)
{
    QgsVectorLayer*p = qobject_cast<QgsVectorLayer*>(canvasLayer());
    if (p)
        p->setRendererV2(r);
}

QgsFeatureRendererV2 *CDTVectorChangeLayer::changeViewRenderer()
{
    auto getCategorySymbol = [](QString value,QString label,QColor color)->QgsRendererCategoryV2
    {
        QgsSimpleFillSymbolLayerV2* symbolLayer = new QgsSimpleFillSymbolLayerV2();
        symbolLayer->setColor(color);
        symbolLayer->setBorderColor(color);
        QgsRendererCategoryV2 rend;
        rend.setValue(value);
        rend.setSymbol(new QgsFillSymbolV2(QgsSymbolLayerV2List()<<symbolLayer));
        rend.setLabel(label);
        return rend;
    };

    QgsCategoryList categoryList;
    categoryList<<getCategorySymbol("Changed","Changed",QColor(255,0,0));
    categoryList<<getCategorySymbol("Unchanged","Unchanged",QColor(0,0,255));

    QgsCategorizedSymbolRendererV2* categorizedSymbolRenderer =
            new QgsCategorizedSymbolRendererV2("ischanged",categoryList);

    return categorizedSymbolRenderer;
}

void CDTVectorChangeLayer::initLayer(
        const QString &name,
        const QString &shapefileID,
        const QString &clsID_T1,
        const QString &clsID_T2,
        const QVariantMap &params)
{
    QString tempShpPath;
    this->fileSystem()->getFile(shapefileID,tempShpPath);
    QgsVectorLayer *newLayer = new QgsVectorLayer(/*shpPath*/tempShpPath,QFileInfo(/*shpPath*/tempShpPath).completeBaseName(),"ogr");
    if (!newLayer->isValid())
    {
        QMessageBox::critical(NULL,tr("Error"),tr("Open shapefile ")+tempShpPath+tr(" failed!"));
        delete newLayer;
        return;
    }

    setCanvasLayer(newLayer);

    standardKeyItem()->setText(name);

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret ;
    ret = query.prepare("insert into vector_change VALUES(?,?,?,?,?,?)");
    if (ret==false)
    {
        logger()->error("Init CDTVectorChangeLayer Failed!");
        return;
    }

    query.bindValue(0,id().toString());
    query.bindValue(1,name);
    query.bindValue(2,shapefileID);
    query.bindValue(3,clsID_T1);
    query.bindValue(4,clsID_T2);
    query.bindValue(5,dataToVariant(params));
    ret = query.exec();

    setOriginRenderer();

    emit nameChanged(name);
    emit appendLayers(QList<QgsMapLayer*>()<<canvasLayer());
    emit layerChanged();    
}



QDataStream &operator<<(QDataStream &out, const CDTVectorChangeLayer &change)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec("select * from vector_change where id ='" + change.id().toString() +"'");
    query.next();

    out <<change.id()         //id
       <<query.value(1).toString() //name
      <<query.value(2).toString() //shapefileID
     <<query.value(3).toString() //clsID_T1
    <<query.value(4).toString() //clsID_T2
    <<query.value(5);//params
    return out;
}

QDataStream &operator>>(QDataStream &in, CDTVectorChangeLayer &change)
{
    QUuid id;
    in>>id;
    change.setID(id);
    QString name,shp,cls1,cls2;
    in>>name>>shp>>cls1>>cls2;
    QVariant temp;
    in>>temp;
    QVariantMap params = temp.toMap();
    change.initLayer(name,shp,cls1,cls2,params);

    return in;
}
