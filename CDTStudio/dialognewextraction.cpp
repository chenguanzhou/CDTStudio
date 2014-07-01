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
    poDS->CreateLayer("extraction",reference,wkbPolygon,NULL);
    reference->Release();

    OGRDataSource::DestroyDataSource(poDS);
    GDALClose(poImageDS);
    OGRCleanupAll();

    shapefileID = QUuid::createUuid().toString();
    fileSystem->registerFile(shapefileID,shapefileTempPath,QString(),QString()
                             ,CDTFileSystem::getShapefileAffaliated(shapefileTempPath));
}
