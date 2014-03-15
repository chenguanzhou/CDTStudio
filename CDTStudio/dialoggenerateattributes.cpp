#include "dialoggenerateattributes.h"
#include "ui_dialoggenerateattributes.h"

#include "cdtattributesinterface.h"

extern QList<CDTAttributesInterface *>     attributesPlugins;

DialogGenerateAttributes::DialogGenerateAttributes(int bandCount,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGenerateAttributes),
    _bandCount(bandCount)
{
    ui->setupUi(this);
    loadPlugin();
}

DialogGenerateAttributes::~DialogGenerateAttributes()
{
    delete ui;
}

int DialogGenerateAttributes::bandCount() const
{
    return _bandCount;
}

void DialogGenerateAttributes::loadPlugin()
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
            if (method.methodType == QString("CDT_ATTRIBUTE_SINGLE_BAND") ||
                    method.methodType == QString("CDT_ATTRIBUTE_SINGLE_BAND_ANGLE"))
            {
                for (int i=0;i<_bandCount;++i)
                {
                    QTreeWidgetItem *itemBand = new QTreeWidgetItem(itemChild);
                    itemBand->setText(0,tr("band")+QString::number(i));
                    itemBand->setCheckState(0,Qt::Unchecked);
                }
            }
        }
    }
    ui->treeWidgetAll->expandToDepth(0);
    ui->treeWidgetAll->depth();

    foreach (CDTAttributesInterface* plugin, attributesPlugins) {
        QString typeName = plugin->attributesType();
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidgetSelect);
        item->setText(0,typeName);
    }
    ui->treeWidgetSelect->expandToDepth(0);
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
    ui->treeWidgetSelect->selectAll();
    on_pushButtonRemove_clicked();
}

void DialogGenerateAttributes::on_pushButtonRemove_clicked()
{
    QList<QTreeWidgetItem*> items = ui->treeWidgetSelect->selectedItems();
    foreach (QTreeWidgetItem* item, items) {
        QString type = item->parent()->text(0);
        QTreeWidgetItem* root = ui->treeWidgetAll->invisibleRootItem();
        for (int i=0;i<root->childCount();i++)
        {
            if (type == root->child(i)->text(0))
            {
                item->parent()->removeChild(item);
                root->child(i)->addChild(item);
            }
        }
    }
}

void DialogGenerateAttributes::on_pushButtonAdd_clicked()
{
    QList<QTreeWidgetItem*> items = ui->treeWidgetAll->selectedItems();
    foreach (QTreeWidgetItem* item, items) {
        QString type = item->parent()->text(0);
        QTreeWidgetItem* root = ui->treeWidgetSelect->invisibleRootItem();
        for (int i=0;i<root->childCount();i++)
        {
            if (type == root->child(i)->text(0))
            {
                item->parent()->removeChild(item);
                root->child(i)->addChild(item);
            }
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

}

void DialogGenerateAttributes::on_treeWidgetAll_itemSelectionChanged()
{
    QList<QTreeWidgetItem*> items = ui->treeWidgetAll->selectedItems();
    foreach (QTreeWidgetItem* item, items) {
        if(itemLevel(item)==1 || itemLevel(item)==3)
        {
            item->setSelected(false);
        }
    }

    ui->pushButtonAdd->setEnabled(ui->treeWidgetAll->selectedItems().size()!=0);
//    ui->pushButtonAddAll->setEnabled(ui->treeWidgetAll->invisibleRootItem()->childCount()!=0);
    ui->pushButtonRemove->setEnabled(ui->treeWidgetSelect->selectedItems().size()!=0);
//    ui->pushButtonRemoveAll->setEnabled(ui->treeWidgetSelect->invisibleRootItem()->childCount()!=0);
}

void DialogGenerateAttributes::on_treeWidgetSelect_itemSelectionChanged()
{
    QList<QTreeWidgetItem*> items = ui->treeWidgetSelect->selectedItems();
    foreach (QTreeWidgetItem* item, items) {
        if(itemLevel(item)==1 || itemLevel(item)==3)
        {
            item->setSelected(false);
        }
    }

    ui->pushButtonAdd->setEnabled(ui->treeWidgetAll->selectedItems().size()!=0);
//    ui->pushButtonAddAll->setEnabled(ui->treeWidgetAll->invisibleRootItem()->childCount()!=0);
    ui->pushButtonRemove->setEnabled(ui->treeWidgetSelect->selectedItems().size()!=0);
//    ui->pushButtonRemoveAll->setEnabled(ui->treeWidgetSelect->invisibleRootItem()->childCount()!=0);
}

