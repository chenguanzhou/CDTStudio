#ifndef CDTSEGMENTATIONLAYER_H
#define CDTSEGMENTATIONLAYER_H

#include "cdtbaselayer.h"
#include "dialogdbconnection.h"

class QWidgetAction;
class QColor;
class QgsFeatureRendererV2;
class CDTClassificationLayer;
class CDTMapToolSelectTrainingSamples;
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

class CDTSegmentationLayer:public CDTBaseLayer
{
    Q_OBJECT
    Q_CLASSINFO("CDTSegmentationLayer","Segmentation")
    Q_CLASSINFO("tableName",tr("segmentationlayer"))
    Q_PROPERTY(QString Name READ name WRITE setName DESIGNABLE true USER true)
    Q_PROPERTY(QColor Border_Color READ borderColor WRITE setBorderColor NOTIFY borderColorChanged DESIGNABLE true USER true)
    Q_PROPERTY(QString Method READ method  DESIGNABLE true USER true)

public:
    explicit CDTSegmentationLayer(QUuid uuid, QObject *parent = 0);
    ~CDTSegmentationLayer();

    friend QDataStream &operator<<(QDataStream &out,const CDTSegmentationLayer &segmentation);
    friend QDataStream &operator>>(QDataStream &in, CDTSegmentationLayer &segmentation);

    QString shapefilePath() const;
    QString markfilePath() const;
    QString shapefileTempPath() const;
    QString markfileTempPath() const;
    QString method()const;
    CDTDatabaseConnInfo databaseURL() const;
    QColor  borderColor()const;
    QString imagePath()const;
    int     layerTransparency() const;
    QList<QAbstractTableModel *> tableModels();

    void setRenderer(QgsFeatureRendererV2 *r);
    void setOriginRenderer();

    static QList<CDTSegmentationLayer *> getLayers();
    static CDTSegmentationLayer *getLayer(QUuid id);

signals:
    void methodParamsChanged();
    void borderColorChanged(QColor);
    void layerTransparencyChanged(int);
    void removeSegmentation(CDTSegmentationLayer*);

public slots:
    void editDBInfo();
    void generateAttributes();
    void exportShapefile();
    void remove();

    void addClassification();
    void removeClassification(CDTClassificationLayer *);
    void removeAllClassifications();
    void decisionFusion();

//    void setName(const QString& name);
    void setBorderColor(const QColor &clr);
    void setLayerTransparency(int transparency);
    void initLayer(
            const QString& name,
            const QString &shpPath,
            const QString &mkPath,
            const QString &method,
            const QVariantMap &params,
            CDTDatabaseConnInfo url,
            const QColor &borderColor);

    void setDatabaseURL(CDTDatabaseConnInfo url);

private:
    QgsFeatureRendererV2 *renderer();
    void addClassification(CDTClassificationLayer* classification);
    void loadSamplesFromStruct(const QMap<QString,QString> &sample_id_name,const QList<SampleElement> &samples);
    void saveSamplesToStruct(QMap<QString,QString> &sample_id_name,QList<SampleElement> &samples) const;

private:
    QVector<CDTClassificationLayer *> classifications;

    CDTProjectTreeItem* classificationRootItem;

    static QList<CDTSegmentationLayer *> layers;
    static QStringList attributeNames();
};
QDataStream &operator<<(QDataStream &out,const CDTSegmentationLayer &segmentation);
QDataStream &operator>>(QDataStream &in, CDTSegmentationLayer &segmentation);
#endif // CDTSEGMENTATIONLAYER_H
