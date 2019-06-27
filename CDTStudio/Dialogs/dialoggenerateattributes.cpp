#include "dialoggenerateattributes.h"
#include "ui_dialoggenerateattributes.h"

#include "stable.h"
#include "cdtfilesystem.h"
#include "cdtattributesinterface.h"
#include "cdtattributegenerator.h"
#include "cdtsegmentationlayer.h"

extern QList<CDTAttributesInterface *>     attributesPlugins;

DialogGenerateAttributes::DialogGenerateAttributes(QUuid segmentationID, int bandCount, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGenerateAttributes),
    segID(segmentationID),
    _bandCount(bandCount),
    attributeGenerator(Q_NULLPTR)
{
    ui->setupUi(this);
    showPlugins();
}

DialogGenerateAttributes::~DialogGenerateAttributes()
{
    delete ui;
    toolBoxWidgets.clear();
}

int DialogGenerateAttributes::bandCount() const
{
    return _bandCount;
}

void DialogGenerateAttributes::showPlugins()
{
    foreach (CDTAttributesInterface* plugin, attributesPlugins) {
        QString typeName = plugin->attributesType();
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidgetAll);
        item->setText(0,typeName);
        ui->treeWidgetAll->addTopLevelItem(item);
        QList<AttributeMethod> methods = plugin->attributesMethods();
        foreach (AttributeMethod method, methods) {
            QTreeWidgetItem *itemChild = new QTreeWidgetItem(item);
            itemChild->setText(0,method.methodName);
            itemChild->setData(0,Qt::UserRole,QString("attribute"));
            if (method.methodType == QString("CDT_ATTRIBUTE_SINGLE_BAND") ||
                    method.methodType == QString("CDT_ATTRIBUTE_SINGLE_BAND_ANGLE"))
            {
                for (int i=0;i<_bandCount;++i)
                {
                    QTreeWidgetItem *itemBand = new QTreeWidgetItem(itemChild);
                    itemBand->setText(0,QString("band")+QString::number(i+1));
                    if(method.methodType == QString("CDT_ATTRIBUTE_SINGLE_BAND_ANGLE"))
                    {
                        for(int j=0;j< 4;++j)
                        {
                            QTreeWidgetItem *itemAngle = new QTreeWidgetItem(itemBand);
                            itemAngle->setText(0,QString("angle")+QString::number(j*45));
                        }
                    }
                }
            }
        }
    }
    ui->treeWidgetAll->expandAll();

    ui->toolBoxSelected->removeItem(0);
    foreach (CDTAttributesInterface* plugin, attributesPlugins) {
        QString typeName = plugin->attributesType();
        QListWidget* widget = new QListWidget(this);
        ui->toolBoxSelected->addItem(widget,typeName);
        toolBoxWidgets.insert(typeName,widget);
        widget->setSelectionMode(QAbstractItemView::MultiSelection);
        widget->setSortingEnabled(true);
        connect(widget,SIGNAL(itemSelectionChanged()),this,SLOT(onToolBoxSelectionChanged()));
    }

}

int DialogGenerateAttributes::itemLevel(QTreeWidgetItem *item)
{
    int level = 0;
    QTreeWidgetItem *p = item;
    while (p) {
        p = p->parent();
        ++level;
    }
    return level;
}

void DialogGenerateAttributes::on_pushButtonRemoveAll_clicked()
{
    QList<QListWidget*> widgets = toolBoxWidgets.values();
    foreach (QListWidget* widget, widgets) {
        while(widget->item(0))
        {
            QListWidgetItem* item = widget->item(0);
            widget->removeItemWidget(item);
            delete item;
        }
    }
}

void DialogGenerateAttributes::on_pushButtonRemove_clicked()
{
    QList<QListWidget*> widgets = toolBoxWidgets.values();
    foreach (QListWidget* widget, widgets) {
        QList<QListWidgetItem*> items = widget->selectedItems();
        foreach (QListWidgetItem* item, items) {
            widget->removeItemWidget(item);
            delete item;
        }
    }
}

void DialogGenerateAttributes::on_pushButtonAdd_clicked()
{
    QList<QTreeWidgetItem*> items = ui->treeWidgetAll->selectedItems();
    foreach (QTreeWidgetItem* item, items) {
        QTreeWidgetItem* p = item;
        QString attribute = item->text(0);
        while (p->data(0,Qt::UserRole).toString()!=QString("attribute")) {
            p = p->parent();
            attribute = p->text(0) + "_" + attribute;
        }
        QListWidget* widget = toolBoxWidgets.value(p->parent()->text(0));
        if (widget)
        {
            if (widget->findItems(attribute,Qt::MatchExactly).size()==0)
                widget->addItem(attribute);
        }
    }
}

void DialogGenerateAttributes::on_pushButtonAddAll_clicked()
{
    ui->treeWidgetAll->selectAll();
    on_pushButtonAdd_clicked();
}

void DialogGenerateAttributes::on_pushButtonGenerate_clicked()
{
    QMap<QString,QStringList> attributes;
    int attributeCount = 0;
    foreach (QString type, toolBoxWidgets.keys()) {
        QListWidget* widget = toolBoxWidgets.value(type);
        if (widget)
        {
            QStringList items;
            for (int i=0;i<widget->count();++i)
            {
                items<<widget->item(i)->text();
                ++attributeCount ;
            }
            attributes.insert(type,items);
        }
    }
    if (attributeCount == 0)
    {
        QMessageBox::critical(this,tr("Error"),tr("No attribute is selected!"));
        return;
    }

    CDTSegmentationLayer* segmentationLayer =
            CDTSegmentationLayer::getLayer(segID);

    if (QSqlDatabase::contains("attribute")==false)
    {
        QMessageBox::critical(this,tr("Error"),tr("Database \"attribute\" is not found"));
        return;
    }

    if (attributeGenerator) delete attributeGenerator;
    attributeGenerator = new CDTAttributeGenerator(
                segmentationLayer->imagePath(),
                segmentationLayer->markfileTempPath(),
                segmentationLayer->shapefileTempPath(),
                QSqlDatabase::database("attribute"),
                attributes,
                this);
    qDebug()<<segmentationLayer->imagePath();
    qDebug()<<segmentationLayer->markfileTempPath();
    qDebug()<<segmentationLayer->shapefileTempPath();

    if(!attributeGenerator->isValid())
    {
        QMessageBox::critical(this,tr("Error"),tr("Construct attribute generator failed!\n")+attributeGenerator->errorInfo());
        return;
    }
    //    connect(attributeGenerator, SIGNAL(finished()), this, SLOT(onFinished()));
    connect(attributeGenerator, SIGNAL(showWarningMessage(QString)),this,SLOT(onShowWarningMessage(QString)));
    connect(attributeGenerator, SIGNAL(progressBarSizeChanged(int,int)),ui->progressBar,SLOT(setRange(int,int)));
    connect(attributeGenerator, SIGNAL(progressBarValueChanged(int)),ui->progressBar,SLOT(setValue(int)));
    connect(attributeGenerator, SIGNAL(currentProgressChanged(QString)),ui->labelProgress,SLOT(setText(QString)));
    attributeGenerator->start();

    ui->widget->setEnabled(false);
    ui->pushButtonGenerate->setEnabled(false);
    ui->pushButtonCancel->setEnabled(false);
    ui->pushButtonCancel->setText(tr("Cancel"));

    connect(attributeGenerator,SIGNAL(finished()),this,SLOT(onFinished()));
    connect(attributeGenerator,SIGNAL(finished()),attributeGenerator,SLOT(deleteLater()));
}

void DialogGenerateAttributes::on_pushButtonCancel_clicked()
{
    if (attributeGenerator)
        attributeGenerator->quit();

    this->close();
}

void DialogGenerateAttributes::on_treeWidgetAll_itemSelectionChanged()
{
    QList<QTreeWidgetItem*> items = ui->treeWidgetAll->selectedItems();
    foreach (QTreeWidgetItem* item, items) {
        if(item->childCount()!=0)
        {
            item->setSelected(false);
        }
    }

    ui->pushButtonAdd->setEnabled(ui->treeWidgetAll->selectedItems().size()!=0);
}

void DialogGenerateAttributes::onToolBoxSelectionChanged()
{
    bool enabled = false;

    QList<QListWidget*> widgets = toolBoxWidgets.values();
    foreach (QListWidget* widget, widgets) {
        if (widget->selectedItems().size()!=0)
        {
            enabled = true;
            break;
        }
    }
    ui->pushButtonRemove->setEnabled(enabled);
}

void DialogGenerateAttributes::onShowWarningMessage(QString msg)
{
    QMessageBox::warning(this,tr("warning"),msg);
}

void DialogGenerateAttributes::onFinished()
{
    ui->widget->setEnabled(true);
    ui->pushButtonGenerate->setEnabled(true);
    ui->pushButtonCancel->setEnabled(true);
    ui->pushButtonCancel->setText(tr("Finish"));
    attributeGenerator = Q_NULLPTR;
}


