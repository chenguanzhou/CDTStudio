#include "cdtclassifierassessmentform.h"
#include "ui_cdtclassifierassessmentform.h"
#include "stable.h"
#include <qgsrasteridentifyresult.h>
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"

CDTClassifierAssessmentForm::CDTClassifierAssessmentForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDTClassifierAssessmentForm),
    modelClassification(new QSqlQueryModel(this)),
    modelSample(new QSqlQueryModel(this))
{
    ui->setupUi(this);
    ui->comboBoxClassification->setModel(modelClassification);
    ui->comboBoxSample->setModel(modelSample);

    connect(ui->comboBoxClassification,SIGNAL(currentIndexChanged(int)),SLOT(onComboBoxClassificationChanged(int)));
    connect(ui->comboBoxSample,SIGNAL(currentIndexChanged(int)),SLOT(onComboBoxSampleChanged(int)));

    init();
}

CDTClassifierAssessmentForm::~CDTClassifierAssessmentForm()
{
    delete ui;
}

void CDTClassifierAssessmentForm::setClassification(QString id)
{
//    CDTClassificationLayer *layer = CDTClassificationLayer::getLayer(id);
    for (int i=0;i<modelClassification->rowCount();++i)
    {
        if (modelClassification->data(modelClassification->index(i,1)).toString() == id)
        {
            ui->comboBoxClassification->setCurrentIndex(i);
        }
    }
}

void CDTClassifierAssessmentForm::init()
{
    QSqlDatabase db = QSqlDatabase::database("category");
    modelClassification->setQuery("Select name,id,segmentationID from classificationlayer",db);
}

void CDTClassifierAssessmentForm::onComboBoxClassificationChanged(int index)
{
    if (index<0)
        return;
    QString segmentationID = modelClassification->data(modelClassification->index(index,2)).toString();
    CDTSegmentationLayer *segLayer = CDTSegmentationLayer::getLayer(segmentationID);
    QString segmentationName = segLayer->name();
    QString imageName = static_cast<CDTImageLayer *>(segLayer->parent())->name();
    QString iamgeID = static_cast<CDTImageLayer *>(segLayer->parent())->id();

    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(tr("Image name:    ")+imageName);
    ui->plainTextEdit->appendPlainText(tr("Segmentation name:    ")+segmentationName);


//    modelSample->setQuery(QString("Select name,id from sample_segmentation where segmentationid = '%1'")
//                          .arg(segmentationID),QSqlDatabase::database("category"));

    modelSample->setQuery(QString("Select name,id from image_validation_samples where imageid = '%1'")
                          .arg(iamgeID),QSqlDatabase::database("category"));
}

void CDTClassifierAssessmentForm::onComboBoxSampleChanged(int index)
{
    if (index<0)
        return;

    QString classificationID = modelClassification->data
            (modelClassification->index(ui->comboBoxClassification->currentIndex(),1)).toString();
    QString sampleID = modelSample->data(modelSample->index(index,1)).toString();

    CDTClassificationLayer *layer = CDTClassificationLayer::getLayer(classificationID);
    CDTClassificationInformation info;
    info.clsName = layer->name();
    info.classifierName = layer->method();
    info.classifierParams = layer->params();
    info.categoryCount = layer->clsInfo().size();
    info.isNormalized = !layer->normalizeMethod().isEmpty();
    info.pcaParams = layer->pcaParams();
    info.selectedFeatures = layer->selectedFeatures();

    //for confusion matrix
    QMap<QString,QString> categoryID_Name;
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec(QString("select id,name from category where imageid ='%1'")
               .arg(static_cast<CDTImageLayer *>(layer->parent()->parent())->id()));
    while(query.next())
    {
        categoryID_Name.insert(query.value(0).toString(),query.value(1).toString());
        info.categories.push_back(query.value(1).toString());
    }

    QMap<int,QString> index_CategoryName;
    QMapIterator<QString, QVariant> i(layer->clsInfo());
    while (i.hasNext()) {
        i.next();
        index_CategoryName.insert(i.value().toInt(),categoryID_Name[i.key()]);
    }

    CDTSegmentationLayer *segLayer = qobject_cast<CDTSegmentationLayer*>(layer->parent());
    if (segLayer == NULL)
        return;
    QString markFIlePath = segLayer->markfileTempPath();
    QgsRasterLayer *imgMarkLayer = new QgsRasterLayer(markFIlePath);
    if (!imgMarkLayer->isValid())
        return;

    QMap<int,QString> testSamples;
    QList<QVariant> label = layer->data();
//    query.exec(QString("select objectid,categoryid from object_samples where sampleid ='%1'").arg(sampleID));

//    while(query.next())
//    {
//        testSamples.insert(query.value(0).toInt(),query.value(1).toString());
//    }
//    foreach (int objID, testSamples.keys()) {
//        QString clsIndex = testSamples.value(objID);
//        info.confusionParams.push_back(QPair<QString,QString>(index_CategoryName[label[objID].toInt()],categoryID_Name[clsIndex]));
//    }

    query.exec(QString("select x,y,categoryid from "
                       "(select id,x,y from points where pointset_name= "
                       "(select pointset_name from image_validation_samples where id = '%1' )) "
                       "INNER JOIN point_category "
                       "USING (id)").arg(sampleID));

    while(query.next())
    {
        double x = query.value(0).toDouble();
        double y = query.value(1).toDouble();
        QString categoryID = query.value(2).toString();
        QgsRasterIdentifyResult result = imgMarkLayer->dataProvider()->identify(QgsPoint(x,y),QgsRaster::IdentifyFormatValue);
        int objID = result.results().value(1).toInt();
        testSamples.insert(objID,categoryID);
    }


    foreach (int objID, testSamples.keys()) {
        QString clsIndex = testSamples.value(objID);
        info.confusionParams.push_back(QPair<QString,QString>(index_CategoryName[label[objID].toInt()],categoryID_Name[clsIndex]));
    }


    this->setInfo(info);

}

void CDTClassifierAssessmentForm::setInfo(const CDTClassificationInformation &info)
{
    updateGeneralInfo(info);
    updateConfusionMatrix(info);
}

void CDTClassifierAssessmentForm::updateGeneralInfo(const CDTClassificationInformation& info)
{
    ui->treeWidget->clear();

    QTreeWidgetItem *itemClsName = new QTreeWidgetItem(
                ui->treeWidget,QStringList()<<tr("Name")<<info.clsName);
    QTreeWidgetItem *itemClsfier = new QTreeWidgetItem(
                ui->treeWidget,QStringList()<<tr("Classifier")<<info.classifierName);
    QTreeWidgetItem *itemCateCount = new QTreeWidgetItem(
                ui->treeWidget,QStringList()<<tr("Category Count")<<QString::number(info.categoryCount));
    QTreeWidgetItem *itemNormal = new QTreeWidgetItem(
                ui->treeWidget,QStringList()<<tr("Normalized")<<(info.isNormalized?tr("true"):tr("false")));
    QTreeWidgetItem *itemPCA = new QTreeWidgetItem(
                ui->treeWidget,QStringList()<<tr("PCA Params")<<info.pcaParams);
    QTreeWidgetItem *itemFeatures = new QTreeWidgetItem(
                ui->treeWidget,QStringList()<<tr("Selected Features"));

    foreach (QString key, info.classifierParams.keys()) {
        QString value = info.classifierParams.value(key).toString();
        itemClsfier->addChild(new QTreeWidgetItem(
                itemClsfier,QStringList()<<key<<value));
    }
    foreach (QString featureName, info.selectedFeatures) {
        itemFeatures->addChild(new QTreeWidgetItem(
                itemFeatures,QStringList()<<featureName));
    }

    ui->treeWidget->addTopLevelItems(QList<QTreeWidgetItem*>()<<itemClsName<<itemClsfier
                <<itemCateCount<<itemNormal<<itemPCA<<itemFeatures);


    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->expandAll();
}

void CDTClassifierAssessmentForm::updateConfusionMatrix(const CDTClassificationInformation &info)
{
    //Confusion Matrix
    QStringList categories = info.categories;
    QMap<QString,int> index;
    for (int i=0;i<categories.size();++i)
    {
        index.insert(categories[i],i);
    }

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(categories.size()+1);
    ui->tableWidget->setRowCount(categories.size()+1);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<categories<<tr("Total"));
    ui->tableWidget->setVerticalHeaderLabels(QStringList()<<categories<<tr("Total"));

    cv::Mat matrixData = cv::Mat::zeros(categories.size()+1,categories.size()+1,CV_32S);
    for (int i=0;i<info.confusionParams.size();++i)
    {
        QString result      = info.confusionParams[i].first;
        QString testSample  = info.confusionParams[i].second;
        int resultIndex     = index.value(result);
        int testSampleIndex = index.value(testSample);
        ++ matrixData.at<int>(resultIndex,testSampleIndex);
    }

    for(int i=0;i<categories.size();++i)
    {
        int sumRow=0,sumCol=0;
        for(int j=0;j<categories.size();++j)
        {
            sumRow += matrixData.at<int>(i,j);
            sumCol += matrixData.at<int>(j,i);
        }
        matrixData.at<int>(categories.size(),i) = sumCol;
        matrixData.at<int>(i,categories.size()) = sumRow;
    }

    matrixData.at<int>(categories.size(),categories.size()) = info.confusionParams.size();

    for(int i=0;i<matrixData.rows;++i)
    {
        for (int j=0;j<matrixData.cols;++j)
        {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(matrixData.at<int>(i,j)));
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i,j,item);
        }
    }

    for(int i=0;i<categories.size();++i)
    {
        QTableWidgetItem *item = ui->tableWidget->item(i,i);
        item->setTextColor(QColor(Qt::red));
        QFont fnt = item->font();
        fnt.setBold(true);
        item->setFont(fnt);
    }

    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();

    //Overall accuracy
    int correctCount = 0;
    for (int i=0;i<categories.size();++i)
    {
        correctCount += matrixData.at<int>(i,i);
    }
//    qDebug()<<"info.confusionParams.size():"<<info.confusionParams.size();
    double overall = correctCount*100/info.confusionParams.size();
    ui->overallAcuraccyLineEdit->setText(QString::number(overall)+"%");

    //Kappa
    double Pa = overall/100.;
    double Pe = 0;
    for(int i=0;i<categories.size();++i)
    {
        Pe += matrixData.at<int>(categories.size(),i)*matrixData.at<int>(i,categories.size());
    }
    Pe /= (double)(info.confusionParams.size()*info.confusionParams.size());
    double kappa = (Pa-Pe)/(1.-Pe);
    ui->kappaLineEdit->setText(QString::number(kappa));
}
