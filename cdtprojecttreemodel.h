#ifndef CDTPROJECTTREEMODEL_H
#define CDTPROJECTTREEMODEL_H

#include <QStandardItemModel>
#include "cdtproject.h"

class CDTProjectTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    friend class CDTProject;
    explicit CDTProjectTreeModel(QObject *parent = 0);

signals:

public slots:
    void update(CDTProject*project);
};

#endif // CDTPROJECTTREEMODEL_H
