#ifndef CDTEXTRACTIONINTERFACE_H
#define CDTEXTRACTIONINTERFACE_H

#include <QtCore>
class QgsMapTool;
class QgsMapCanvas;
class QgsVectorLayer;

class CDTExtractionInterface:public QObject
{
    Q_OBJECT
public:
    explicit CDTExtractionInterface(QObject* parent = 0)
        :QObject(parent)
    {}

    virtual QString methodName()    const = 0;
    virtual QString description()   const = 0;

public slots:
    virtual QgsMapTool* mapTool(QgsMapCanvas* canvas,QString imagePath,QgsVectorLayer *vectorLayer) = 0;
};

Q_DECLARE_INTERFACE(CDTExtractionInterface,"cn.edu.WHU.CDTStudio.CDTExtractionInterface/1.0")
#endif // CDTEXTRACTIONINTERFACE_H
