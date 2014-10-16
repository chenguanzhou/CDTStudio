#ifndef CDTCHANGELAYER_H
#define CDTCHANGELAYER_H

#include "cdtbaselayer.h"
#include <QMap>

class CDTChangeLayer : public CDTBaseLayer
{
    Q_OBJECT
    Q_CLASSINFO("CDTChangeLayer",tr("Change Detection Layer"))
    Q_PROPERTY(QString Name READ name WRITE setName DESIGNABLE true USER true)
    Q_PROPERTY(QString T1_Image READ image_t1 DESIGNABLE true USER true)
    Q_PROPERTY(QString T2_Image READ image_t2 DESIGNABLE true USER true)

public:
    enum ChangeType{
        PBCD_BINARY,
        PBCD_FROMTO,
        OBCD_BINARY,
        OBCD_FROMTO
    };
    explicit CDTChangeLayer(QUuid uuid,QObject *parent = 0);
    virtual ~CDTChangeLayer();

    friend QDataStream &operator<<(QDataStream &out, const CDTChangeLayer &layer);
    friend QDataStream &operator>>(QDataStream &in, CDTChangeLayer &layer);

    QString name()const;
    QString image_t1() const;
    QString image_t2() const;
    QVariantMap params()const;
public slots:
    virtual void onContextMenuRequest(QWidget *parent) = 0;

    void setName(const QString& name);

    virtual void initLayer(const QString& name,
            const QString& image_t1,
            const QString& image_t2,
            const QVariantMap &params);

public:
    ChangeType type;
    static QMap<QString,QMetaObject> changeLayerMetaObjects;
};

#endif // CDTCHANGELAYER_H
