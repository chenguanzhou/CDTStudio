#include "cdtattibutestatisticshelper.h"
#include "stable.h"

CDTAttibuteStatisticsHelper::CDTAttibuteStatisticsHelper(QSqlDatabase db, QString tableName, QString fieldName, QObject *parent)
    :CDTBaseThread(parent),valid(false)
{
    this->db = db;
    this->tableName = tableName;
    this->fieldName = fieldName;
}

CDTAttibuteStatisticsHelper::~CDTAttibuteStatisticsHelper()
{

}

bool CDTAttibuteStatisticsHelper::isValid() const
{
    return valid;
}

QMap<QString, QString> CDTAttibuteStatisticsHelper::values() const
{
    return data;
}

void CDTAttibuteStatisticsHelper::run()
{
    if( (!db.isValid()) || (!db.isOpen()) )
        return;

    if (!db.tables().contains(tableName))
        return;

    if (fieldName.isEmpty())
        return;

    QSqlQuery query(db);    
    try{
        bool ret = false;
        ret = query.exec(QString("select min(%1),max(%1),avg(%1) from %2").arg(fieldName).arg(tableName));
        if(ret == false) throw "Excute sql failed!";
        ret = query.next();
        if(ret == false) throw "Excute sql failed!";

        data.insert(tr("Min"),query.value(0).toString());
        data.insert(tr("Max"),query.value(1).toString());
        data.insert(tr("Average"),query.value(0).toString());
    }
    catch (QString msg)
    {
        logger()->error(msg);
    }

}

