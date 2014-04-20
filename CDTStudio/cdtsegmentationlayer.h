#ifndef CDTSEGMENTATIONLAYER_H
#define CDTSEGMENTATIONLAYER_H

#include "cdtbaseobject.h"
#include "cdtattributeswidget.h"

class CDTDatabaseConnInfo;
class CDTClassification;
class CDTMapToolSelectTrainingSamples;
class CDTTrainingSamplesForm;
class CDTProjectTreeItem;

class SampleElement
{
public:
    SampleElement(int objID=0,QUuid ctgrID=QUuid(),QUuid spID=QUuid())
        :ObjectID(objID),categoryID(ctgrID),sampleID(spID){}
    friend QDataStream &operator<<(QDataStream &out,const SampleElement &sample);
    friend QDataStream &operator>>(QDataStream &in, SampleElement &sample);
    int ObjectID;
    QUuid categoryID;
    QUuid sampleID;
};

class CDTSegmentationLayer:public CDTBaseObject
{
    Q_OBJECT
    Q_PROPERTY(QString method READ method)

public:
    explicit CDTSegmentationLayer(QUuid uuid, QString imagePath,QObject *parent = 0);
    ~CDTSegmentationLayer();

    friend QDataStream &operator<<(QDataStream &out,const CDTSegmentationLayer &segmentation);
    friend QDataStream &operator>>(QDataStream &in, CDTSegmentationLayer &segmentation);

//    void addClassification(CDTClassification* classification);

    QString name()const;
    QString shapefilePath() const;
    QString markfilePath() const;
    QString method()const;
    CDTDatabaseConnInfo databaseURL() const;
    QString imagePath()const;

    CDTTrainingSamplesForm *trainingForm()const;

    static QList<CDTSegmentationLayer *> getLayers();
    static CDTSegmentationLayer * getLayer(QUuid uuid);

signals:
    void methodParamsChanged();
    void nameChanged();
    void segmentationChanged();
    void removeSegmentation(CDTSegmentationLayer*);
public slots:
    void updateTreeModel(CDTProjectTreeItem* parent);
    void onContextMenuRequest(QWidget *parent);
    void onActionRename();
    void remove();
//    void addClassification();    
//    void removeClassification(CDTClassification *);
//    void removeAllClassifications();
    void onTrainingSamples();
    void setName(const QString& name);
    void setLayerInfo(const QString& name,const QString &shpPath,const QString &mkPath);
    void setMethodParams(const QString& methodName,const QMap<QString,QVariant> &params);
    void setDatabaseURL(CDTDatabaseConnInfo url);

private:
    void loadSamplesFromStruct(const QMap<QString,QString> &sample_id_name,const QList<SampleElement> &samples);
    void saveSamplesToStruct(QMap<QString,QString> &sample_id_name,QList<SampleElement> &samples) const;
private:
    QString m_imagePath;
    CDTDatabaseConnInfo    m_dbUrl;
    QString m_method;
    QMap<QString,QVariant> m_params;
    QVector<CDTClassification *> classifications;
//    CDTTrainingSampleList trainingSampleList;

//    CDTMapToolSelectTrainingSamples* maptoolTraining;

    QAction *addClassifications;
    QAction *actionRemoveSegmentation;
    QAction *actionRemoveAllClassifications;
    QAction *actionRename;
    QAction *actionTrainingSamples;

    CDTProjectTreeItem* shapefileItem;
    CDTProjectTreeItem* markfileItem;
    CDTProjectTreeItem* paramRootItem;
    CDTProjectTreeItem* paramRootValueItem;

    static QList<CDTSegmentationLayer *> layers;
};
QDataStream &operator<<(QDataStream &out,const CDTSegmentationLayer &segmentation);
QDataStream &operator>>(QDataStream &in, CDTSegmentationLayer &segmentation);
#endif // CDTSEGMENTATIONLAYER_H
