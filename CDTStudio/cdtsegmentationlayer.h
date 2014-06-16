#ifndef CDTSEGMENTATIONLAYER_H
#define CDTSEGMENTATIONLAYER_H

#include "cdtbaseobject.h"
#include "cdtattributedockwidget.h"

class CDTDatabaseConnInfo;
class CDTClassification;
class CDTMapToolSelectTrainingSamples;
class CDTTrainingSamplesForm;
class CDTProjectTreeItem;
class QgsFeatureRendererV2;
class QWidgetAction;

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

    QString name()const;
    QString shapefilePath() const;
    QString markfilePath() const;
    QString method()const;
    CDTDatabaseConnInfo databaseURL() const;
    QColor  color()const;
    QString imagePath()const;


    void setRenderer(QgsFeatureRendererV2 *r);
    void setOriginRenderer();

    static QList<CDTSegmentationLayer *> getLayers();
    static CDTSegmentationLayer * getLayer(QUuid uuid);

signals:
    void methodParamsChanged();
    void nameChanged();
    void segmentationChanged();
    void removeSegmentation(CDTSegmentationLayer*);

public slots:
    void onContextMenuRequest(QWidget *parent);
    void rename();
    void remove();


    void addClassification();
    void removeClassification(CDTClassification *);
    void removeAllClassifications();

    void setName(const QString& name);
    void setBorderColor(const QColor &clr);
    void initSegmentationLayer(const QString& name,
            const QString &shpPath,
            const QString &mkPath,
            const QString &method,
            const QVariantMap &params,
            CDTDatabaseConnInfo url,
            const QColor &color);

    void setDatabaseURL(CDTDatabaseConnInfo url);

private:
    QgsFeatureRendererV2 *renderer();
    void addClassification(CDTClassification* classification);
    void loadSamplesFromStruct(const QMap<QString,QString> &sample_id_name,const QList<SampleElement> &samples);
    void saveSamplesToStruct(QMap<QString,QString> &sample_id_name,QList<SampleElement> &samples) const;

private:
    QString m_imagePath;
    QVector<CDTClassification *> classifications;

    QAction *addClassifications;
    QAction *actionRemoveSegmentation;
    QAction *actionRemoveAllClassifications;
    QAction *actionRename;
    QWidgetAction *actionChangeBorderColor;

    CDTProjectTreeItem* shapefileItem;
    CDTProjectTreeItem* markfileItem;
    CDTProjectTreeItem* paramRootItem;
    CDTProjectTreeItem* paramRootValueItem;
    CDTProjectTreeItem* classificationRootItem;

    static QList<CDTSegmentationLayer *> layers;
};
QDataStream &operator<<(QDataStream &out,const CDTSegmentationLayer &segmentation);
QDataStream &operator>>(QDataStream &in, CDTSegmentationLayer &segmentation);
#endif // CDTSEGMENTATIONLAYER_H
