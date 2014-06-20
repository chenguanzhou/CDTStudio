#ifndef CDTAPPLICATION_H
#define CDTAPPLICATION_H

#include "QtGlobal"
#include "qgsapplication.h"

class CDTApplication : public QgsApplication
{
    Q_OBJECT
public:
    explicit CDTApplication(int & argc, char ** argv);
    ~CDTApplication();
signals:

public slots:

private:
    bool initDatabase();
    #ifdef Q_OS_WIN
    void initStxxl();
    #endif
};

#endif // CDTAPPLICATION_H
