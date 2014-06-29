#ifndef CDTEXTRACTIONLAYER_H
#define CDTEXTRACTIONLAYER_H

#include "cdtbaseobject.h"

class QColor;

class CDTExtractionLayer : public CDTBaseObject
{
    Q_OBJECT
public:
    explicit CDTExtractionLayer(QUuid uuid,QObject *parent);

    friend QDataStream &operator<<(QDataStream &out,const CDTExtractionLayer &extraction);
    friend QDataStream &operator>>(QDataStream &in, CDTExtractionLayer &extraction);

    QString name()const;
    QString shapefilePath() const;
    QColor  color()const;
    QString imagePath()const;


public slots:
    void onContextMenuRequest(QWidget *parent);
    void setName(const QString& name);
    void setBorderColor(const QColor &clr);
    void initLayer(const QString& name,
            const QString &shpPath,
            const QColor &color);
signals:
    void nameChanged();
    void extractionChanged();

public slots:

};

QDataStream &operator<<(QDataStream &out,const CDTExtractionLayer &extraction);
QDataStream &operator>>(QDataStream &in, CDTExtractionLayer &extraction);

#endif // CDTEXTRACTIONLAYER_H
