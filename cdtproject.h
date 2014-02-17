#ifndef CDTPROJECT_H
#define CDTPROJECT_H

#include <QtCore>
#include <QVector>
#include <QStandardItemModel>
#include "cdtimagelayer.h"
#include "cdtprojecttreemodel.h"

class CDTProject: public QObject
{
    Q_OBJECT
public:
    explicit CDTProject(QObject *parent = 0);
    friend QDataStream &operator <<(QDataStream &out,const CDTProject &project);
    friend QDataStream &operator >>(QDataStream &in, CDTProject &project);
    friend class CDTProjectTreeModel;

    void setName(const QString& n);
    void setPath(const QString& p);
signals:
public slots:
private:
    QString name;
    QString path;
    bool    isFileExsit;
    QVector<CDTImageLayer> images;    

//    void updateTreeModel(CDTProjectTreeModel* model);
};

#endif // CDTPROJECT_H
