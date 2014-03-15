#ifndef CDTSEGMENTATIONINTERFACE_H
#define CDTSEGMENTATIONINTERFACE_H

#include <QtCore>
#include <QtPlugin>

class CDTSegmentationInterface:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString inputImagePath READ inputImagePath WRITE setInputImagePath)
    Q_PROPERTY(QString markfilePath READ markfilePath WRITE setMarkfilePath NOTIFY onMarkfilePathChanged)
    Q_PROPERTY(QString shapefilePath READ shapefilePath WRITE setShapefilePath NOTIFY onShapefilePathChanged)
public:
    explicit CDTSegmentationInterface(QObject* parent = 0):QObject(parent){}

    virtual QString segmentationMethod()const =0;
    virtual QWidget* paramsForm() =0;

    void setInputImagePath(const QString &path){_inputImagePath=path;}
    void setMarkfilePath  (const QString &path){_markfilePath=path;}
    void setShapefilePath (const QString &path){_shapefilePath=path;}

    QString inputImagePath()const{return _inputImagePath;}
    QString markfilePath()  const{return _markfilePath;}
    QString shapefilePath() const{return _shapefilePath;}

signals:
    void finished(QMap<QString, QVariant>);
    void onMarkfilePathChanged(const QString& path);
    void onShapefilePathChanged(const QString& path);

private:
    QString _inputImagePath;
    QString _markfilePath;
    QString _shapefilePath;
};

Q_DECLARE_INTERFACE(CDTSegmentationInterface,"cn.edu.WHU.CDTStudio.CDTSegmentationInterface/1.0")

#endif // CDTSEGMENTATIONINTERFACE_H
