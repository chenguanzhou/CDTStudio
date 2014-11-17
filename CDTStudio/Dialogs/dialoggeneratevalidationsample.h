#ifndef DIALOGGENERATEVALIDATIONSAMPLE_H
#define DIALOGGENERATEVALIDATIONSAMPLE_H

#include <QDialog>
#include <QValidator>

namespace Ui {
class DialogGenerateValidationSample;
}
class QSqlQueryModel;

class DialogGenerateValidationSample : public QDialog
{
    Q_OBJECT

public:

private:
    explicit DialogGenerateValidationSample(QString projectID,QWidget *parent = 0);
    ~DialogGenerateValidationSample();

    void updateCombobox();
    QStringList getValidationNames() const;

    Ui::DialogGenerateValidationSample *ui;
    QSqlQueryModel *model;
    QString prjID;
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
            return Invalid;
        else
            return Acceptable;
    }
private:
    QStringList blackLst;
};

#endif // DIALOGGENERATEVALIDATIONSAMPLE_H
