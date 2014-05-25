#include "wizardnewclassification.h"
#include "ui_wizardnewclassification.h"
#include "stable.h"
#include "cdtsegmentationlayer.h"
#include "cdtattributesinterface.h"
#include "cdtclassifierinterface.h"

extern QList<CDTAttributesInterface *>     attributesPlugins;
extern QList<CDTClassifierInterface *>     classifierPlugins;

WizardNewClassification::WizardNewClassification(QWidget *parent) :
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
    connect(this,SIGNAL(currentIdChanged(int)),SLOT(onCurrentPageChanged(int)));
    connect(this,SIGNAL(customButtonClicked(int)),SLOT(onButtonClicked(int)));

    QSqlQueryModel *modelSegmentation = new QSqlQueryModel(this);
    modelSegmentation->setQuery("select t1.name||' -> '||t2.name,t2.id,t2.name from imagelayer t1,segmentationlayer t2 where t1.id=t2.imageID",QSqlDatabase::database("category"));
    ui->comboBoxInput->setModel(modelSegmentation);

    ui->listViewSelectedFeatures->setModel(modelSelectedFeature);

    this->setButtonText(CustomButton1,tr("Start Classification"));

    initClassifiers();
}

WizardNewClassification::~WizardNewClassification()
{
    delete ui;
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
        ;
        finished = true;
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
}
