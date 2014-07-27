#ifndef CDTPROJECT_H
#define CDTPROJECT_H

#include "cdtbaseobject.h"

class QAction;
class CDTImageLayer;
class CDTFileSystem;

class CDTProject: public CDTBaseObject
{
    Q_OBJECT
    Q_CLASSINFO("CDTProject",tr("Project"))
    Q_PROPERTY(QString Name READ name WRITE setName DESIGNABLE true USER true)
public:
    explicit CDTProject(QUuid uuid,QObject *parent = 0);
    ~CDTProject();

    friend QDataStream &operator <<(QDataStream &out,const CDTProject &project);
    friend QDataStream &operator >>(QDataStream &in, CDTProject &project);
    friend class CDTProjectTreeModel;
    friend class CDTBaseObject;
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
    void rename();    
    void setName(const QString& name);

private:
    bool    isFileExsit;
    QVector<CDTImageLayer *> images;
    CDTFileSystem* fileSystem;

    QAction* actionAddImage;
    QAction* removeAllImages;
    QAction* actionRename;

//    void updateTreeModel(CDTProjectTreeModel* model);
};

#endif // CDTPROJECT_H
