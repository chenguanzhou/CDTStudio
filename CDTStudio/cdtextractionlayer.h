#ifndef CDTEXTRACTIONLAYER_H
#define CDTEXTRACTIONLAYER_H

#include "cdtbaseobject.h"

class QColor;
class QAction;
class QWidgetAction;
class QgsFeatureRendererV2;

class CDTExtractionLayer : public CDTBaseObject
{
    Q_OBJECT

public:    
    explicit CDTExtractionLayer(QUuid uuid,QObject *parent);
    ~CDTExtractionLayer();

    friend QDataStream &operator<<(QDataStream &out,const CDTExtractionLayer &extraction);
    friend QDataStream &operator>>(QDataStream &in, CDTExtractionLayer &extraction);

    QString     name()          const;
    QString     shapefilePath() const;
    QColor      color()         const;
    QColor      borderColor()   const;
    double      opacity()       const;
    QString     imagePath()     const;

    void setRenderer(QgsFeatureRendererV2 *r);
    void setOriginRenderer();

    static QList<CDTExtractionLayer *>  getLayers();
    static CDTExtractionLayer *         getLayer(QUuid id);

public slots:
    void    onContextMenuRequest(QWidget *parent);
    void    setName(const QString& name);
    void    setColor(const QColor &clr);
    void    setBorderColor(const QColor &clr);
    void    setOpacity(const double &val) ;
    void    setOpacity(const int &val);
    void    initLayer(const QString& name,
            const QString &shpPath,
            const QColor &color,
            const QColor &borderColor,
            double opacity);
    void    rename();
    void    remove();
    void    exportShapefile();

signals:
    void    nameChanged();
    void    extractionChanged();
    void    removeExtraction(CDTExtractionLayer*);

private:
    QWidgetAction   *actionChangeParams;
    QAction         *actionRemoveExtraction;
    QAction         *actionRename;
    QAction         *actionExportShapefile;

    static QList<CDTExtractionLayer *> layers;
};

QDataStream &operator<<(QDataStream &out,const CDTExtractionLayer &extraction);
QDataStream &operator>>(QDataStream &in, CDTExtractionLayer &extraction);

#endif // CDTEXTRACTIONLAYER_H
