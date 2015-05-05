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
    bool useRelativePath()const;

private slots:
    void onPathChanged(QString path);
signals:
    void imageNameChanged();

private:
    Ui::DialogNewImage *ui;
//    void loadHistoryPaths();
//    void saveHistoryPaths();
};

#endif // DIALOGNEWIMAGE_H
