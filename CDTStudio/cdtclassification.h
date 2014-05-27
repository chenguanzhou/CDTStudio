#ifndef CDTCLASSIFICATION_H
#define CDTCLASSIFICATION_H

#include <QtCore>
#include <QMap>
#include <QVariant>
#include "cdtprojecttreeitem.h"
#include "cdtbaseobject.h"
#include <QAction>



class CDTClassification:public CDTBaseObject
{
    Q_OBJECT   
public:
    explicit CDTClassification(QUuid uuid,QObject* parent=0);

    friend QDataStream &operator<<(QDataStream &out, const CDTClassification &classification);
    friend QDataStream &operator>>(QDataStream &in, CDTClassification &classification);

    QString name()const;
    QString method()const;    
    QMap<QString,QVariant> params()const;
    QList<QVariant> data()const;
    QMap<QString,QVariant> clsInfo()const;

    void initClassificationLayer(
            const QString &name,
            const QString &methodName,
            const QMap<QString, QVariant> &param,
            const QList<QVariant> &data,
            const QMap<QString, QVariant> &clsInfo
    );

signals:
    void removeClassification(CDTClassification*);
    void classificationLayerChanged();

public slots:
    void onContextMenuRequest(QWidget *parent);
    void remove();

private:
    void setName(const QString& name);
    void setMethod(const QString &methodName);
    void setParam(const QMap<QString,QVariant> &param);
    void setData(const QList<QVariant>& data);
    void setClsInfo(const QMap<QString,QVariant>& clsInfo)const;

private:
    QAction* actionRemoveClassification;

    CDTProjectTreeItem* paramRootItem;
    CDTProjectTreeItem* paramRootValueItem;
};

QDataStream &operator<<(QDataStream &out, const CDTClassification &classification);
QDataStream &operator>>(QDataStream &in, CDTClassification &classification);

#endif // CDTCLASSIFICATION_H
