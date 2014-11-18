#ifndef DIALOGGENERATEVALIDATIONSAMPLE_H
#define DIALOGGENERATEVALIDATIONSAMPLE_H

#include <QDialog>
#include <QValidator>
#include "log4qt/logger.h"

namespace Ui {
class DialogGenerateValidationSample;
}
class QSqlQueryModel;

class DialogGenerateValidationSample : public QDialog
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
private:
    explicit DialogGenerateValidationSample(QString projectID,QWidget *parent = 0);
    ~DialogGenerateValidationSample();

public:
    //return a stringlist (name,pointset_name)
    static QStringList getGeneratedValidationPointsetName(QString projectID,QWidget *parent);

private slots:
    void onComboBoxChanged(QString pointsetName);

private:
    void updateCombobox();
    QStringList getValidationNames() const;

    Ui::DialogGenerateValidationSample *ui;
    QSqlQueryModel *model;
    QString imgID;
};

class CDTBlackListValidator:public QValidator
{
    Q_OBJECT
public:
    explicit CDTBlackListValidator(QStringList blackList, QObject * parent = 0)
        :QValidator(parent),blackLst(blackList){}

    State validate(QString &input, int &pos) const
    {
        if (blackLst.contains(input))
        {
            return Invalid;
        }
        else
            return Acceptable;
    }
private:
    QStringList blackLst;
};

#endif // DIALOGGENERATEVALIDATIONSAMPLE_H
