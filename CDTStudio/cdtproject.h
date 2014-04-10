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
    friend class CDTSegmentationLayer;

    void addImageLayer(CDTImageLayer *image);
    QString path()const;
    QString name()const;

signals:
    void projectChanged(CDTProject*);

public slots:    
    void addImageLayer();
    void removeImageLayer(CDTImageLayer *image);
    void removeAllImageLayers();
    void onContextMenuRequest(QWidget *parent);
    void onActionRename();    
    void setName(const QString& n);
    void setPath(const QString& p);

private slots:
    void childrenChanged();
private:
    QString projectName;
    QString projectPath;
    bool    isFileExsit;
    QVector<CDTImageLayer *> images;    

    QAction* actionAddImage;
    QAction* removeAllImages;
    QAction* actionRename;


//    void updateTreeModel(CDTProjectTreeModel* model);
};

#endif // CDTPROJECT_H
