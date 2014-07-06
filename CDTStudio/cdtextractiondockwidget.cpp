#include "cdtextractiondockwidget.h"
#include "ui_cdtextractiondockwidget.h"
#include "cdtextractioninterface.h"
#include "cdtextractionlayer.h"
#include "stable.h"

extern QList<CDTExtractionInterface *> extractionPlugins;

CDTExtractionDockWidget::CDTExtractionDockWidget(QWidget *parent) :
    QDockWidget(parent),
    modelExtractions(new QSqlQueryModel(this)),
    actionStartEdit (new QAction(tr("Start Edit"),this)),
    actionRollBack  (new QAction(tr("Roll Back"),this)),
    actionSave      (new QAction(tr("Save"),this)),
    actionStop      (new QAction(tr("Stop"),this)),
    currentEditState    (LOCKED),
    isGeometryModified  (false),
    vectorLayer (NULL),
    mapCanvas   (NULL),
    lastMapTool (NULL),
    ui(new Ui::CDTExtractionDockWidget)
{
    ui->setupUi(this);

    ui->comboBoxExtraction->setModel(modelExtractions);

    connect(ui->comboBoxMethod, SIGNAL(currentIndexChanged(int)),
            SLOT(updateDescription(int)));
    connect(actionStartEdit,    SIGNAL(triggered()),
            SLOT(onActionStartEdit()));
    connect(actionRollBack,     SIGNAL(triggered()),
            SLOT(onActionRollBack()));
    connect(actionSave,         SIGNAL(triggered()),
            SLOT(onActionSave()));
    connect(actionStop,SIGNAL(triggered()),
            SLOT(onActionStop()));

    ui->toolButtonStart->   setDefaultAction(actionStartEdit);
    ui->toolButtonRollback->setDefaultAction(actionRollBack);
    ui->toolButtonSave->    setDefaultAction(actionSave);
    ui->toolButtonStop->    setDefaultAction(actionStop);

    foreach (CDTExtractionInterface* plugin, extractionPlugins) {
        description.insert(plugin->methodName(),plugin->description());
        ui->comboBoxMethod->addItem(plugin->methodName());
    }

    setEditState(LOCKED);
}

CDTExtractionDockWidget::~CDTExtractionDockWidget()
{
    delete ui;
}

CDTExtractionDockWidget::EDITSTATE CDTExtractionDockWidget::editState() const
{
    return currentEditState;
}

void CDTExtractionDockWidget::setExtractionLayer(QString id)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("select name,imageID from extractionlayer where id = ?");
    query.addBindValue(id);
    query.exec();

    if (query.next()==false)
        return;

    QString name = query.value(0).toString();
    QString imageID = query.value(1).toString();

    query.prepare("select path from imagelayer where id = ?");
    query.addBindValue(imageID);
    query.exec();
    if (query.next()==false)
        return;
    currentImagePath = query.value(0).toString();


    modelExtractions->setQuery(QString("select name,id from extractionlayer where imageID = '%1'" )
                               .arg(imageID),QSqlDatabase::database("category"));

    int index = ui->comboBoxExtraction->findText(name);
    if (index <0)
        return;
    ui->comboBoxExtraction->setCurrentIndex(index);
    currentExtractionID = id;
    CDTExtractionLayer *vecLayer = CDTExtractionLayer::getLayer(id);
    vectorLayer = (QgsVectorLayer*)(vecLayer->canvasLayer());
    mapCanvas   = vecLayer->canvas();
}

void CDTExtractionDockWidget::updateDescription(int currentIndex)
{
    ui->textBrowser->setText(description.value(ui->comboBoxMethod->currentText()));
}

void CDTExtractionDockWidget::setEditState(CDTExtractionDockWidget::EDITSTATE state)
{
    currentEditState = state;

    bool isLocked = currentEditState!=EDITING;
    actionStartEdit->setEnabled(isLocked);
    actionRollBack->setEnabled(!isLocked);
    actionSave->setEnabled(!isLocked);
    actionStop->setEnabled(!isLocked);
}

void CDTExtractionDockWidget::setGeometryModified(bool modified)
{
    isGeometryModified = modified;

    actionRollBack->setEnabled(modified);
    actionSave->setEnabled(modified);
}

void CDTExtractionDockWidget::onActionStartEdit()
{
    if (currentExtractionID.isNull() || ui->comboBoxMethod->count()==0)
        return;
    start();
}

void CDTExtractionDockWidget::onActionRollBack()
{
    if (isGeometryModified)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(NULL, tr("Extraction"),
                                   tr("The extraction layer has been modified.\n"
                                      "Do you want to roolback your changes without save?"),
                                   QMessageBox::Reset | QMessageBox::Cancel);
        if (ret == QMessageBox::Reset)
        {
            ///RollBack changes......
            ///
            rollback();
        }
    }
}

void CDTExtractionDockWidget::onActionSave()
{
    save();
}

void CDTExtractionDockWidget::onActionStop()
{
    if (isGeometryModified)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(NULL, tr("Extraction"),
                                   tr("The extraction layer has been modified.\n"
                                      "Do you want to save your changes or ignore changes before stopping editing?"),
                                   QMessageBox::Save | QMessageBox::Ignore| QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
        {
            save();
            stop();
        }
        else if (ret == QMessageBox::Ignore)
        {
            stop();
        }
        else
            return;
    }
    else stop();
}


void CDTExtractionDockWidget::start()
{
    if (currentEditState == EDITING)
        return;
    lastMapTool = mapCanvas->mapTool();
    currentMapTool = extractionPlugins[ui->comboBoxMethod->currentIndex()]->mapTool(mapCanvas,currentImagePath,vectorLayer);
    mapCanvas->setMapTool(currentMapTool);

    setEditState(EDITING);
    setGeometryModified(false);

    ///start
    ///
    qDebug()<<"start";
}

void CDTExtractionDockWidget::rollback()
{
    if (currentEditState == LOCKED)
        return;
    if (isGeometryModified == false)
        return;
    ///rollback
    ///
    setGeometryModified(false);
    qDebug()<<"rollback";
}

void CDTExtractionDockWidget::save()
{
    if (currentEditState == LOCKED)
        return;
    ///save
    ///
    setGeometryModified(false);
    qDebug()<<"save";
}

void CDTExtractionDockWidget::stop()
{
    if (currentEditState == LOCKED)
        return;
    /// stop
    ///

    mapCanvas->setMapTool(lastMapTool);
    if (currentMapTool)
        delete currentMapTool;

    setEditState(LOCKED);
    qDebug()<<"stop";
}
