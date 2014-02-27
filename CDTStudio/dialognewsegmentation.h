#ifndef DIALOGNEWSEGMENTATION_H
#define DIALOGNEWSEGMENTATION_H

#include <QDialog>
#include <QGridLayout>
#include <QMap>

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
    void on_comboBox_currentIndexChanged(const QString &arg1);
    void on_pushButtonStart_clicked();
    void on_pushButtonBrowseMarkfilePath_clicked();
    void on_pushButtonBrowseshapefilePath_clicked();
    void onLineEditChanged(int);
    void onSegmentationFinished();
    void onWarningMessage(QString msg);

    void on_lineEditName_textChanged(const QString &arg1);
    void on_lineEditMarkfile_textChanged(const QString &arg1);
    void on_lineEditShapefile_textChanged(const QString &arg1);

private:
    Ui::DialogNewSegmentation *ui;
    QMap<QString,QVariant> segmentationParams;
    QGridLayout* gridLatoutParams;
    QString inputImagePath;

    void initSegmentationMethod();
};

#endif // DIALOGNEWSEGMENTATION_H
