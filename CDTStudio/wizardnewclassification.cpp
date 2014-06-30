#include "wizardnewclassification.h"
#include "ui_wizardnewclassification.h"
#include "stable.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtattributesinterface.h"
#include "cdtclassifierinterface.h"
#include "dialogdbconnection.h"

extern QList<CDTAttributesInterface *>     attributesPlugins;
extern QList<CDTClassifierInterface *>     classifierPlugins;

//Data transform function
void dataNormalize(cv::Mat& data,QString method)
{
    int nRows = data.rows;
    int nCols = data.cols;

    if (method==QString("linear"))
    {
        //linear
        for (int j = 0; j < nCols; ++j)
        {
            float minVal = data.at<float>(0, j);
            float maxVal = data.at<float>(0, j);
            float temp;
            for (int i = 0; i < nRows; ++i)
            {
                temp = data.at<float>(i, j);
                if (temp < minVal)
                    minVal = temp;
                if(temp > maxVal)
                    maxVal = temp;
            }

            temp = maxVal - minVal;
            for (int i = 0; i < nRows; ++i)
            {
                data.at<float>(i, j) = (data.at<float>(i, j) - minVal)/temp;
            }
        }
    }
    else if (method==QString("stddev"))
    {
        //stddev
        for (int j = 0; j < nCols; ++j)
        {
            float meanVal = 0;
            float sigmaVal = 0;
            for (int i =0; i < nRows; ++i)
            {
                meanVal += data.at<float>(i, j);
            }
            meanVal /= nRows;

            for (int i = 0; i < nRows; ++i)
            {
                float temp = data.at<float>(i, j) - meanVal;
                sigmaVal += temp * temp;
            }
            sigmaVal = sqrt(sigmaVal/(nRows - 1));

            //normalize
            for (int i = 0; i < nRows; ++i)
            {
                float temp = ((data.at<float>(i, j) - meanVal)/(3*sigmaVal)+1.)/2.;
                if (temp < 0) temp = 0;
                if (temp > 1) temp = 1;
                data.at<float>(i,j) = temp;
            }
        }
    }
    else if (method==QString("calculas"))
    {
        //calculas
        for (int j = 0; j < nCols; ++j)
        {
            std::vector< std::pair<float, int> > temp_feature;
            for (int i = 0; i < nRows; ++i)
            {
                temp_feature.push_back(std::make_pair(data.at<float>(i, j),i));
            }
            std::sort(temp_feature.begin(), temp_feature.end());


            float sumVal = 0;
            for (int i = 0; i < nRows; ++i)
            {
                sumVal += temp_feature[i].first;
            }

            float sum_temp = 0;
            for (int i = 0; i < nRows; ++i)
            {
                sum_temp += temp_feature[i].first;
                data.at<float>(temp_feature[i].second, j) = sum_temp/sumVal;
            }
        }
    }
    else if (method==QString("rank"))
    {
        //rank
        for (int j = 0; j < nCols; ++j)
        {
            std::vector< std::pair<float, int> > temp_feature;
            for (int i = 0; i < nRows; ++i)
            {
                temp_feature.push_back(std::make_pair(data.at<float>(i, j), i));
            }
            std::sort(temp_feature.begin(), temp_feature.end());

            for (int i = 0; i < nRows; ++i)
            {
                data.at<float>(temp_feature[i].second, j) = (float)i/(nRows - 1);
                //                qDebug()<<mat.at<float>(temp_feature[i].second, j);
            }
        }
    }
}

void pcaTransform(cv::Mat& data,int pcaParam)
{
    if (pcaParam>0 && pcaParam<=data.cols)
    {
        cv::PCA pca(data,cv::Mat(),CV_PCA_DATA_AS_ROW,pcaParam);
        data = pca.project(data);
    }
}

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

QStringList WizardNewClassification::attributeNames()
{
    QStringList list;
    foreach (CDTAttributesInterface *interface, attributesPlugins) {
        list<<interface->tableName();
    }
    return list;
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

    QSqlQueryModel *modelSegmentation = (QSqlQueryModel *)(ui->comboBoxInput->model());
    QString segmentationID = modelSegmentation->data(modelSegmentation->index(ui->comboBoxInput->currentIndex(),1)).toString();
    QString sampleID = modelSample->data(modelSample->index(ui->comboBoxSample->currentIndex(),1)).toString();
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec(QString("select imageID from segmentationlayer where id ='%1'").arg(segmentationID));
    query.next();
    QString imageID = query.value(0).toString();

    //Samples
    categoryID_Index.clear();
    query.exec(QString("select id from category where imageid ='%1'").arg(imageID));
    int index = 0;
    while(query.next())
    {
        categoryID_Index.insert(query.value(0).toString(),index);
        ++index;
    }

    query.exec(QString("select count(*) from samples where sampleid ='%1'").arg(sampleID));
    query.next();
    int samplesCount = query.value(0).toInt();
    if (samplesCount<=0)
    {
        QMessageBox::warning(this,tr("Error"),tr("No selected sample, classification failed!"));
        finished = false;
        return ;
    }

    QMap<int,QString> samples;//objID_catID
    query.exec(QString("select objectid,categoryid from samples where sampleid ='%1'").arg(sampleID));
    while(query.next())
    {
        samples.insert(query.value(0).toInt(),query.value(1).toString());
    }

    //Data
    QSqlDatabase db = QSqlDatabase::database("cls");
    query = QSqlQuery(db);

    QStringList featuresList = modelSelectedFeature->stringList();
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
        dataNormalize(data,transformMethod);
        normalizeMethod = transformMethod;
    }
    else
        normalizeMethod = QString::null;

    //PCA
    if (isPCA)
    {
        pcaTransform(data,pca);
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

    bool isOrigin = ui->radioButtonOriginal->isChecked();

    ui->qwtPlot->setDatabase(QSqlDatabase::database("cls"));
    ui->qwtPlot->setTableName(temp[0]);
    ui->qwtPlot->setFieldName(temp[1]);


    ui->qwtPlot->replot();
}
