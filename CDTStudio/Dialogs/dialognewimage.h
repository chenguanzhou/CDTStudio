#ifndef DIALOGNEWIMAGE_H
#define DIALOGNEWIMAGE_H

#include <QDialog>

namespace Ui {
class DialogNewImage;
}

class DialogNewImage : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewImage(QUuid prjID,QWidget *parent = 0);
    ~DialogNewImage();

    QString imageName() const;
    QString imagePath() const;

private slots:
    void on_pushButton_clicked();
    void on_comboBox_currentIndexChanged(const QString &arg1);

signals:
    void imageNameChanged();

private:
    Ui::DialogNewImage *ui;
    void loadHistoryPaths();
    void saveHistoryPaths();
};

#endif // DIALOGNEWIMAGE_H
