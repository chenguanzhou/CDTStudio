#include "cdtpbcdbinarylayer.h"
#include "stable.h"
#include "cdtfilesystem.h"
#include "cdtprojecttreeitem.h"

CDTPBCDBinaryLayer::CDTPBCDBinaryLayer(QUuid id, QObject *parent) :
    CDTPixelChangeLayer(id,parent)
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

QStringList CDTPBCDBinaryLayer::files() const
{
    return QStringList()<<params().value("diffImageID").toString();
}

void CDTPBCDBinaryLayer::onContextMenuRequest(QWidget *parent)
{
    Q_UNUSED(parent);
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

    setCanvasLayer(newLayer);

    CDTPixelChangeLayer::initLayer(name,image_t1,image_t2,params);

    emit layerChanged();
    emit appendLayers(QList<QgsMapLayer*>()<<canvasLayer());
}

