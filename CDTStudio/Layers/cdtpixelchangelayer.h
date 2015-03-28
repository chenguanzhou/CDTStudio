#ifndef CDTPIXELCHANGELAYER_H
#define CDTPIXELCHANGELAYER_H

#include "cdtbaselayer.h"
#include <QMap>

class CDTPixelChangeLayer : public CDTBaseLayer
{
    Q_OBJECT
    Q_CLASSINFO("CDTPixelChangeLayer",tr("Pixel-based"))
    Q_PROPERTY(QString T1_Image READ image_t1 DESIGNABLE true USER true)
    Q_PROPERTY(QString T2_Image READ image_t2 DESIGNABLE true USER true)

public:
    enum ChangeType{
        PBCD_BINARY,
        PBCD_FROMTO
    };
    explicit CDTPixelChangeLayer(QUuid uuid,QObject *parent = 0);
    virtual ~CDTPixelChangeLayer();

    friend QDataStream &operator<<(QDataStream &out, const CDTPixelChangeLayer &layer);
    friend QDataStream &operator>>(QDataStream &in, CDTPixelChangeLayer &layer);

    QString image_t1()  const;
    QString image_t2()  const;
    QVariantMap params()const;
    virtual QStringList files() const;//Used when layer removed
public slots:
    virtual void onContextMenuRequest(QWidget *parent) = 0;

    virtual void initLayer(const QString& name,
            const QString& image_t1,
            const QString& image_t2,
            const QVariantMap &params);

public:
    ChangeType type;
    static QMap<QString,QMetaObject> changeLayerMetaObjects;
};

#endif // CDTPIXELCHANGELAYER_H
