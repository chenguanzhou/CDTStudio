#include "wizardnewclassification.h"
#include "ui_wizardnewclassification.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"
#include "cdtattributesinterface.h"
#include "cdtclassifierinterface.h"
#include "dialogdbconnection.h"
#include "cdtclassifierassessmentform.h"
#include "cdtlayernamevalidator.h"

extern QList<CDTAttributesInterface *>     attributesPlugins;
extern QList<CDTClassifierInterface *>     classifierPlugins;

WizardNewClassification::WizardNewClassification(QUuid segmentationID, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::WizardNewClassification),
    modelSample(new QSqlQueryModel(this)),
    modelSelectedFeature(new QStringListModel(this)),
    finished(false)
{
    ui->setupUi(this);

    connect(ui->comboBoxInput,SIGNAL(currentIndexChanged(int)),this,SLOT(onSegmentationChanged(int)));
    connect(ui->checkBoxAssessment,SIGNAL(clicked(bool)),ui->comboBoxAssessment,SLOT(setEnabled(bool)));
    connect(ui->treeWidgetAllFeatures,SIGNAL(itemChanged(QTreeWidgetItem*,int)),SLOT(updateSelectedFeature()));
    connect(ui->comboBoxClassifier,SIGNAL(currentIndexChanged(int)),SLOT(onClassifierChanged(int)));
    connect(ui->checkBoxNormalize,SIGNAL(toggled(bool)),ui->comboBoxNormalize,SLOT(setEnabled(bool)));
    connect(ui->checkBoxNormalize,SIGNAL(toggled(bool)),ui->radioButtonTransformed,SLOT(setEnabled(bool)));
    connect(ui->checkBoxPCA,SIGNAL(toggled(bool)),ui->spinBoxPCA,SLOT(setEnabled(bool)));
    connect(ui->listViewSelectedFeatures,SIGNAL(clicked(QModelIndex)),SLOT(updateHistogram()));
    connect(this,SIGNAL(currentIdChanged(int)),SLOT(onCurrentPageChanged(int)));
    connect(this,SIGNAL(customButtonClicked(int)),SLOT(onButtonClicked(int)));

    QSqlQueryModel *modelSegmentation = new QSqlQueryModel(this);
    modelSegmentation->setQuery("select t1.name||' -> '||t2.name,t2.id,t2.name from imagelayer t1,segmentationlayer t2 where t1.id=t2.imageID",QSqlDatabase::database("category"));
    ui->comboBoxInput->setModel(modelSegmentation);
    CDTSegmentationLayer *segLayer = CDTSegmentationLayer::getLayer(segmentationID);
    ui->comboBoxInput->setCurrentIndex(ui->comboBoxInput->findText(((CDTImageLayer*)(segLayer->parent()))->name()+" -> "+segLayer->name()));

    ui->listViewSelectedFeatures->setModel(modelSelectedFeature);

    this->setButtonText(CustomButton1,tr("Start Classification"));

    initClassifiers();

    int index = CDTClassificationLayer::staticMetaObject.indexOfClassInfo("tableName");
    if (index != -1)
    {
        CDTLayerNameValidator *validator = new CDTLayerNameValidator
                (QSqlDatabase::database("category"),"name",CDTClassificationLayer::staticMetaObject.classInfo(index).value(),QString("segmentationid='%1'").arg(segmentationID));
        ui->lineEditOutputName->setValidator(validator);
    }
    ui->lineEditOutputName->setText(tr("Untitled"));

    logger()->debug("WizardNewClassification constructed");
}

WizardNewClassification::~WizardNewClassification()
{
    delete ui;
    if (QSqlDatabase::connectionNames().contains("cls"))
        QSqlDatabase::removeDatabase("cls");
}

bool WizardNewClassification::isValid() const
{
    return finished;
}

void WizardNewClassification::initClassifiers()
{
    ui->comboBoxClassifier->clear();
    foreach (CDTClassifierInterface *interface, classifierPlugins) {
        ui->comboBoxClassifier->addItem(interface->classifierName());
    }
}

void WizardNewClassification::updateFeatures(QString segID)
{
    CDTDatabaseConnInfo connInfo = CDTSegmentationLayer::getLayer(QUuid(segID))->databaseURL();
    QSqlDatabase::removeDatabase("cls");
    ui->treeWidgetAllFeatures->clear();
    QSqlDatabase db = QSqlDatabase::addDatabase(connInfo.dbType,"cls");
    db.setDatabaseName(connInfo.dbName);
    db.setHostName(connInfo.hostName);
    db.setPort(connInfo.port);

    if (!db.open(connInfo.username, connInfo.password)) {
        QSqlError err = db.lastError();
        db = QSqlDatabase();
        QSqlDatabase::removeDatabase("cls");
        QMessageBox::critical(this,tr("Error"),tr("Open database failed!\n information:")+err.text());
    }
    else
    {
        qDebug()<<"Open database "<<connInfo.dbType<<connInfo.dbName<<"suceed";
    }

    QStringList attributes = attributeNames();
    QStringList originalTables = db.tables();
    QStringList tables;
    foreach (QString name, originalTables) {
        if (attributes.contains(name))
            tables<<name;
    }
    foreach (QString name, tables) {
        QTreeWidgetItem *root = new QTreeWidgetItem(QStringList()<<name);
        QSqlRecord record = db.record(name);
        for (int i=0;i<record.count();++i)
        {
            QString fieldName = record.fieldName(i);
            if (fieldName.toLower() != "objectid")
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(QStringList()<<fieldName);
                item->setCheckState(0,Qt::Checked);
                root->addChild(item);
            }
        }
        ui->treeWidgetAllFeatures->addTopLevelItem(root);
    }
    ui->treeWidgetAllFeatures->expandAll();
    updateSelectedFeature();
}

bool WizardNewClassification::validateCurrentPage()
{
    if (currentId()==1)
    {
        if (finished == false)
        {
            QToolTip::showText(this->mapToGlobal(this->button(FinishButton)->pos()),
                               tr("Please start classification first!"),this);
            return false;
        }
        else
            return true;
    }
    else
        return QWizard::validateCurrentPage();
}

void WizardNewClassification::startClassification()
{
    //Init
    finished = false;
    bool isTransform = ui->checkBoxNormalize->isChecked();
    QString transformMethod = ui->comboBoxNormalize->currentText();
    bool isPCA = ui->checkBoxPCA->isChecked();
    int pca = ui->spinBoxPCA->value();
    QSqlQuery query(QSqlDatabase::database("category"));

    QString sampleID = modelSample->data(modelSample->index(ui->comboBoxSample->currentIndex(),1)).toString();
    QString trainingSampleID = modelSample->data(modelSample->index(ui->comboBoxAssessment->currentIndex(),1)).toString();

    //Samples
    categoryID_Index.clear();
    query.exec(QString("select id from category where imageid ='%1'").arg(imageID()));
    int index = 0;
    while(query.next())
    {
        categoryID_Index.insert(query.value(0).toString(),index);
        ++index;
    }

    query.exec(QString("select count(*) from object_samples where sampleid ='%1'").arg(sampleID));
    query.next();
    int samplesCount = query.value(0).toInt();
    if (samplesCount<=0)
    {
        QMessageBox::warning(this,tr("Error"),tr("No selected sample, classification failed!"));
        finished = false;
        return ;
    }

    samples.clear();
    query.exec(QString("select objectid,categoryid from object_samples where sampleid ='%1'").arg(sampleID));
    while(query.next())
    {
        samples.insert(query.value(0).toInt(),query.value(1).toString());
    }

    testSamples.clear();
    query.exec(QString("select objectid,categoryid from object_samples where sampleid ='%1'").arg(trainingSampleID));
    while(query.next())
    {
        testSamples.insert(query.value(0).toInt(),query.value(1).toString());
    }

    //Data
    QSqlDatabase db = QSqlDatabase::database("cls");
    query = QSqlQuery(db);

    featuresList = modelSelectedFeature->stringList();
    QStringList selectString;
    QSet<QString> tables;
    foreach (QString featureName, featuresList) {
        QStringList temp = featureName.split("->");
        selectString << QString("%1.%2").arg(temp[0]).arg(temp[1]);
        tables.insert(temp[0]);
    }
    QStringList tablesName = QStringList::fromSet(tables);

    QString sql;
    sql = "select count(*) from "+ tablesName.join(" natural join ");
    query.exec(sql);
    query.next();
    int dataCount = query.value(0).toInt();
    int featureCount = featuresList.size();

    cv::Mat data(dataCount,featureCount,CV_32FC1);
    sql = "select " + selectString.join(",") + " from " + tablesName.join(" natural join ");
    query.exec(sql);
    index = 0;
    while(query.next())
    {
        float* pRow = data.ptr<float>(index);
        for (int i=0;i<featureCount;++i,++pRow)
            *pRow = query.value(i).toDouble();
        ++index;
    }

    //Transform
    if (isTransform)
    {
        CDTClassificationHelper::dataNormalize(data,transformMethod);
        normalizeMethod = transformMethod;
    }
    else
        normalizeMethod = QString::null;

    //PCA
    if (isPCA)
    {
        CDTClassificationHelper::pcaTransform(data,pca);
        pcaParams = QString::number(pca);
    }
    else
        pcaParams = QString::null;

    //Start
    cv::Mat train_data(samples.size(),featureCount,CV_32FC1);
    cv::Mat responses(samples.size(),1,CV_32SC1);

    for (int i=0;i<samples.keys().size();++i)
    {
        int objID = samples.keys().at(i);
        int catID = categoryID_Index.value(samples.value(objID)).toInt();
        float* pData = data.ptr<float>(objID);
        float* pTrain = train_data.ptr<float>(i);

        std::copy(pData,pData+featureCount,pTrain);
        responses.at<int>(i,0) = catID;
    }

    CDTClassifierInterface *interface = classifierPlugins[ui->comboBoxClassifier->currentIndex()];
    cv::Mat result = interface->startClassification(data,train_data,responses);

    //Export
    name = ui->lineEditOutputName->text();
    method = ui->comboBoxClassifier->currentText();
    label.clear();

    for (int i=0;i<result.rows;++i)
    {
        label<<static_cast<int>(result.at<float>(i,0)+0.5);
    }

    params = interface->params();
    finished = true;
}

//void WizardNewClassification::generateAssessmentResult()
//{
//    Q_ASSERT(finished);
//    CDTClassificationInformation info;
//    QSqlQuery query(QSqlDatabase::database("category"));

//    //general info
////    info.clsName = ui->lineEditOutputName->text();
////    info.classifierName = ui->comboBoxClassifier->currentText();
////    info.classifierParams = params;
////    info.categoryCount = categoryID_Index.size();
////    info.isNormalized = ui->checkBoxNormalize->isChecked();
////    info.pcaParams = ui->checkBoxPCA->isChecked()?ui->spinBoxPCA->value():0;
////    info.selectedFeatures = modelSelectedFeature->stringList();

//    info.clsName = name;
//    info.classifierName = method;
//    info.classifierParams = params;
//    info.categoryCount = categoryID_Index.size();
//    info.isNormalized = !normalizeMethod.isEmpty();
//    info.pcaParams = pcaParams;
//    info.selectedFeatures = featuresList;

//    //for confusion matrix
//    QMap<QString,QString> categoryID_Name;
//    query.exec(QString("select id,name from category where imageid ='%1'").arg(this->imageID()));
//    while(query.next())
//    {
//        categoryID_Name.insert(query.value(0).toString(),query.value(1).toString());
//        info.categories.push_back(query.value(1).toString());
//    }

//    QMap<int,QString> index_CategoryName;
//    QMapIterator<QString, QVariant> i(categoryID_Index);
//    while (i.hasNext()) {
//        i.next();
//        index_CategoryName.insert(i.value().toInt(),categoryID_Name[i.key()]);
//    }

//    foreach (int objID, testSamples.keys()) {
//        QString clsIndex = testSamples.value(objID);
//        info.confusionParams.push_back(QPair<QString,QString>(index_CategoryName[label[objID].toInt()],categoryID_Name[clsIndex]));
//    }

//    ui->assessmentForm->setInfo(info);
//}

//int WizardNewClassification::nextId() const
//{
//    switch (currentId()) {
//    case 0:
//        return 1;
//    case 1:
//        if (ui->checkBoxAssessment->isChecked())
//            return 2;
//        else
//            return -1;
//    case 2:
//        return -1;
//    default://Never used!!
//        return -1;
//    }
//}

//void WizardNewClassification::initializePage(int id )
//{
//    QWizard::initializePage(id);
//    if (id == 2)    //Assessment
//    {
//        generateAssessmentResult();
//    }
//}

QString WizardNewClassification::segmentationID() const
{
    QSqlQueryModel *modelSegmentation = (QSqlQueryModel *)(ui->comboBoxInput->model());
    return modelSegmentation->data(modelSegmentation->index(ui->comboBoxInput->currentIndex(),1)).toString();
}

QString WizardNewClassification::imageID() const
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec(QString("select imageID from segmentationlayer where id ='%1'").arg(segmentationID()));
    query.next();
    return query.value(0).toString();
}

QStringList WizardNewClassification::attributeNames()
{
    QStringList list;
    foreach (CDTAttributesInterface *interface, attributesPlugins) {
        list<<interface->tableName();
    }
    return list;
}

void WizardNewClassification::onSegmentationChanged(int index)
{
    QSqlQueryModel *modelSegmentation = (QSqlQueryModel *)(ui->comboBoxInput->model());
    QString segmentationID = modelSegmentation->data(modelSegmentation->index(index,1)).toString();

    modelSample->setQuery(QString("select name,id from sample_segmentation where segmentationid ='%1'").arg(segmentationID),
                          QSqlDatabase::database("category"));
    ui->comboBoxSample->setModel(modelSample);
    ui->comboBoxAssessment->setModel(modelSample);

    updateFeatures(segmentationID);
}

void WizardNewClassification::onClassifierChanged(int index)
{    
    if (index<0) return;
    CDTClassifierInterface *interface = classifierPlugins[index];
    ui->propertyEditWidget->setObject(interface);
}

void WizardNewClassification::onCurrentPageChanged(int pageID)
{
    this->setOption(HaveCustomButton1,pageID == 1);
}

void WizardNewClassification::onButtonClicked(int buttonID)
{
    if (buttonID == CustomButton1)
    {
        startClassification();
    }
}

void WizardNewClassification::updateSelectedFeature()
{
    QStringList list;
    for (int i=0;i<ui->treeWidgetAllFeatures->topLevelItemCount();++i)
    {
        QTreeWidgetItem *root = ui->treeWidgetAllFeatures->topLevelItem(i);
        for (int k=0;k<root->childCount();++k)
        {
            QTreeWidgetItem *item = root->child(k);
            if (item->checkState(0)==Qt::Checked)
            {
                QString itemName = root->text(0) + "->" + item->text(0);
                list.push_back(itemName);
            }
        }
    }
    modelSelectedFeature->setStringList(list);
    if (list.isEmpty())
        ui->checkBoxPCA->setChecked(false);
    else
        ui->spinBoxPCA->setMaximum(list.size());
}

void WizardNewClassification::updateHistogram()
{
    int index = ui->listViewSelectedFeatures->currentIndex().row();
    if (index<0)
        return;

    QString plotName = modelSelectedFeature->stringList().at(index);
    QStringList temp = plotName.split("->");

//    bool isOrigin = ui->radioButtonOriginal->isChecked();

    ui->qwtPlot->setDatabase(QSqlDatabase::database("cls"));
    ui->qwtPlot->setTableName(temp[0]);
    ui->qwtPlot->setFieldName(temp[1]);


    ui->qwtPlot->replot();
}
