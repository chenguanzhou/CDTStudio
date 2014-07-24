#ifndef DIALOGNEWSEGMENTATION_H
#define DIALOGNEWSEGMENTATION_H

#include <QDialog>
#include <QMap>
#include "dialogdbconnection.h"

namespace Ui {
class DialogNewSegmentation;
}
class CDTFileSystem;

class DialogNewSegmentation : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewSegmentation(
            const QString &inputImage,
            CDTFileSystem* fileSys ,
            QWidget *parent = 0
    );
    ~DialogNewSegmentation();

    QString name() const;
    QString markfileID() const;
    QString shapefileID() const;
    QColor  borderColor() const;
    QString method() const;
    QVariantMap params() const;
    CDTDatabaseConnInfo databaseConnInfo()const;

private slots:
    void setSegMethod(int index);
    void setDBConnectionInfo();
    void startSegmentation();
    void onSegFinished();
    void updateButtonBoxStatus();

private:
    Ui::DialogNewSegmentation *ui;
    QVariantMap segmentationParams;
    QString inputImagePath;
    CDTFileSystem *fileSystem;

    QString markfileTempPath;
    QString shapefileTempPath;
    QString mkID;
    QString shpID;
    CDTDatabaseConnInfo dbConnInfo;

    bool isFinished;
    bool isDBTested;

    void loadPlugins();
};




#endif // DIALOGNEWSEGMENTATION_H
