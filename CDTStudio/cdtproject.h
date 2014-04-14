#ifndef CDTPROJECT_H
#define CDTPROJECT_H

#include <QtCore>
#include <QVector>
#include <QStandardItemModel>
#include "cdtimagelayer.h"
#include "cdtprojecttreemodel.h"
#include <QAction>
#include "cdtbaseobject.h"
#include "qgsmapcanvas.h"

class CDTProject: public CDTBaseObject
{
    Q_OBJECT
public:
    explicit CDTProject(QUuid uuid,QObject *parent = 0);
    ~CDTProject();

    friend QDataStream &operator <<(QDataStream &out,const CDTProject &project);
    friend QDataStream &operator >>(QDataStream &in, CDTProject &project);
    friend class CDTProjectTreeModel;
    friend class CDTSegmentationLayer;

    void addImageLayer(CDTImageLayer *image);
    void insertToTable(QString name);
    QString name()const;

signals:
    void projectChanged();

public slots:    
    void addImageLayer();
    void removeImageLayer(CDTImageLayer *image);
    void removeAllImageLayers();
    void onContextMenuRequest(QWidget *parent);
    void onActionRename();    
    void setName(const QString& name);

private:
    bool    isFileExsit;
    QVector<CDTImageLayer *> images;

    QAction* actionAddImage;
    QAction* removeAllImages;
    QAction* actionRename;

//    void updateTreeModel(CDTProjectTreeModel* model);
};

#endif // CDTPROJECT_H
