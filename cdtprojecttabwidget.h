#ifndef CDTPROJECTTABWIDGET_H
#define CDTPROJECTTABWIDGET_H

#include <QTabWidget>
#include "cdtprojectwidget.h"

class CDTProjectTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    friend class MainWindow;
    explicit CDTProjectTabWidget(QWidget *parent = 0);

signals:

public slots:
    void createNewProject();
    void addProjectTab(const QString& path);//load project file
private:
};

#endif // CDTPROJECTTABWIDGET_H
