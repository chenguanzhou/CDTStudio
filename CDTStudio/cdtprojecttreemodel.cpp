#include "cdtprojecttreemodel.h"
#include "cdtprojecttreeitem.h"
#include <QtCore>

CDTProjectTreeModel::CDTProjectTreeModel(QObject *parent) :
    QStandardItemModel(parent)
{
    setColumnCount(2);
    setHorizontalHeaderLabels(QStringList()<<tr("Layer")<<tr("Value"));
}

