#ifndef DIALOGNEWSEGMENTATION_H
#define DIALOGNEWSEGMENTATION_H

#include <QDialog>
#include <QMap>
#include "cdtsegmentationInterface.h"
#include "cdtpluginloader.h"

namespace Ui {
class DialogNewSegmentation;
}
class CDTFileSystem;

class DialogNewSegmentation : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewSegmentation(const QString &inputImage, CDTFileSystem* fileSys ,QWidget *parent = 0);
    ~DialogNewSegmentation();

    QString name() const;
    QString markfilePath() const;
    QString shapefilePath() const;
    QColor  borderColor() const;
    QString method() const;
    QVariantMap params() const;

private slots:
    void on_comboBox_currentIndexChanged(int index);
//    void on_pushButtonMarkfile_clicked();
//    void on_pushButtonShapefile_clicked();
//    void on_comboBoxMarkfile_currentIndexChanged(const QString &arg1);
//    void on_comboBoxShapefile_currentIndexChanged(const QString &arg1);
    void onFinished();
    void on_pushButtonStart_clicked();

private:
    Ui::DialogNewSegmentation *ui;
    QVariantMap segmentationParams;
    QString inputImagePath;
    CDTFileSystem *fileSystem;

    QString markfileTempPath;
    QString shapefileTempPath;
    QString markfileID;
    QString shapefileID;

    void loadPlugins();
//    void saveHistoryPaths();
//    void loadHistoryPaths();

};




#endif // DIALOGNEWSEGMENTATION_H
