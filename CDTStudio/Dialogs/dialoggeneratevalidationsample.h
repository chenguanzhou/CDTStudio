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
    explicit DialogGenerateValidationSample(QString imageID,QWidget *parent = 0);
    ~DialogGenerateValidationSample();

public:
    //return a stringlist (name,pointset_name)
    static QStringList getGeneratedValidationPointsetName(QString imageID,QWidget *parent);

private slots:
    void onComboBoxChanged(QString pointsetName);
    void onAddPointset();

private:
    void updateCombobox();
    QVector<QPointF> generatePoints(int pointsCount, const QgsRectangle &extent);
    bool insertPointsIntoDB(QVector<QPointF> points,QString pointset_name);
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
        Q_UNUSED(pos);
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
