#ifndef CDTCLASSIFICATION_H
#define CDTCLASSIFICATION_H

#include "cdtbaseobject.h"

class QAction;
class QgsFeatureRendererV2;
class CDTProjectTreeItem;

class CDTClassification:public CDTBaseObject
{
    Q_OBJECT   
public:
    explicit CDTClassification(QUuid uuid,QObject* parent=0);
    ~CDTClassification();

    friend QDataStream &operator<<(QDataStream &out, const CDTClassification &classification);
    friend QDataStream &operator>>(QDataStream &in, CDTClassification &classification);

    QString name()const;
    QString method()const;    
    QVariantMap params()const;
    QVariantList data()const;
    QVariantMap clsInfo()const;
    QString normalizeMethod()const;
    QString pcaParams()const;

    QgsFeatureRendererV2* renderer();

    void initClassificationLayer(
            const QString &name,
            const QString &methodName,
            const QMap<QString, QVariant> &param,
            const QList<QVariant> &data,
            const QMap<QString, QVariant> &clsInfo,
            const QString &normalizeMethod,
            const QString &pcaParams);

signals:
    void removeClassification(CDTClassification*);
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

    CDTProjectTreeItem* paramRootItem;
    CDTProjectTreeItem* paramRootValueItem;
    CDTProjectTreeItem* normalizeItem;
    CDTProjectTreeItem* pcaItem;
};

QDataStream &operator<<(QDataStream &out, const CDTClassification &classification);
QDataStream &operator>>(QDataStream &in, CDTClassification &classification);

#endif // CDTCLASSIFICATION_H
