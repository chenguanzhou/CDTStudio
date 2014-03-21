#include "cdtprojectview.h"

CDTProjectView::CDTProjectView(QWidget *parent) :
    QWidget(parent),
    projectModel(NULL),
    attributeWidget(new CDTAttributesWidget(this)),
    treeView(new QTreeView(this))
{    
    treeView->expandAll();
    treeView->resizeColumnToContents(0);
}

CDTProjectModel *CDTProjectView::model()
{
    return    projectModel;
}

QTreeView *CDTProjectView::projectTreeView()
{
    return treeView;
}

CDTAttributesWidget *CDTProjectView::attributeView()
{
    return attributeWidget;
}

void CDTProjectView::setModel(CDTProjectModel *model)
{
    if (projectModel != model)
    {
        projectModel = model;
        treeView->setModel(projectModel->treeModel());
        emit modelChanged(model);
    }
}

void CDTProjectView::setTreeView(QTreeView *tree)
{
    treeView = tree;
}
