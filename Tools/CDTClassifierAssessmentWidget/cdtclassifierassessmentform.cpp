#include "cdtclassifierassessmentform.h"
#include "ui_cdtclassifierassessmentform.h"
#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif
#include <opencv2/core/core.hpp>
#include <iostream>

CDTClassifierAssessmentForm::CDTClassifierAssessmentForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDTClassifierAssessmentForm)
{
    ui->setupUi(this);
}

CDTClassifierAssessmentForm::~CDTClassifierAssessmentForm()
{
    delete ui;
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
                ui->treeWidget,QStringList()<<tr("PCA Params")<<QString::number(info.pcaParams));
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
