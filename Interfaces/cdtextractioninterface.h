#ifndef CDTEXTRACTIONINTERFACE_H
#define CDTEXTRACTIONINTERFACE_H

#include <QtCore>
#include <qgsmaptool.h>

class CDTExtractionInterface:public QObject
{
    Q_OBJECT
public:
    explicit CDTExtractionInterface(QUuid extravtionLayerID,QObject* parent = 0)
        :layerID(extravtionLayerID),QObject(parent)
    {}

    virtual QString methodName()    const = 0;
    virtual QString description()   const = 0;

public slots:
    virtual void start()    = 0;
    virtual void rollback() = 0;
    virtual void save()     = 0;
    virtual void stop()     = 0;

protected:
    QUuid layerID;
};


#endif // CDTEXTRACTIONINTERFACE_H
