#ifndef SNAKEINTERFACE_H
#define SNAKEINTERFACE_H

#include "cdtextractioninterface.h"

class SnakeInterface : public CDTExtractionInterface
{
    Q_OBJECT
public:
    SnakeInterface(QUuid extravtionLayerID,QObject *parent = 0);

    QString methodName()    const;
    QString description()   const;

//public slots:
    void start();
    void rollback();
    void save();
    void stop();
};

#endif // SNAKEINTERFACE_H
