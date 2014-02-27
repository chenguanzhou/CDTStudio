#ifndef CDTSEGMENTATIONINTERFACE_H
#define CDTSEGMENTATIONINTERFACE_H
#include <QThread>

class CDTSegmentationInterface : public QThread
{
    Q_OBJECT
    Q_PROPERTY(QString inputImagePath READ inputImagePath WRITE setInputImagePath)
    Q_PROPERTY(QString markfilePath READ markfilePath WRITE setMarkfilePath)
    Q_PROPERTY(QString shapefilePath READ shapefilePath WRITE setShapefilePath)

public:
    explicit CDTSegmentationInterface(QObject *parent = 0):
        QThread(parent)
    {}

    virtual QString     segmentationMethod()const               = 0;
    virtual QWidget*    paramsInterface  (QWidget* parent)const = 0;

    void setInputImagePath(const QString &path){_inputImagePath=path;}
    void setMarkfilePath  (const QString &path){_markfilePath=path;}
    void setShapefilePath (const QString &path){_shapefilePath=path;}

    QString inputImagePath()const{return _inputImagePath;}
    QString markfilePath()  const{return _markfilePath;}
    QString shapefilePath() const{return _shapefilePath;}

signals:
    void paramsChanged(bool);               //Does Params valid
    void currentProgressChanged(QString);
    void progressBarValueChanged(int);
    void progressBarSizeChanged(int,int);
//    void showNormalMessage(QString);
    void showWarningMessage(QString);

private:
    QString _inputImagePath;
    QString _markfilePath;
    QString _shapefilePath;
};
Q_DECLARE_INTERFACE(CDTSegmentationInterface,"cn.edu.WHU.CDTStudio.CDTSegmentationInterface/1.0")

#endif // CDTSEGMENTATIONINTERFACE_H
