#ifndef DIALOGDBCONNECTION_H
#define DIALOGDBCONNECTION_H

#include <QDialog>
#include <QMetaType>

namespace Ui {
class DialogDBConnection;
}

class CDTDatabaseConnInfo
{
public:
    QString dbType;
    QString dbName;
    QString username;
    QString password;
    QString hostName;
    int port;

    bool operator==(const CDTDatabaseConnInfo& rhs) const;
    bool isNull();
};

Q_DECLARE_METATYPE(CDTDatabaseConnInfo)

QDataStream &operator<<(QDataStream &out, const CDTDatabaseConnInfo &dbInfo);
QDataStream &operator>>(QDataStream &in, CDTDatabaseConnInfo &dbInfo);



class DialogDBConnection : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDBConnection(CDTDatabaseConnInfo dbInfo = CDTDatabaseConnInfo(),QWidget *parent = 0);
    ~DialogDBConnection();

    CDTDatabaseConnInfo dbConnectInfo();

private slots:
    void on_toolButton_clicked();
    void on_comboDriver_currentIndexChanged(const QString &arg1);
    void on_pushButtonTest_clicked();

private:
    void initialize();
    void updateConnInfo();

private:
    Ui::DialogDBConnection *ui;
    CDTDatabaseConnInfo dbConnInfo;
};

#endif // DIALOGDBCONNECTION_H
