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

    QgsFeatureRendererV2* renderer();

    void initClassificationLayer(const QString &name,
            const QString &methodName,
            const QMap<QString, QVariant> &params,
            const QList<QVariant> &data,
            const QMap<QString, QVariant> &clsInfo,
            const QString &normalizeMethod,
            const QString &pcaParams);

signals:
    void removeClassification(CDTClassificationLayer*);
    void classificationLayerChanged();

public slots:
    void onContextMenuRequest(QWidget *parent);
    void rename();
    void remove();

private:
    void setName(const QString& name);

private:
    QAction* actionRemoveClassification;
    QAction *actionRename;

//    CDTProjectTreeItem* paramRootItem;
//    CDTProjectTreeItem* paramRootValueItem;
//    CDTProjectTreeItem* normalizeItem;
//    CDTProjectTreeItem* pcaItem;
};

QDataStream &operator<<(QDataStream &out, const CDTClassificationLayer &classification);
QDataStream &operator>>(QDataStream &in, CDTClassificationLayer &classification);

#endif // CDTCLASSIFICATION_H
