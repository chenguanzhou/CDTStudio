#ifndef CDTATTIBUTESTATISTICSHELPER_H
#define CDTATTIBUTESTATISTICSHELPER_H

#include "cdtbasethread.h"
#include <log4qt/logger.h>
#include <QMap>

class QSqlDatabase;
class CDTAttibuteStatisticsHelper : public CDTBaseThread
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit CDTAttibuteStatisticsHelper(QSqlDatabase db, QString tableName, QString fieldName, QObject *parent = NULL);
    ~CDTAttibuteStatisticsHelper();

    bool isValid()const;
    QMap<QString,QString> values()const;

    void run();

private:
    QSqlDatabase db;
    QString tableName;
    QString fieldName;
    bool valid;
    QMap<QString,QString> data;
};

#endif // CDTATTIBUTESTATISTICSHELPER_H
