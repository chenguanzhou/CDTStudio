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
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString shapefilePath READ shapefilePath WRITE setShapefilePath NOTIFY shapefilePathChanged)
    Q_PROPERTY(QString method READ method)
public:
    explicit CDTClassification(QObject* parent=0);

    friend QDataStream &operator<<(QDataStream &out, const CDTClassification &classification);
    friend QDataStream &operator>>(QDataStream &in, CDTClassification &classification);

    QString name()const;
    QString shapefilePath() const;
    QString method()const;

    void setName(const QString& name);
    void setShapefilePath(const QString &shpPath);
    void setMethodParams(const QString& methodName,const QMap<QString,QVariant> &params);

signals:
    void nameChanged();
    void shapefilePathChanged();
    void methodParamsChanged();
    void removeClassification(CDTClassification*);
public slots:
    void updateTreeModel(CDTProjectTreeItem* parent);
    void onContextMenuRequest(QWidget *parent);
    void remove();

private:
    QString m_name;
    QString m_shapefilePath;
    QString m_method;
    QMap<QString,QVariant> m_params;
    QAction* actionRemoveClassification;
};

QDataStream &operator<<(QDataStream &out, const CDTClassification &classification);
QDataStream &operator>>(QDataStream &in, CDTClassification &classification);

#endif // CDTCLASSIFICATION_H
