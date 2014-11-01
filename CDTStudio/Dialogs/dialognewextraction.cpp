#include "dialognewextraction.h"
#include "ui_dialognewextraction.h"
#include "cdtfilesystem.h"
#include "stable.h"

DialogNewExtraction::DialogNewExtraction(
        const QString &inputImage,
        CDTFileSystem *fileSys,
        QWidget *parent) :
    inputImagePath(inputImage),
    fileSystem(fileSys),
    QDialog(parent),
    ui(new Ui::DialogNewExtraction)
{
    ui->setupUi(this);
    shapefileTempPath = QDir::tempPath()+"/"+QUuid::createUuid().toString()+".shp";
    ui->colorPicker->setStandardColors();
    ui->borderColorPicker->setStandardColors();

    connect(this,SIGNAL(accepted()),SLOT(onAccepted()));
}


DialogNewExtraction::~DialogNewExtraction()
{
    delete ui;
}

QString DialogNewExtraction::name() const
{
    return ui->lineEditName->text();
}

QColor DialogNewExtraction::color() const
{
    return ui->colorPicker->currentColor();
}

QColor DialogNewExtraction::borderColor() const
{
    return ui->borderColorPicker->currentColor();
}

double DialogNewExtraction::opacity() const
{
    return ui->doubleSpinBoxOpacity->value();
}

QString DialogNewExtraction::fileID() const
{
    return shapefileID;
}

void DialogNewExtraction::onAccepted()
{
    GDALAllRegister();
    OGRRegisterAll();

    GDALDataset *poImageDS = (GDALDataset *)GDALOpen(inputImagePath.toUtf8().constData(),GA_ReadOnly);
    Q_ASSERT(poImageDS);

    OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
    Q_ASSERT(poDriver);

    OGRDataSource* poDS = poDriver->CreateDataSource(shapefileTempPath.toUtf8().constData(),NULL);
    Q_ASSERT(poDS);
    OGRSpatialReference *reference = new OGRSpatialReference(poImageDS->GetProjectionRef());
    OGRLayer *layer = poDS->CreateLayer("extraction",reference,wkbPolygon,NULL);
    Q_ASSERT(layer);
    OGRFieldDefn oField( "id", OFTInteger );
    oField.SetWidth(10);
    if( layer->CreateField( &oField ) != OGRERR_NONE )
    {
        qWarning()<< "Creating Name field failed.\n" ;
        return;
    }

    OGRDataSource::DestroyDataSource(poDS);
    reference->Release();
    GDALClose(poImageDS);
//    OGRCleanupAll();

    shapefileID = QUuid::createUuid().toString();
    fileSystem->registerFile(shapefileID,shapefileTempPath,QString(),QString()
                             ,CDTFileSystem::getShapefileAffaliated(shapefileTempPath));
}
