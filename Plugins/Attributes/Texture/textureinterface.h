#ifndef TEXTUREINTERFACE_H
#define TEXTUREINTERFACE_H

#include "cdtattributesinterface.h"


class TextureInterface : public CDTAttributesInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTAttributesInterface" FILE "texture.json")

#else
    Q_INTERFACES(CDTAttributesInterface)

#endif // QT_VERSION >= 0x050000

public:
    TextureInterface( QObject *parent = 0);
    ~TextureInterface();

    QString attributesType() const;
    QString tableName() const;

    QStringList attributesName(QString name,QString funcName);

    static QStringList GetGlcmHaralickName();
    static QStringList GetGldvHaralickName();

    Q_INVOKABLE CDT_ATTRIBUTE_SINGLE_BAND_ANGLE QVector<qreal> GLCM(
            const AttributeParamsSingleAngleBand& param) const;
    Q_INVOKABLE CDT_ATTRIBUTE_SINGLE_BAND_ANGLE QVector<qreal> GLDV(
            const AttributeParamsSingleAngleBand& param) const;
private:
    QPoint estimateOffset(int angle)const;
    QMap<QString,QStringList> _mapHaralickNames;
};

#endif // TEXTUREINTERFACE_H
