#ifndef CDTPROJECT_H
#define CDTPROJECT_H

#include <QtCore>
#include <QVector>
#include <QStandardItemModel>
#include "cdtimagelayer.h"
#include "cdtprojecttreemodel.h"
#include <QAction>

class CDTProject: public QObject
{
    Q_OBJECT
public:
    explicit CDTProject(QObject *parent = 0);
    friend QDataStream &operator <<(QDataStream &out,const CDTProject &project);
    friend QDataStream &operator >>(QDataStream &in, CDTProject &project);
    friend class CDTProjectTreeModel;

    void addImageLayer(CDTImageLayer *image);
    void addImageLayer(const QString name,const QString path);

    void setName(const QString& n);
    void setPath(const QString& p);

signals:
public slots:
    void addImageLayer();
    void onContextMenu(QWidget *parent);
private:
    QString name;
    QString path;
    bool    isFileExsit;
    QVector<CDTImageLayer *> images;

    QAction* actionAddImage;

//    void updateTreeModel(CDTProjectTreeModel* model);
};

#endif // CDTPROJECT_H
