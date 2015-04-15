#ifndef CDTLAYERNAMEVALIDATOR_H
#define CDTLAYERNAMEVALIDATOR_H

#include <QValidator>
class QSqlDatabase;

class CDTLayerNameValidator : public QValidator
{
    Q_OBJECT
public:
    explicit CDTLayerNameValidator(QSqlDatabase db,QString fieldName,QString tableName,QString filter = QString(),QObject *parent = NULL);
    ~CDTLayerNameValidator();

    State validate(QString &text, int &pos) const;

private:
    QStringList blackList;
};

#endif // CDTLAYERNAMEVALIDATOR_H
