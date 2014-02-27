#ifndef CDTPROJECT_H
#define CDTPROJECT_H

#include <QtCore>
#include <QVector>
#include <QStandardItemModel>
#include "cdtimagelayer.h"
#include "cdtprojecttreemodel.h"
#include <QAction>
#include "cdtbaseobject.h"

class CDTProject: public CDTBaseObject
{
    Q_OBJECT
public:
    explicit CDTProject(QObject *parent = 0);
    friend QDataStream &operator <<(QDataStream &out,const CDTProject &project);
    friend QDataStream &operator >>(QDataStream &in, CDTProject &project);
    friend class CDTProjectTreeModel;

    void addImageLayer(CDTImageLayer *image);

    void setName(const QString& n);
    void setPath(const QString& p);

signals:
    void projectChanged(CDTProject*);

public slots:    
    void addImageLayer();
    void onContextMenuRequest(QWidget *parent);

private slots:
    void childrenChanged();
private:
    QString name;
    QString path;
    bool    isFileExsit;
    QVector<CDTImageLayer *> images;

    QAction* actionAddImage;

//    void updateTreeModel(CDTProjectTreeModel* model);
};

#endif // CDTPROJECT_H
