#ifndef DIALOGNEWSEGMENTATION_H
#define DIALOGNEWSEGMENTATION_H

#include <QDialog>
#include <QGridLayout>
#include <QMap>
#include "cdtsegmentationInterface.h"
#include "cdtpluginloader.h"

namespace Ui {
class DialogNewSegmentation;
}

class DialogNewSegmentation : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewSegmentation(const QString &inputImage, QWidget *parent = 0);
    ~DialogNewSegmentation();

    QString name() const;
    QString markfilePath() const;
    QString shapefilePath() const;
    QString method() const;
    QMap<QString,QVariant> params() const;

private slots:
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButtonMarkfile_clicked();
    void on_pushButtonShapefile_clicked();
    void on_comboBoxMarkfile_currentIndexChanged(const QString &arg1);
    void on_comboBoxShapefile_currentIndexChanged(const QString &arg1);
    void onFinished();
    void on_pushButtonStart_clicked();

private:
    Ui::DialogNewSegmentation *ui;
    QMap<QString,QVariant> segmentationParams;
    QGridLayout* gridLatoutPlugin;
    QString inputImagePath;

    void loadPlugins();
    void saveHistoryPaths();
    void loadHistoryPaths();

};




#endif // DIALOGNEWSEGMENTATION_H
