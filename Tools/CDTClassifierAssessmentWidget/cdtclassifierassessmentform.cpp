#include "cdtclassifierassessmentform.h"
#include "ui_cdtclassifierassessmentform.h"
#include <QtGui>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#endif

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
                ui->treeWidget,QStringList()<<tr("Classification Name")<<info.clsName);
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

}
