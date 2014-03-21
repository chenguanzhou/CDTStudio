#ifndef CDTPROJECTVIEW_H
#define CDTPROJECTVIEW_H

#include <QWidget>
#include <QTreeView>
#include "cdtprojectmodel.h"
#include "cdtattributeswidget.h"

class CDTProjectView : public QWidget
{
    Q_OBJECT
public:
    friend class CDTProjectModel;
    explicit CDTProjectView(QWidget *parent = 0);
    CDTProjectModel *model();
    QTreeView * projectTreeView();
    CDTAttributesWidget* attributeView();
signals:
    void modelChanged(CDTProjectModel* model);
public slots:
    void setModel(CDTProjectModel* model);
    void setTreeView(QTreeView *tree);

private:
    CDTProjectModel* projectModel;
    CDTAttributesWidget* attributeWidget;
    QTreeView * treeView;
};

#endif // CDTPROJECTVIEW_H
