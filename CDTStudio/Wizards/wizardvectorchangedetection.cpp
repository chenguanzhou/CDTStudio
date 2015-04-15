#include "wizardvectorchangedetection.h"
#include "ui_wizardvectorchangedetection.h"

#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"
#include "cdtfilesystem.h"
#include "cdtvectorchangelayer.h"
#include "cdtvectorchangedetectioninterface.h"
#include "cdtvectorchangedetectionhelper.h"
#include "cdtlayernamevalidator.h"
#ifdef Q_OS_WIN
#include "Windows.h"
#endif

extern QList<CDTVectorChangeDetectionInterface *> vectorDetectionPlugins;


WizardVectorChangeDetection::WizardVectorChangeDetection(QUuid projectID, QWidget *parent) :
    QWizard(parent),
    prjID(projectID),
    isValid_Page1(false),
    isValid_Page2(false),
    isValid_Page3(false),
    ui(new Ui::WizardVectorChangeDetection)
{
    ui->setupUi(this);
    initPage1();
    initPage2();
    initPage3();

    connect(this,SIGNAL(currentIdChanged(int)),SLOT(onPageChanged(int)));

#ifdef Q_OS_WIN
    int dpiX = GetDeviceCaps(this->getDC(),LOGPIXELSX);
    if(dpiX == 96)
        this->adjustSize();
#endif

    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("WizardVectorChangeDetection");
    this->restoreGeometry(setting.value("geometry").toByteArray());
    setting.endGroup();

    int index = CDTVectorChangeLayer::staticMetaObject.indexOfClassInfo("tableName");
    if (index != -1)
    {
        CDTLayerNameValidator *validator = new CDTLayerNameValidator
                (QSqlDatabase::database("category"),"name",CDTVectorChangeLayer::staticMetaObject.classInfo(index).value(),QString("project='%1'").arg(projectID));
        ui->lineEditName->setValidator(validator);
    }
    ui->lineEditName->setText(tr("Untitled"));
}

WizardVectorChangeDetection::~WizardVectorChangeDetection()
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("WizardVectorChangeDetection");
    setting.setValue("geometry",this->saveGeometry());
    setting.endGroup();
    delete ui;
}

void WizardVectorChangeDetection::closeEvent(QCloseEvent *)
{

}

bool WizardVectorChangeDetection::validateCurrentPage()
{
    if (currentId()==0)
        //Page1
        return isValid_Page1;
    if (currentId()==1)
        //Page2
        return isValid_Page2;
    if (currentId()==2)
        //Page3
        return isValid_Page3;
    return true;
}

void WizardVectorChangeDetection::onPageChanged(int pageID)
{
    if (pageID==2 && ui->comboBoxAlgo->count()>0)
        this->setOption(HaveCustomButton1,true);
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
    connect(ui->lineEditT1_ShpPath,SIGNAL(textChanged(QString)),SLOT(updateShpFieldsT1()));
    connect(ui->lineEditT2_ShpPath,SIGNAL(textChanged(QString)),SLOT(updateShpFieldsT2()));
    connect(ui->pushButtonT1_ShpPath,SIGNAL(clicked()),SLOT(onButtonShpT1Clicked()));
    connect(ui->pushButtonT2_ShpPath,SIGNAL(clicked()),SLOT(onButtonShpT2Clicked()));
    connect(ui->comboBoxT1_ShpField,SIGNAL(currentIndexChanged(int)),SLOT(updateReport_Page1()));
    connect(ui->comboBoxT2_ShpField,SIGNAL(currentIndexChanged(int)),SLOT(updateReport_Page1()));

    updateReport_Page1();
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

void WizardVectorChangeDetection::updateShpFieldsT1()
{
    ui->comboBoxT1_ShpField->clear();
    QString path = ui->lineEditT1_ShpPath->text();
    if (path.isEmpty()) return;
    QgsVectorLayer *layer = new QgsVectorLayer(path,QFileInfo(path).completeBaseName(),"ogr");
    if (!layer->isValid()) return;
    QgsFields fields = layer->dataProvider()->fields();
    for (int i=0;i<fields.count();++i) ui->comboBoxT1_ShpField->addItem(fields.at(i).name());
}

void WizardVectorChangeDetection::updateShpFieldsT2()
{
    ui->comboBoxT2_ShpField->clear();
    QString path = ui->lineEditT2_ShpPath->text();
    if (path.isEmpty()) return;
    QgsVectorLayer *layer = new QgsVectorLayer(path,QFileInfo(path).completeBaseName(),"ogr");
    if (!layer->isValid()) return;
    QgsFields fields = layer->dataProvider()->fields();
    for (int i=0;i<fields.count();++i) ui->comboBoxT2_ShpField->addItem(fields.at(i).name());

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
    QString shapefileFieldName_t1 = ui->comboBoxT1_ShpField->currentText();
    QString shapefileFieldName_t2 = ui->comboBoxT2_ShpField->currentText();

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
            isText_Empty(clsid_t1,tr("Classification layer of epoch 1"));
        }
        else
        {
            //shapefile
            isText_Empty(shapefile_t1,tr("Shapefile of epoch 1"));
            isText_Empty(shapefileFieldName_t1,tr("Field name for shapefile of epoch 1"));
        }

        if (isUseLayer_t2)
        {
            //layers
            isText_Empty(imageid_t2,tr("Image layer of epoch 2"));
            isText_Empty(segid_t2,tr("Segmentation layer of epoch 2"));
            isText_Empty(clsid_t2,tr("Classification layer of epoch 2"));
        }
        else
        {
            //shapefile
            isText_Empty(shapefile_t2,tr("Shapefile of epoch 2"));
            isText_Empty(shapefileFieldName_t2,tr("Field name for shapefile of epoch 2"));
        }

        //Same image when both use layers
        if (isUseLayer_t1 == true && isUseLayer_t2==true && imageid_t1 == imageid_t2)
            throw tr("The images of 2 epochs looks the same!");

        if (isUseLayer_t1 == false && isUseLayer_t2==false && shapefile_t1 == shapefile_t2)
            throw tr("The shapefiles of 2 epochs looks the same!");
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
        imageid_t2,segid_t2,clsid_t2,
        shapefile_t1,shapefile_t2,
        shapefileFieldName_t1,shapefileFieldName_t2,
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

void WizardVectorChangeDetection::showCorrectText_Page1(QString name ,
        QString imageid_t1, QString segid_t1, QString clsid_t1,
        QString imageid_t2, QString segid_t2, QString clsid_t2,
        QString shapefile_t1, QString shapefile_t2,
        QString shapefileFieldName_t1, QString shapefileFieldName_t2,
        bool isUseLayer_t1, bool isUseLayer_t2)
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
    auto shapefileHtml = [=](QString path,QString fieldName)->QString
    {
        return QString(
                    "<tr><td>%1<br /></td><td>Shapefile<br /></td></tr>"
                    "<tr><td>%2<br /></td><td>%3<br /></td></tr>"
                    "<tr><td>%4<br /></td><td>%5<br /></td></tr>")
                .arg(tr("Source type"))
                .arg(tr("Path")).arg(path)
                .arg(tr("Field name")).arg(fieldName);
    };

    const QString htmlTable = "<p><table bordercolor=\"#000000\" width=\"95%\" border=\"1\" cellspacing=\"0\" cellpadding=\"2\"><tbody>%1</tbody></table></p>";
    if (isUseLayer_t1)
        t1=htmlTable.arg(layerHtml(imageid_t1,segid_t1,clsid_t1));
    else
        t1=htmlTable.arg(shapefileHtml(shapefile_t1,shapefileFieldName_t1));

    if (isUseLayer_t2)
        t2=htmlTable.arg(layerHtml(imageid_t2,segid_t2,clsid_t2));
    else
        t2=htmlTable.arg(shapefileHtml(shapefile_t2,shapefileFieldName_t2));

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

    //Update Page 2 & Get categories
    auto getCategoriesFromLayer = [](QString imageID)->QStringList
    {
        QSqlQuery query(QSqlDatabase::database("category"));
        query.exec(QString("select name from category where imageID = '%1'").arg(imageID));
        QStringList list;
        while (query.next()) {
            list<<query.value(0).toString();
        }
        return list;
    };

    auto getCategoriesFromShapefile = [](QString shpPath,QString fieldName)->QStringList
    {
        QSet<QString> categories;
        QgsVectorLayer *layer = new QgsVectorLayer(shpPath,QFileInfo(shpPath).completeBaseName(),"ogr");
        QgsFeatureIterator iter = layer->getFeatures();
        QgsFeature f;
        while (iter.nextFeature(f)) {
            categories.insert( f.attribute(fieldName).toString() );
        }
        QStringList list = QStringList::fromSet(categories);
        return list;
    };

    QStringList categories_t1,categories_t2;
    if (isUseLayer_t1)
        categories_t1 = getCategoriesFromLayer(imageid_t1);
    else
        categories_t1 = getCategoriesFromShapefile(shapefile_t1,shapefileFieldName_t1);
    if (isUseLayer_t2)
        categories_t2 = getCategoriesFromLayer(imageid_t2);
    else
        categories_t2 = getCategoriesFromShapefile(shapefile_t2,shapefileFieldName_t2);

    updateMatchingPairs(categories_t1,categories_t2);
}

/********************************************/
/*                  Page 2                  */
/********************************************/

void WizardVectorChangeDetection::initPage2()
{
    connect(ui->pushButtonMatchingDefault,SIGNAL(clicked()),SLOT(defaultMatching()));
    connect(ui->pushButtonMatching,SIGNAL(clicked()),SLOT(onButtonMatching()));
    connect(ui->pushButtonUnmatching,SIGNAL(clicked()),SLOT(onButtonUnmatching()));
    connect(ui->pushButtonUnmatchingAll,SIGNAL(clicked()),SLOT(unmatchingAll()));

    //radioButtonChecked()
    connect(ui->buttonGroupDetectionType,SIGNAL(buttonClicked(int)),SLOT(onDetectionTypeChanged()));
}

void WizardVectorChangeDetection::updateMatchingPairs(QStringList t1, QStringList t2)
{
    ui->listWidgetCategoryT1->clear();
    ui->listWidgetCategoryT2->clear();
    ui->listWidgetCategoryPairs->clear();

    ui->listWidgetCategoryT1->addItems(t1);
    ui->listWidgetCategoryT2->addItems(t2);
    ui->listWidgetCategoryT1->sortItems();
    ui->listWidgetCategoryT2->sortItems();

    defaultMatching();
}

void WizardVectorChangeDetection::makeCategoryPair(int t1_index, int t2_index)
{
    if (t1_index>=ui->listWidgetCategoryT1->count() || t1_index<0) return;
    if (t2_index>=ui->listWidgetCategoryT2->count() || t2_index<0) return;

    auto t1_item = ui->listWidgetCategoryT1->takeItem(t1_index);
    auto t2_item = ui->listWidgetCategoryT2->takeItem(t2_index);

    QString t1 = t1_item->text();
    QString t2 = t2_item->text();
    QString newPair = t1 + "<->" + t2;

    delete t1_item;
    delete t2_item;


    ui->listWidgetCategoryPairs->addItem(newPair);
    ui->listWidgetCategoryPairs->sortItems();
}

void WizardVectorChangeDetection::removeCategoryPair(int index)
{
    if (index>=ui->listWidgetCategoryPairs->count() || index<0) return;

    auto item = ui->listWidgetCategoryPairs->takeItem(index);
    QString pair = item->text();
    delete item;

    QStringList pairItems = pair.split("<->");
    Q_ASSERT(pairItems.count()==2);
    ui->listWidgetCategoryT1->addItem(pairItems[0]);
    ui->listWidgetCategoryT2->addItem(pairItems[1]);

    ui->listWidgetCategoryT1->sortItems();
    ui->listWidgetCategoryT2->sortItems();
}

void WizardVectorChangeDetection::updatePage2State()
{
    bool isPairEmpty = ui->listWidgetCategoryPairs->count()==0;
    ui->pushButtonUnmatchingAll->setEnabled(!isPairEmpty);
    ui->pushButtonUnmatching->setEnabled(!isPairEmpty);

    if (ui->radioButtonMultipleCategories->isChecked())
    {
        ui->pushButtonMatching->setEnabled(
                    ui->listWidgetCategoryT1->count()!=0 &&
                    ui->listWidgetCategoryT2->count()!=0);
    }
    else
        ui->pushButtonMatching->setEnabled(
                    ui->listWidgetCategoryT1->count()!=0 &&
                    ui->listWidgetCategoryT2->count()!=0 &&
                    isPairEmpty);

    if (isPairEmpty)
        ui->listWidgetCategoryPairs->setStyleSheet("border: 3px solid red");
    else
        ui->listWidgetCategoryPairs->setStyleSheet("border: 3px solid green");
    isValid_Page2 = !isPairEmpty;
}

void WizardVectorChangeDetection::defaultMatching()
{
    unmatchingAll();

    for (int i=0;i<ui->listWidgetCategoryT1->count();++i)
    {
        QString name = ui->listWidgetCategoryT1->item(i)->text();
        QList<QListWidgetItem*> list = ui->listWidgetCategoryT2->findItems(name,Qt::MatchExactly);
        if (list.size()>0)
        {
            makeCategoryPair(i,ui->listWidgetCategoryT2->row(list[0]));
            i--;
            if (!ui->radioButtonMultipleCategories->isChecked())
                break;
        }
    }

    updatePage2State();
}

void WizardVectorChangeDetection::onButtonMatching()
{
    if (ui->listWidgetCategoryT1->count() == 0 || ui->listWidgetCategoryT2->count() == 0 )
        return;

    makeCategoryPair(ui->listWidgetCategoryT1->currentIndex().row(),ui->listWidgetCategoryT2->currentIndex().row());
    updatePage2State();
}

void WizardVectorChangeDetection::onButtonUnmatching()
{
    if (ui->listWidgetCategoryPairs->count() == 0)
        return;

    removeCategoryPair(ui->listWidgetCategoryPairs->currentIndex().row());
    updatePage2State();
}

void WizardVectorChangeDetection::unmatchingAll()
{
    while (ui->listWidgetCategoryPairs->count()>0) {
        removeCategoryPair(0);
    }
    updatePage2State();
}

void WizardVectorChangeDetection::onDetectionTypeChanged()
{
    if (!ui->radioButtonMultipleCategories->isChecked())
    {
        while (ui->listWidgetCategoryPairs->count()>1)
        {
            removeCategoryPair(1);
        }
    }
    updatePage2State();
}

/********************************************/
/*                  Page 3                  */
/********************************************/

void WizardVectorChangeDetection::initPage3()
{
    this->setButtonText(CustomButton1,tr("Start Detection"));
    foreach (CDTVectorChangeDetectionInterface* plugin, vectorDetectionPlugins) {
        ui->comboBoxAlgo->addItem(plugin->methodName());
    }

    connect(this,SIGNAL(customButtonClicked(int)),SLOT(onCustomButtonClicked(int)));
    updatePage3State();
}

void WizardVectorChangeDetection::startDetect()
{
    auto getCurrentBoxID = [](QComboBox* box)->QString
    {
        return box->model()->data(box->model()->index(box->currentIndex(),1))
                .toString();
    };

    QString imageid_t1 = getCurrentBoxID(ui->comboBoxT1_ImageLayer);
    QString segid_t1 = getCurrentBoxID(ui->comboBoxT1_SegLayer);
    QString clsid_t1 = getCurrentBoxID(ui->comboBoxT1_ClsLayer);
    QString imageid_t2 = getCurrentBoxID(ui->comboBoxT2_ImageLayer);
    QString segid_t2 = getCurrentBoxID(ui->comboBoxT2_SegLayer);
    QString clsid_t2 = getCurrentBoxID(ui->comboBoxT2_ClsLayer);

    QString shapefile_t1 = ui->lineEditT1_ShpPath->text();
    QString shapefile_t2 = ui->lineEditT2_ShpPath->text();
    QString shapefileFieldName_t1 = ui->comboBoxT1_ShpField->currentText();
    QString shapefileFieldName_t2 = ui->comboBoxT2_ShpField->currentText();

    bool isUseLayer_t1 = ui->tabWidgetT1->currentIndex()==0;
    bool isUseLayer_t2 = ui->tabWidgetT2->currentIndex()==0;

    QMap<QString, QString> categoryPairs;
//    QStringList categoryNamesT1;
//    QStringList categoryNamesT2;
    for (int i=0;i<ui->listWidgetCategoryPairs->count();++i)
    {
        QString pair = ui->listWidgetCategoryPairs->item(i)->text();
        QStringList pairText = pair.split("<->");
//        categoryNamesT1.push_back(pairText[0]);
//        categoryNamesT2.push_back(pairText[1]);
        categoryPairs.insert(pairText[0],pairText[1]);
    }

    CDTVectorChangeDetectionHelper *helper =
            new CDTVectorChangeDetectionHelper(
                imageid_t1,
                imageid_t2,
                segid_t1,
                segid_t2,
                clsid_t1,
                clsid_t2,
                shapefile_t1,
                shapefile_t2,
                shapefileFieldName_t1,
                shapefileFieldName_t2,
                isUseLayer_t1,
                isUseLayer_t2,
//                categoryNamesT1,
//                categoryNamesT2,
                categoryPairs,
                vectorDetectionPlugins[ui->comboBoxAlgo->currentIndex()]);
    connect(helper,SIGNAL(finished()),SLOT(onDetectionFinished()));
    connect(helper,SIGNAL(currentProgressChanged(QString)),this,SLOT(updateProgressLabel(QString)));
    connect(helper,SIGNAL(progressBarValueChanged(int)),ui->progressBar,SLOT(setValue(int)));
    helper->start();
    this->setEnabled(false);
}

void WizardVectorChangeDetection::showErrorText_Page3(QString msg)
{
    ui->textEditResult->clear();
    ui->textEditResult->setHtml(QString(
        "<div align=\"center\"><font color=red size=10 face=verdana>%1</font></div>"
        "<p>%2</p>").arg(tr("Params invalid")).arg(msg));
    ui->textEditResult->setStyleSheet("border: 3px solid red");
}

void WizardVectorChangeDetection::showCorrectText_Page3()
{

}

void WizardVectorChangeDetection::updateProgressLabel(QString text)
{
    ui->groupBoxProgressLabel->setTitle(text);
}

void WizardVectorChangeDetection::onCustomButtonClicked(int buttonID)
{
    if (buttonID == CustomButton1)
    {
        startDetect();
    }
}

void WizardVectorChangeDetection::updatePage3State()
{
    if (ui->comboBoxAlgo->count()==0)
    {
        //No plugin
        showErrorText_Page3("No detection plugin found!");
        return;
    }
    else if (isValid_Page3 == false)
    {
        //Enable to detect change
        ui->textEditResult->clear();
        ui->textEditResult->setHtml(QString(
            "<div align=\"center\"><font size=10 face=verdana>%1</font></div>"
            "<p>%2</p>").arg(tr("Params are valid")).arg(tr("Detection process can be start!")));
        ui->textEditResult->setStyleSheet("border: 3px solid green");
    }
    else
    {
        //det3ection completed
        ui->textEditResult->clear();
        ui->textEditResult->setHtml(QString(
            "<div align=\"center\"><font size=10 face=verdana>%1</font></div>"
            "<p>%2</p><p>%3</p>")
            .arg(tr("Completed!"))
            .arg(tr("Change detection process is completed!\n"))
            .arg(tr("You can press \"%1\" button to close the wizard"
                    ", or press \"%2\" to cancel.")).
                                    arg(tr("Finish"))
                                    .arg(tr("Close"))
        );
        ui->textEditResult->setStyleSheet("border: 3px solid green");
    }
}

void WizardVectorChangeDetection::onDetectionFinished()
{
    this->setEnabled(true);

    CDTVectorChangeDetectionHelper *helper = qobject_cast<CDTVectorChangeDetectionHelper*>(sender());
    if (helper->isValid())
    {
        shpID = QUuid::createUuid().toString();
        shpPath = helper->shapefilePath();
        isValid_Page3 = true;
    }
    else
    {
        QMessageBox::critical(this,tr("Error"),tr("Change det3ection failed!"));
        isValid_Page3 = false;
    }
    updatePage3State();
}

QString WizardVectorChangeDetection::name() const
{
    return ui->lineEditName->text();
}

QString WizardVectorChangeDetection::shapefileID() const
{
    return shpID;
}

QString WizardVectorChangeDetection::shapefilePath() const
{
    return shpPath;
}

QString WizardVectorChangeDetection::clsID1() const
{
    auto getCurrentBoxID = [](QComboBox* box)->QString
    {
        return box->model()->data(box->model()->index(box->currentIndex(),1))
                .toString();
    };

    return getCurrentBoxID(ui->comboBoxT1_ClsLayer);
}


QString WizardVectorChangeDetection::clsID2() const
{
    auto getCurrentBoxID = [](QComboBox* box)->QString
    {
        return box->model()->data(box->model()->index(box->currentIndex(),1))
                .toString();
    };

    return getCurrentBoxID(ui->comboBoxT2_ClsLayer);
}

QVariantMap WizardVectorChangeDetection::params() const
{
    return vectorDetectionPlugins[ui->comboBoxAlgo->currentIndex()]->params();
}


