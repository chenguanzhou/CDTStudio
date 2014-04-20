#ifndef FORMATTRIBUTES_H
#define FORMATTRIBUTES_H

#include <QWidget>
#include <QtSql>

namespace Ui {
class CDTAttributesWidget;
}

class QToolBar;
class QMenuBar;
class CDTSegmentationLayer;
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
QDataStream &operator<<(QDataStream &out, const CDTDatabaseConnInfo &dbInfo);
QDataStream &operator>>(QDataStream &in, CDTDatabaseConnInfo &dbInfo);

class CDTAttributesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CDTAttributesWidget(QWidget *parent = 0);
    ~CDTAttributesWidget();

    QMenuBar *menuBar()const;
    CDTDatabaseConnInfo databaseURL() const;
    CDTSegmentationLayer *segmentationLayer()const;

signals:
    void databaseURLChanged(CDTDatabaseConnInfo);

public slots:
    void setDatabaseURL(CDTDatabaseConnInfo url);
    void setSegmentationLayer(CDTSegmentationLayer *layer);
    void updateTable();

private slots:
    void onActionEditDataSourceTriggered();
    void onActionGenerateAttributesTriggered();
    void on_pushButtonApply_clicked();    
    void on_pushButtonReset_clicked();
    void on_comboDriver_currentIndexChanged(const QString &arg1);
    void onDatabaseChanged(CDTDatabaseConnInfo connInfo);
    void onSegmentationDestroyed();

private:
    Ui::CDTAttributesWidget *ui;
    QMenuBar *_menuBar;
    CDTDatabaseConnInfo _dbConnInfo;
    CDTSegmentationLayer* _segmentationLayer;

    CDTDatabaseConnInfo dbConnInfoFromWidgets();
    void updateWidgetsByUrl(const CDTDatabaseConnInfo& dbConnInfo);
    void clearTables();
};

#endif // FORMATTRIBUTES_H
