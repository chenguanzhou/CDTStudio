#ifndef CDTVECTORCHANGELAYER_H
#define CDTVECTORCHANGELAYER_H

#include "cdtbaselayer.h"

class CDTVectorChangeLayer : public CDTBaseLayer
{
    Q_OBJECT
    Q_CLASSINFO("CDTVectorCHangeLayer","Vector-based change detection")
    Q_PROPERTY(QString Name READ name WRITE setName DESIGNABLE true USER true)
public:
    explicit CDTVectorChangeLayer(QUuid uuid, QObject *parent = 0);
    ~CDTVectorChangeLayer();

    friend QDataStream &operator<<(QDataStream &out,const CDTVectorChangeLayer &change);
    friend QDataStream &operator>>(QDataStream &in, CDTVectorChangeLayer &change);

    QString name()const;
    QString shapefileID()const;

signals:
    void nameChanged(QString);
    void removeVectorChangeLayer(CDTVectorChangeLayer*);

public slots:
    void rename();
    void remove();

    void setName(const QString& name);

    void initVectorChangeLayer(
            const QString& name,
            const QString &shapefileID,
            const QString& clsID_T1,
            const QString& clsID_T2,
            const QVariantMap &params);
};

#endif // CDTVECTORCHANGELAYER_H
