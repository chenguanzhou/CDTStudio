#ifndef POLYGONIZER_H
#define POLYGONIZER_H

#include "cdtbasethread.h"

class Polygonizer : public CDTBaseThread
{
    Q_OBJECT
public:
    explicit Polygonizer(QString flagPath,
                         QString shpPath,
                         QObject *parent = 0);

    void run();

signals:

public slots:

private:
    QString _flagPath;
    QString _shapefilePath;
};


#endif // POLYGONIZER_H
