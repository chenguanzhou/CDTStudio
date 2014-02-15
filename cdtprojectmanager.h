#ifndef CDTPROJECTMANAGER_H
#define CDTPROJECTMANAGER_H

#include <QObject>
#include <QStandardItemModel>
#include "cdtproject.h"

class CDTProjectManager : public QObject
{
    Q_OBJECT
public:
    explicit CDTProjectManager(QObject *parent = 0);

signals:

public slots:

private:
    QStandardItemModel* treeModel;
    CDTProject *project;
};

#endif // CDTPROJECTMANAGER_H
