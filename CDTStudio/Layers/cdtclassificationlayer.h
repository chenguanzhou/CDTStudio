#ifndef CDTCLASSIFICATION_H
#define CDTCLASSIFICATION_H

#include "cdtbaselayer.h"

class QAction;
class QgsFeatureRendererV2;
class CDTProjectTreeItem;

class CDTClassificationLayer:public CDTBaseLayer
{
    Q_OBJECT   
    Q_CLASSINFO("CDTClassificationLayer","Classification")
    Q_PROPERTY(QString Name READ name WRITE setName DESIGNABLE true USER true)
    Q_PROPERTY(QString Method READ method  DESIGNABLE true USER true)
    Q_PROPERTY(QString Normalization_Param READ normalizeMethod  DESIGNABLE true USER true)
    Q_PROPERTY(QString PCA_Param READ pcaParams  DESIGNABLE true USER true)
public:
    explicit CDTClassificationLayer(QUuid uuid,QObject* parent=0);
    ~CDTClassificationLayer();

    friend QDataStream &operator<<(QDataStream &out, const CDTClassificationLayer &classification);
    friend QDataStream &operator>>(QDataStream &in, CDTClassificationLayer &classification);

    QString         name()              const;
    QString         method()            const;
    QVariantMap     params()            const;
    QVariantList    data()              const;
    QVariantMap     clsInfo()           const;
    QString         normalizeMethod()   const;
    QString         pcaParams()         const;
    QStringList     selectedFeatures()  const;

    QgsFeatureRendererV2* renderer();

    void initClassificationLayer(const QString &name,
            const QString &methodName,
            const QMap<QString, QVariant> &params,
            const QList<QVariant> &data,
            const QMap<QString, QVariant> &clsInfo,
            const QString &normalizeMethod,
            const QString &pcaParams,
            const QStringList &selectedFeatures);

    static QList<CDTClassificationLayer *> getLayers();
    static CDTClassificationLayer *getLayer(QUuid id);
signals:
    void removeClassification(CDTClassificationLayer*);

public slots:
    void rename();
    void remove();
    void showAccuracy();

private:
    void setName(const QString& name);

private:
    QStringList featuresList;
    static QList<CDTClassificationLayer *> layers;
};

QDataStream &operator<<(QDataStream &out, const CDTClassificationLayer &classification);
QDataStream &operator>>(QDataStream &in, CDTClassificationLayer &classification);

#endif // CDTCLASSIFICATION_H
