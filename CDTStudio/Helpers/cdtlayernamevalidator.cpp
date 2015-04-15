#include "cdtlayernamevalidator.h"
#include "stable.h"

CDTLayerNameValidator::CDTLayerNameValidator(QSqlDatabase db, QString fieldName, QString tableName, QString filter, QObject *parent)
    :QValidator(parent)
{
    QSqlQuery query(db);
    if (filter.isEmpty())
        query.exec(QString("select %1 from %2").arg(fieldName).arg(tableName));
    else
        query.exec(QString("select %1 from %2 where %3").arg(fieldName).arg(tableName).arg(filter));
    qDebug()<<QString("select %1 from %2 where %3").arg(fieldName).arg(tableName).arg(filter);
    while(query.next())
    {
        blackList<<query.value(0).toString();
    }
}

CDTLayerNameValidator::~CDTLayerNameValidator()
{

}

QValidator::State CDTLayerNameValidator::validate(QString &text, int &pos) const
{
    Q_UNUSED(pos);
    if (text.isEmpty())
        return QValidator::Invalid;
    if (blackList.contains(text))
    {
        text += "_new";
        return QValidator::Intermediate;
    }
    else
        return QValidator::Acceptable;
}

