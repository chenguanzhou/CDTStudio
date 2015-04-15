#ifndef DIALOGNEWSEGMENTATION_H
#define DIALOGNEWSEGMENTATION_H

#include <QDialog>
#include <QMap>
#include <log4qt/logger.h>

namespace Ui {
class DialogNewSegmentation;
}
class CDTFileSystem;
struct QUuid;

class DialogNewSegmentation : public QDialog
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit DialogNewSegmentation(
            QUuid imageID,
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

private slots:
    void setSegMethod(int index);
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
    bool isFinished;

    void showPlugins();
};




#endif // DIALOGNEWSEGMENTATION_H
