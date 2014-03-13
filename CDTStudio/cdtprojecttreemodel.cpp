#include "cdtprojecttreemodel.h"
#include "cdtprojecttreeitem.h"
#include <QtCore>

CDTProjectTreeModel::CDTProjectTreeModel(QObject *parent) :
    QStandardItemModel(parent)
{
    setColumnCount(2);
    setHorizontalHeaderLabels(QStringList()<<tr("Layer")<<tr("Value"));
}

void CDTProjectTreeModel::update(CDTProject *project)
{
    this->clear();
    CDTProjectTreeItem *item =
            new CDTProjectTreeItem(CDTProjectTreeItem::PROJECT_ROOT,project->name,project);
    CDTProjectTreeItem *value =
            new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,project->path,project);

    this->invisibleRootItem()->setChild(0,0,item);
    this->invisibleRootItem()->setChild(0,1,value);

    for (int i=0;i<(project->images).size();++i)
    {
         project->images[i]->updateTreeModel(item);
    }

    emit updated();
}
