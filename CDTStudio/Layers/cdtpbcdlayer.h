#ifndef CDTPBCDLAYER_H
#define CDTPBCDLAYER_H

#include "cdtbaselayer.h"
class CDTPBCDLayer : public CDTBaseLayer
{
    Q_OBJECT
    Q_CLASSINFO("CDTPBCDLayer",tr("Pixel-based change detection"))
    Q_PROPERTY(QString Name READ name WRITE setName DESIGNABLE true USER true)
    Q_PROPERTY(QString T1_Image READ path_t1 DESIGNABLE true USER true)
    Q_PROPERTY(QString T2_Image READ path_t2 DESIGNABLE true USER true)
public:
    explicit CDTPBCDLayer(QUuid uuid,QObject *parent = 0);
    virtual ~CDTPBCDLayer();

    QString name()const;
    QString diffImage() const;
    QString path_t1() const;
    QString path_t2() const;
public slots:
    virtual void onContextMenuRequest(QWidget *parent) = 0;

    void setName(const QString& name);

    void initPBCDBinaryLayer(
            const QString& name,
            const QString& diff_image,
            const QString& image_t1,
            const QString& image_t2,
            const QVariantMap &params);

protected:
    QString tableName;
};

#endif // CDTPBCDLAYER_H
