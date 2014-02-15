#ifndef CDTPROJECT_H
#define CDTPROJECT_H

#include <QObject>
#include <QVector>


class CDTImageLayer;

class CDTProject
{
public:
    explicit CDTProject();

private:
    QString name;
    QString path;
    bool    isFileExsit;
    QVector<CDTImageLayer *> images;
};

#endif // CDTPROJECT_H
