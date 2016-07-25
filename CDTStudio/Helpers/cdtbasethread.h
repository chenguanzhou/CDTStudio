#ifndef CDTBASETHREAD_H
#define CDTBASETHREAD_H

#include <QThread>

class CDTBaseThread : public QThread
{
    Q_OBJECT
public:
    explicit CDTBaseThread(QObject *parent = 0):
        QThread(parent)
    {}

signals:
    void currentProgressChanged(QString);
    void progressBarValueChanged(int);//0-100
    void progressBarSizeChanged(int,int);
    void showNormalMessage(QString);
    void showWarningMessage(QString);
    void aborted();
    void completed();
public slots:

};

#endif // CDTBASETHREAD_H
