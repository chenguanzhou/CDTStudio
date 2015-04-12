#ifndef CDTVECTORCHANGELAYER_H
#define CDTVECTORCHANGELAYER_H

#include "cdtbaselayer.h"

class QgsFeatureRendererV2;

class CDTVectorChangeLayer : public CDTBaseLayer
{
    Q_OBJECT
    Q_CLASSINFO("CDTVectorChangeLayer","Vector-based change detection")
    Q_CLASSINFO("tableName","vector_change")
    Q_PROPERTY(QString Name READ name WRITE setName DESIGNABLE true USER true)
public:
    explicit CDTVectorChangeLayer(QUuid uuid, QObject *parent = 0);
    ~CDTVectorChangeLayer();

    friend QDataStream &operator<<(QDataStream &out,const CDTVectorChangeLayer &change);
    friend QDataStream &operator>>(QDataStream &in, CDTVectorChangeLayer &change);

    QString shapefileID()const;

signals:
    void removeVectorChangeLayer(CDTVectorChangeLayer*);

public slots:
    void exportShapefile();
    void remove();
    void setOriginRenderer();
    void setRenderer(QgsFeatureRendererV2 *r);
    QgsFeatureRendererV2 * changeViewRenderer();

    void initLayer(
            const QString& name,
            const QString &shapefileID,
            const QString& clsID_T1,
            const QString& clsID_T2,
            const QVariantMap &params);
};

#endif // CDTVECTORCHANGELAYER_H
