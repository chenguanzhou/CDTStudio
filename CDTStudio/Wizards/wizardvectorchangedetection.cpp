#include "wizardvectorchangedetection.h"
#include "ui_wizardvectorchangedetection.h"

#include <QtCore>
#include <QtSql>
#include <QFileDialog>
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"

WizardVectorChangeDetection::WizardVectorChangeDetection(QUuid projectID, QWidget *parent) :
    QWizard(parent),
    prjID(projectID),
    isValid_Page1(false),
    ui(new Ui::WizardVectorChangeDetection)
{
    ui->setupUi(this);
    initPage1();
}

WizardVectorChangeDetection::~WizardVectorChangeDetection()
{
    delete ui;
}

bool WizardVectorChangeDetection::validateCurrentPage()
{
    if (currentId()==0)
    {
        //Page1
        return isValid_Page1;
    }
    return true;
}

/********************************************/
/*                  Page 1                  */
/********************************************/

void WizardVectorChangeDetection::initPage1()
{
    //Change Detection Name
    connect(ui->lineEditName,SIGNAL(textChanged(QString)),SLOT(updateReport_Page1()));

    //Data Source Tabs
    connect(ui->tabWidgetT1,SIGNAL(currentChanged(int)),SLOT(updateReport_Page1()));
    connect(ui->tabWidgetT2,SIGNAL(currentChanged(int)),SLOT(updateReport_Page1()));

    //Data Source (From Project)
    model_ImageLayer  = new QSqlQueryModel(this);
    modelT1_SegLayer    = new QSqlQueryModel(this);
    modelT1_ClsLayer    = new QSqlQueryModel(this);
    modelT2_SegLayer    = new QSqlQueryModel(this);
    modelT2_ClsLayer    = new QSqlQueryModel(this);

    ui->comboBoxT1_ImageLayer->setModel(model_ImageLayer);
    ui->comboBoxT1_SegLayer->setModel(modelT1_SegLayer);
    ui->comboBoxT1_ClsLayer->setModel(modelT1_ClsLayer);
    ui->comboBoxT2_ImageLayer->setModel(model_ImageLayer);
    ui->comboBoxT2_SegLayer->setModel(modelT2_SegLayer);
    ui->comboBoxT2_ClsLayer->setModel(modelT2_ClsLayer);

    connect(ui->comboBoxT1_ImageLayer,SIGNAL(currentIndexChanged(int)),SLOT(updateT1SegLayer()));
    connect(ui->comboBoxT1_SegLayer,SIGNAL(currentIndexChanged(int)),SLOT(updateT1ClsLayer()));
    connect(ui->comboBoxT2_ImageLayer,SIGNAL(currentIndexChanged(int)),SLOT(updateT2SegLayer()));
    connect(ui->comboBoxT2_SegLayer,SIGNAL(currentIndexChanged(int)),SLOT(updateT2ClsLayer()));

    connect(ui->comboBoxT1_ClsLayer,SIGNAL(currentIndexChanged(int)),SLOT(updateReport_Page1()));
    connect(ui->comboBoxT2_ClsLayer,SIGNAL(currentIndexChanged(int)),SLOT(updateReport_Page1()));
    connect(ui->lineEditName,SIGNAL(textChanged(QString)),SLOT(updateReport_Page1()));

    model_ImageLayer->setQuery(
                QString("select name,id from imagelayer where projectid='%1'")
                .arg(prjID.toString()),QSqlDatabase::database("category"));
    if (ui->comboBoxT2_ImageLayer->count()>=2)
        ui->comboBoxT2_ImageLayer->setCurrentIndex(1);

    //Data Source (From Shapefile)
    connect(ui->lineEditT1_ShpPath,SIGNAL(textChanged(QString)),SLOT(updateReport_Page1()));
    connect(ui->lineEditT2_ShpPath,SIGNAL(textChanged(QString)),SLOT(updateReport_Page1()));
    connect(ui->pushButtonT1_ShpPath,SIGNAL(clicked()),SLOT(onButtonShpT1Clicked()));
    connect(ui->pushButtonT2_ShpPath,SIGNAL(clicked()),SLOT(onButtonShpT2Clicked()));
}

void WizardVectorChangeDetection::updateCombobox(QComboBox *sender,QString IDFieldName,QSqlQueryModel *currentModel,QString tableName)
{
    QSqlQueryModel* senderModel = qobject_cast<QSqlQueryModel*>(sender->model());
    if (!senderModel)
    {
        logger()->error("No model in the QComboBox!");
        return;
    }

    QString id = senderModel->data(
        senderModel->index(
            sender->currentIndex(),1
        )
    ).toString();
    currentModel->setQuery(QString("select name,id from %1 where %2='%3'")
                    .arg(tableName).arg(IDFieldName).arg(id),QSqlDatabase::database("category"));
}

void WizardVectorChangeDetection::updateT1SegLayer()
{
    updateCombobox(qobject_cast<QComboBox*>(sender()),"imageid",modelT1_SegLayer,"segmentationlayer");
}

void WizardVectorChangeDetection::updateT1ClsLayer()
{
    updateCombobox(qobject_cast<QComboBox*>(sender()),"segmentationid",modelT1_ClsLayer,"classificationlayer");
}

void WizardVectorChangeDetection::updateT2SegLayer()
{
    updateCombobox(qobject_cast<QComboBox*>(sender()),"imageid",modelT2_SegLayer,"segmentationlayer");
}

void WizardVectorChangeDetection::updateT2ClsLayer()
{
    updateCombobox(qobject_cast<QComboBox*>(sender()),"segmentationid",modelT2_ClsLayer,"classificationlayer");
}

void WizardVectorChangeDetection::onButtonShpT1Clicked()
{
    QString path = QFileDialog::getOpenFileName(this,tr("Choose a Shapefile as Data Source"),QString(),"ESRI Shapefile (*.shp)");
    if (path.isEmpty())
    {
        return;
    }
    ui->lineEditT1_ShpPath->setText(path);
}

void WizardVectorChangeDetection::onButtonShpT2Clicked()
{
    QString path = QFileDialog::getOpenFileName(this,tr("Choose a Shapefile as Data Source"),QString(),"ESRI Shapefile (*.shp)");
    if (path.isEmpty())
    {
        return;
    }
    ui->lineEditT2_ShpPath->setText(path);
}

void WizardVectorChangeDetection::updateReport_Page1()
{
    isValid_Page1 = false;

    auto getCurrentBoxID = [=](QComboBox* box)->QString
    {
        return box->model()->data(box->model()->index(box->currentIndex(),1))
                .toString();
    };

    QString name = ui->lineEditName->text();

    QString imageid_t1 = getCurrentBoxID(ui->comboBoxT1_ImageLayer);
    QString segid_t1 = getCurrentBoxID(ui->comboBoxT1_SegLayer);
    QString clsid_t1 = getCurrentBoxID(ui->comboBoxT1_ClsLayer);
    QString imageid_t2 = getCurrentBoxID(ui->comboBoxT2_ImageLayer);
    QString segid_t2 = getCurrentBoxID(ui->comboBoxT2_SegLayer);
    QString clsid_t2 = getCurrentBoxID(ui->comboBoxT2_ClsLayer);

    QString shapefile_t1 = ui->lineEditT1_ShpPath->text();
    QString shapefile_t2 = ui->lineEditT2_ShpPath->text();

    bool isUseLayer_t1 = ui->tabWidgetT1->currentIndex()==0;
    bool isUseLayer_t2 = ui->tabWidgetT2->currentIndex()==0;

    try{
        auto isText_Empty = [=](QString content,QString name)
        {
            if (content.isEmpty())
                throw tr("%1 is empty!").arg(name);
        };


        isText_Empty(name,tr("Change Detection's name"));

        if (isUseLayer_t1)
        {
            //layers
            isText_Empty(imageid_t1,tr("Image layer of epoch 1"));
            isText_Empty(segid_t1,tr("Segmentation layer of epoch 1"));
            isText_Empty(clsid_t1,tr("Image layer of epoch 1"));
        }
        else
        {
            //shapefile
            isText_Empty(shapefile_t1,tr("Shapefile of epoch 1"));
        }

        if (isUseLayer_t2)
        {
            //layers
            isText_Empty(imageid_t2,tr("Image layer of epoch 2"));
            isText_Empty(segid_t2,tr("Segmentation layer of epoch 2"));
            isText_Empty(clsid_t2,tr("Image layer of epoch 2"));
        }
        else
        {
            //shapefile
            isText_Empty(shapefile_t2,tr("Shapefile of epoch 2"));
        }

        //Same image when both use layers
        if (isUseLayer_t1 == isUseLayer_t2 && imageid_t1 == imageid_t2)
            throw tr("The images of 2 epoch looks the same!");
    }
    catch(QString msg)
    {
        logger()->warn(msg);
        showErrorText_Page1(msg);
        isValid_Page1 = false;
        return;
    }
    isValid_Page1 = true;
    showCorrectText_Page1(name ,imageid_t1,segid_t1,clsid_t1,
        imageid_t2,segid_t2,clsid_t2,shapefile_t1,shapefile_t2,
        isUseLayer_t1,isUseLayer_t2);
}

void WizardVectorChangeDetection::showErrorText_Page1(QString msg)
{
    ui->textEditDataSource->clear();
    ui->textEditDataSource->setHtml(QString(
        "<div align=\"center\"><font color=red size=10 face=verdana>%1</font></div>"
        "<p>%2</p>").arg(tr("Params invalid")).arg(msg));
    ui->textEditDataSource->setStyleSheet("border: 3px solid red");
}

void WizardVectorChangeDetection::showCorrectText_Page1(
        QString name ,
        QString imageid_t1,QString segid_t1,QString clsid_t1,
        QString imageid_t2,QString segid_t2,QString clsid_t2,
        QString shapefile_t1,QString shapefile_t2,
        bool isUseLayer_t1,bool isUseLayer_t2)
{
    ui->textEditDataSource->clear();
    QString t1,t2;

    auto layerHtml = [=](QString imgID,QString segID,QString clsID)->QString
    {
        QString imgName = CDTImageLayer::getLayer(imgID)->name();
        QString segName = CDTSegmentationLayer::getLayer(segID)->name();
        QString clsName = CDTClassificationLayer::getLayer(clsID)->name();
        return QString(
                    "<tr><td>%1<br /></td><td>Layer<br /></td></tr>"
                    "<tr><td>%2<br /></td><td>%3<br /></td></tr>"
                    "<tr><td>%4<br /></td><td>%5<br /></td></tr>"
                    "<tr><td>%6<br /></td><td>%7<br /></td></tr>")
                .arg(tr("Source type"))
                .arg(tr("Image layer")).arg(imgName)
                .arg(tr("Segmentation layer")).arg(segName)
                .arg(tr("Classification layer")).arg(clsName);
    };
    auto shapefileHtml = [=](QString path)->QString
    {
        return QString(
                    "<tr><td>%1<br /></td><td>Shapefile<br /></td></tr>"
                    "<tr><td>%2<br /></td><td>%3<br /></td></tr>")
                .arg(tr("Source type")).arg(tr("Path")).arg(path);
    };

    const QString htmlTable = "<p><table bordercolor=\"#000000\" width=\"95%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\"><tbody>%1</tbody></table></p>";
    if (isUseLayer_t1)
        t1=htmlTable.arg(layerHtml(imageid_t1,segid_t1,clsid_t1));
    else
        t1=htmlTable.arg(shapefileHtml(shapefile_t1));

    if (isUseLayer_t2)
        t2=htmlTable.arg(layerHtml(imageid_t2,segid_t2,clsid_t2));
    else
        t2=htmlTable.arg(shapefileHtml(shapefile_t2));

    ui->textEditDataSource->setHtml(QString(
        "<h3><font face=verdana>%4</font></h3>"
        "<p align=\"center\">%1</p>"
        "<h3><font face=verdana>%5</font></h3>%2"
        "<h3><font face=verdana>%6</font></h3>%3")
            .arg(name).arg(t1).arg(t2)
            .arg(tr("Change Detection's name:"))
            .arg(tr("Source of epoch 1 :"))
            .arg(tr("Source of epoch 2 :")));
    ui->textEditDataSource->setStyleSheet("border: 3px solid green;");
}


