#include "cdtcategorydockwidget.h"
#include "stable.h"
#include "mainwindow.h"
#include "cdtimagelayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"

CDTCategoryDockWidget::CDTCategoryDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    tableView(new QTableView(this)),
    //    categoryModel(new QSqlRelationalTableModel(this,QSqlDatabase::database("category"))),
    categoryModel(NULL),
    delegateColor(new CDTCategoryDelegate(this)),
    actionEdit(new QAction(QIcon(":/Icons/Edit.png"),tr("Edit"),this)),
    actionRevert(new QAction(QIcon(":/Icons/Undo.png"),tr("Revert"),this)),
    actionSubmit(new QAction(QIcon(":/Icons/Ok.png"),tr("Submit"),this)),
    actionInsert(new QAction(QIcon(":/Icons/Add.png"),tr("Insert"),this)),
    actionRemove(new QAction(QIcon(":/Icons/Remove.png"),tr("Remove"),this)),
    actionRemove_All(new QAction(QIcon(":/Icons/Remove.png"),tr("Remove All"),this)),
    actionImportCategoroies(new QAction(QIcon(":/Icons/Import.png"),tr("Import Categories"),this)),
    actionExportCategoroies(new QAction(QIcon(":/Icons/Export.png"),tr("Export Categories"),this))
{
    //layout
    this->setEnabled(false);
    QWidget *panel = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(panel);
    QToolBar* toolBar = new QToolBar(this);
    toolBar->setIconSize(MainWindow::getIconSize());
    vbox->setSpacing(1);
    vbox->addWidget(toolBar);
    vbox->addWidget(tableView);
    this->setWidget(panel);
    this->setWindowTitle(tr("Categories"));


    //tableView
    tableView->setEditTriggers(QTableView::DoubleClicked|QTableView::AnyKeyPressed);
    tableView->setSelectionBehavior(QTableView::SelectRows);
    tableView->setSelectionMode(QTableView::SingleSelection);

    //toolbar
    toolBar->addActions(QList<QAction*>()
                        <<actionEdit<<actionRevert<<actionSubmit);
    toolBar->addSeparator();
    toolBar->addActions(QList<QAction*>()
                        <<actionInsert<<actionRemove<<actionRemove_All);
    toolBar->addSeparator();
    toolBar->addActions(QList<QAction*>()
                        <<actionImportCategoroies<<actionExportCategoroies);

    connect(actionInsert,SIGNAL(triggered()),SLOT(on_actionInsert_triggered()));
    connect(actionRemove,SIGNAL(triggered()),SLOT(on_actionRemove_triggered()));
    connect(actionRemove_All,SIGNAL(triggered()),SLOT(on_actionRemove_All_triggered()));
    connect(actionRevert,SIGNAL(triggered()),SLOT(on_actionRevert_triggered()));
    connect(actionSubmit,SIGNAL(triggered()),SLOT(on_actionSubmit_triggered()));
    connect(actionImportCategoroies,SIGNAL(triggered()),SLOT(importCategories()));
    connect(actionExportCategoroies,SIGNAL(triggered()),SLOT(exportCategories()));

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    logger()->info("Constructed");
}

void CDTCategoryDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    if (layer == NULL)
    {
        this->setEnabled(false);
        return;
    }

    onDockClear();
    CDTImageLayer *imgLayer = qobject_cast<CDTImageLayer*>(layer->getAncestor("CDTImageLayer"));
    if (imgLayer)
    {
        logger()->info("Find ancestor class of CDTImageLayer");
        if (imgLayer->id()==imageLayerID)
            return;
        this->updateImageID(imgLayer->id());
        this->setEnabled(true);
        this->setVisible(true);
        this->raise();
//        this->adjustSize();
    }
}

void CDTCategoryDockWidget::onDockClear()
{
//    CDTImageLayer* layer = CDTImageLayer::getLayer(imageLayerID);
//    if (layer) disconnect(categoryModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),
//                          layer,SIGNAL(layerChanged()));
//    if (layer) disconnect(categoryModel,SIGNAL(beforeInsert(QSqlRecord&)),
//                          layer,SIGNAL(layerChanged()));
//    if (layer) disconnect(categoryModel,SIGNAL(beforeDelete(int)),
//                          layer,SIGNAL(layerChanged()));
    if (categoryModel)
    {
        delete categoryModel;
        categoryModel = NULL;
    }
    tableView->setModel(new QSqlTableModel(this));
    imageLayerID = QUuid();
    this->setEnabled(false);
    this->setVisible(false);
}

void CDTCategoryDockWidget::updateImageID(QUuid id)
{
    imageLayerID = id;

    if (categoryModel == NULL)
    {
        QAbstractItemModel *model = tableView->model();
        if (model) delete model;
        categoryModel = new QSqlTableModel(this,QSqlDatabase::database("category"));
        categoryModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Category Name"));
        categoryModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Color"));
//        connect(categoryModel,SIGNAL(primeInsert(int,QSqlRecord&)),SLOT(onPrimeInsert(int,QSqlRecord&)));
        tableView->setModel(categoryModel);
    }
    categoryModel->setTable("category");
    categoryModel->setFilter("imageID='"+imageLayerID.toString()+"'");
    categoryModel->select();

    tableView->setItemDelegateForColumn(2,delegateColor);
    tableView->hideColumn(0);
    tableView->hideColumn(3);
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();

    CDTImageLayer *imgLayer = CDTImageLayer::getLayer(id);
    connect(categoryModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            imgLayer,SIGNAL(layerChanged()));
    connect(categoryModel,SIGNAL(beforeInsert(QSqlRecord&)),
            imgLayer,SIGNAL(layerChanged()));
    connect(categoryModel,SIGNAL(beforeDelete(int)),
            imgLayer,SIGNAL(layerChanged()));

}

void CDTCategoryDockWidget::on_actionInsert_triggered()
{
    static int indexOffset = 0;
    QSqlRecord record= categoryModel->record();
    record.setValue(0,QUuid::createUuid().toString());
    record.setValue(1,tr("New Class")+QString::number(indexOffset++));
    record.setValue(2,QColor(qrand()%255,qrand()%255,qrand()%255));
    record.setValue(3,imageLayerID.toString());
    categoryModel->insertRecord(categoryModel->rowCount(),record);
}

void CDTCategoryDockWidget::on_actionRemove_triggered()
{
    QModelIndexList list = tableView->selectionModel()->selectedRows();
    foreach (QModelIndex index, list) {
        categoryModel->removeRow(index.row());
    }

    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
}

void CDTCategoryDockWidget::on_actionRemove_All_triggered()
{
    categoryModel->removeRows(0,categoryModel->rowCount());
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
}

void CDTCategoryDockWidget::on_actionRevert_triggered()
{
    categoryModel->revertAll();
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
}

void CDTCategoryDockWidget::on_actionSubmit_triggered()
{
    if (categoryModel->submitAll()==false)
    {
        QMessageBox::critical(this,tr("Submit failed!"),tr("Error message: %1").arg(categoryModel->lastError().text()));
        qDebug()<<"Submit failed:"<<categoryModel->lastError();
        categoryModel->revertAll();
    }
//    actionInsert->setEnabled(true);
    tableView->resizeColumnsToContents();
    tableView->resizeRowsToContents();
}

void CDTCategoryDockWidget::importCategories()
{
    if (categoryModel==NULL)
        return;

    QString path = QFileDialog::getOpenFileName(this,tr("Import"),QString(),tr("XML file(*.xml)"));
    if (path.isEmpty())
        return;

    try
    {
        QDomDocument doc;
        QFile file(path);
        if (file.open(QFile::ReadOnly)==false)
            throw tr("Open file %1 failed!");
        if (doc.setContent(&file)==false)
            throw tr("Read file %1 failed!");
        QDomElement root = doc.documentElement();
        if (root.isNull() || root.tagName() != QString("Categories"))
            throw tr("XML file error!");

        QDomElement ele = root.firstChildElement("Category");
        QStringList errorNames;

        while (!ele.isNull())
        {
            QString name = ele.attribute("name");
            QColor color(qrand()%255,qrand()%255,qrand()%255);
            QStringList clrText = ele.attribute("color").split(",");
            if (clrText.size()==3){
                color = QColor(clrText[0].toInt(),clrText[1].toInt(),clrText[2].toInt());
            }

            QSqlRecord record= categoryModel->record();
            record.setValue(0,QUuid::createUuid().toString());
            record.setValue(1,name);
            record.setValue(2,color);
            record.setValue(3,imageLayerID.toString());


            if (categoryModel->insertRecord(categoryModel->rowCount(),record))
            {
                if (categoryModel->submitAll()==false)
                    categoryModel->revertAll();
                else
                    errorNames<<name;
            }
            else
                errorNames<<name;

            ele = ele.nextSiblingElement("Category");
        }

        if (errorNames.isEmpty())
            QMessageBox::information(this,tr("Completed!"),tr("Categories imported succeed!"));
        else
            QMessageBox::warning(this,tr("Warning"),tr("Following categories inserted failed:\n")+errorNames.join("\n"));

        categoryModel->select();

    }
    catch (QString msg)
    {
        QMessageBox::critical(this,tr("Error"),msg);
        logger()->error(msg);
    }
}

void CDTCategoryDockWidget::exportCategories()
{
    if (categoryModel==NULL || categoryModel->rowCount()==0)
        return;
    QString path = QFileDialog::getSaveFileName(this,tr("Export"),QString(),tr("XML file(*.xml)"));
    if (path.isEmpty())
        return;

    QMessageBox::StandardButton ret =
            QMessageBox::information(this,tr("Color information"),tr("Export categories with color information?"),QMessageBox::Ok|QMessageBox::Cancel);

    QDomDocument doc;
    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);

    QDomElement root = doc.createElement("Categories");
    root.setAttribute("count",categoryModel->rowCount());
    doc.appendChild(root);

    for (int i=0;i<categoryModel->rowCount();++i)
    {
        QString name = categoryModel->data(categoryModel->index(i,1)).toString();
        QColor color = categoryModel->data(categoryModel->index(i,2)).value<QColor>();
        qDebug()<<name;

        QDomElement category = doc.createElement("Category");
        category.setAttribute("name",name);
        if (ret == QMessageBox::Ok)
        {
            category.setAttribute("color",QString("%1,%2,%3").arg(color.red()).arg(color.green()).arg(color.blue()));
        }

        root.appendChild(category);
    }

    QFile file(path);
    file.open(QFile::WriteOnly);
    QTextStream out(&file);
    doc.save(out,4);
    file.close();

    QMessageBox::information(this,tr("Completed"),tr("Export categories completely!"));
}

//void CDTCategoryDockWidget::onPrimeInsert(int , QSqlRecord &record)
//{
//        static int indexOffset = 0;
//        record.setValue(0,QUuid::createUuid().toString());
//        record.setValue(1,tr("New Class")+QString::number(indexOffset++));
//        record.setValue(2,QColor(qrand()%255,qrand()%255,qrand()%255));
//        record.setValue(3,imageLayerID.toString());
//        record.setGenerated(0,true);
//        record.setGenerated(1,true);
//        record.setGenerated(2,true);
//        record.setGenerated(3,true);
//}

//void CDTCategoryDockWidget::on_actionEdit_triggered(bool checked)
//{
//    actionInsert->setEnabled(checked);
//    actionRemove->setEnabled(checked);
//    actionRemove_All->setEnabled(checked);
//    actionRevert->setEnabled(checked);
//    actionSubmit->setEnabled(checked);
//    if (checked)
//        tableView->setEditTriggers(QTableView::DoubleClicked|QTableView::AnyKeyPressed);
//    else
//        tableView->setEditTriggers(QTableView::NoEditTriggers);
//}

