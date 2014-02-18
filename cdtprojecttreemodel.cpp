#include "cdtprojecttreemodel.h"
#include "cdtprojecttreeitem.h"

CDTProjectTreeModel::CDTProjectTreeModel(QObject *parent) :
    QStandardItemModel(parent)
{
    setColumnCount(2);
    setHorizontalHeaderLabels(QStringList()<<tr("Layer")<<tr("Value"));
}

void CDTProjectTreeModel::update(CDTProject *project)
{
    this->removeRows(0,this->rowCount());
    CDTProjectTreeItem *item = new CDTProjectTreeItem(CDTProjectTreeItem::PROJECT_ROOT,project->name);
    appendRow(item);
    CDTProjectTreeItem *param = new CDTProjectTreeItem(CDTProjectTreeItem::PARAM,tr("path"));
    CDTProjectTreeItem *value = new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,project->path);
    item->setChild(0,0,param);
    item->setChild(0,1,value);

    for (int i=0;i<(project->images).size();++i)
    {
         project->images[i].updateTreeModel(item);
    }
}
