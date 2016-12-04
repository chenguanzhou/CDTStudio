#include "dialognewextraction.h"
#include "ui_dialognewextraction.h"
#include "stable.h"
#include "cdtfilesystem.h"
#include "cdtextractionlayer.h"
#include "cdtlayernamevalidator.h"

DialogNewExtraction::DialogNewExtraction(
        QUuid imageID,
        const QString &inputImage,
        CDTFileSystem *fileSys,
        QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewExtraction),
    inputImagePath(inputImage),
    fileSystem(fileSys)
{
    ui->setupUi(this);
    shapefileTempPath = QDir::tempPath()+"/"+QUuid::createUuid().toString()+".shp";
    ui->colorPicker->setStandardColors();
    ui->borderColorPicker->setStandardColors();

    connect(this,SIGNAL(accepted()),SLOT(onAccepted()));

    int index = CDTExtractionLayer::staticMetaObject.indexOfClassInfo("tableName");
    if (index != -1)
    {
        CDTLayerNameValidator *validator = new CDTLayerNameValidator
                (QSqlDatabase::database("category"),"name",CDTExtractionLayer::staticMetaObject.classInfo(index).value(),QString("imageid='%1'").arg(imageID.toString()));
        ui->lineEditName->setValidator(validator);
    }
    ui->lineEditName->setText(tr("Untitled"));
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

    GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    Q_ASSERT(poDriver);

    GDALDataset* poDS = poDriver->Create(shapefileTempPath.toUtf8().constData(),0,0,0,GDT_Unknown,NULL);
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

    GDALClose(poDS);
    reference->Release();
    GDALClose(poImageDS);
//    OGRCleanupAll();

    shapefileID = QUuid::createUuid().toString();
    fileSystem->registerFile(shapefileID,shapefileTempPath,QString(),QString()
                             ,CDTFileSystem::getShapefileAffaliated(shapefileTempPath));
}
