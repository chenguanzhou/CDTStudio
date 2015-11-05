#ifndef DIALOGNEWEXTRACTION_H
#define DIALOGNEWEXTRACTION_H

#include <QDialog>

namespace Ui {
class DialogNewExtraction;
}
class CDTFileSystem;
struct QUuid;

class DialogNewExtraction : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewExtraction(
            QUuid imageID,
            const QString &inputImage,
            CDTFileSystem* fileSys ,
            QWidget *parent = 0
    );
    ~DialogNewExtraction();

    QString name()          const;
    QColor  color()         const;
    QColor  borderColor()   const;
    QString fileID()        const;

private slots:
    void onAccepted();

private:
    Ui::DialogNewExtraction *ui;
    QString inputImagePath;
    CDTFileSystem *fileSystem;

    QString shapefileTempPath;
    QString shapefileID;
};

#endif // DIALOGNEWEXTRACTION_H
