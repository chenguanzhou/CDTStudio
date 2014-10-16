#include "cdtpbcdbinarylayer.h"
#include "stable.h"
#include "cdtfilesystem.h"
#include "cdtprojecttreeitem.h"

CDTPBCDBinaryLayer::CDTPBCDBinaryLayer(QUuid id, QObject *parent) :
    CDTChangeLayer(id,parent)
{
    type = PBCD_BINARY;
}

QString CDTPBCDBinaryLayer::diffImage() const
{
    return params().value("diffImageID").toString();
}

double CDTPBCDBinaryLayer::posT() const
{
    return params().value("positiveThreshold").toDouble();
}

double CDTPBCDBinaryLayer::negT() const
{
    return params().value("negetiveThreshold").toDouble();
}

void CDTPBCDBinaryLayer::onContextMenuRequest(QWidget *parent)
{

}

void CDTPBCDBinaryLayer::initLayer(const QString &name, const QString &image_t1, const QString &image_t2, const QVariantMap &params)
{
    QString tempDiffPath;
    this->fileSystem()->getFile(params.value("diffImageID").toString(),tempDiffPath);
    QgsRasterLayer *newLayer = new QgsRasterLayer(tempDiffPath);
    if (!newLayer->isValid())
    {
        QMessageBox::critical(NULL,tr("Error"),tr("Open diff image ")+tempDiffPath+tr(" failed!"));
        delete newLayer;
        return;
    }

    if (mapCanvasLayer)
    {
        QgsMapLayerRegistry::instance()->removeMapLayer(mapCanvasLayer->id());
        delete mapCanvasLayer;
    }
    mapCanvasLayer = newLayer;
    QgsMapLayerRegistry::instance()->addMapLayer(mapCanvasLayer);
    keyItem->setMapLayer(mapCanvasLayer);

    CDTChangeLayer::initLayer(name,image_t1,image_t2,params);

    emit layerChanged();
}

