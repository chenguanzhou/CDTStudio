#ifndef CDTPBCDBINARYLAYER_H
#define CDTPBCDBINARYLAYER_H

#include "cdtpixelchangelayer.h"

class CDTPBCDBinaryLayer : public CDTPixelChangeLayer
{
    Q_OBJECT
    Q_CLASSINFO("CDTPBCDBinaryLayer",tr("Pixel-based Change Detection(Binary) Layer"))
    Q_CLASSINFO("tableName",tr("pbcd_binary"))
    Q_PROPERTY(QString Difference_Image READ diffImage DESIGNABLE true USER true)
    Q_PROPERTY(QString Positive_Threshold READ posT DESIGNABLE true USER true)
    Q_PROPERTY(QString Negetive_Threshold READ negT DESIGNABLE true USER true)
public:
    Q_INVOKABLE explicit CDTPBCDBinaryLayer(QUuid id,QObject *parent = 0);

    QString diffImage() const;
    double posT()const;
    double negT()const;
    QStringList files()const;
signals:

public slots:
    void onContextMenuRequest(QWidget *parent);

    void initLayer(const QString& name,
            const QString& image_t1,
            const QString& image_t2,
            const QVariantMap &params);
};

#endif // CDTPBCDBINARYLAYER_H
