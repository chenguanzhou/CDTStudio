#include "cdtextractiondockwidget.h"
#include "ui_cdtextractiondockwidget.h"
#include <qgsvectordataprovider.h>
#include "stable.h"
#include "mainwindow.h"
#include "cdtextractioninterface.h"
#include "cdtextractionlayer.h"
#include "cdtimagelayer.h"
#include "cdtundowidget.h"

extern QList<CDTExtractionInterface *> extractionPlugins;



CDTExtractionDockWidget::CDTExtractionDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    ui(new Ui::CDTExtractionDockWidget),
    modelExtractions    (new QSqlQueryModel(reinterpret_cast<QObject *>(this) )),
    isGeometryModified  (false),
    currentEditState    (LOCKED),
    vectorLayer (Q_NULLPTR),
    mapCanvas   (Q_NULLPTR),
    lastMapTool (Q_NULLPTR),
    actionStartEdit     (new QAction(QIcon(":/Icons/Start.png"), tr("Start Edit"), reinterpret_cast<QObject *>(this))),
    actionRollBack      (new QAction(QIcon(":/Icons/Undo.png"), tr("Rollback"), reinterpret_cast<QObject *>(this))),
    actionSave          (new QAction(QIcon(":/Icons/Save.png"), tr("Save"), reinterpret_cast<QObject *>(this))),
    actionStop          (new QAction(QIcon(":/Icons/Stop.png"), tr("Stop"), reinterpret_cast<QObject *>(this)))
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

    ui->toolButtonStart->setIconSize(MainWindow::getIconSize());
    ui->toolButtonRollback->setIconSize(MainWindow::getIconSize());
    ui->toolButtonSave->setIconSize(MainWindow::getIconSize());
    ui->toolButtonStop->setIconSize(MainWindow::getIconSize());

    foreach (CDTExtractionInterface* plugin, extractionPlugins) {
        description.insert(plugin->methodName(),plugin->description());
        ui->comboBoxMethod->addItem(plugin->methodName());
    }

    setEditState(LOCKED);
    qDebug("Constructed");
}

CDTExtractionDockWidget::~CDTExtractionDockWidget()
{
    delete ui;
}

CDTExtractionDockWidget::EDITSTATE CDTExtractionDockWidget::editState() const
{
    return currentEditState;
}

void CDTExtractionDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    if (layer == Q_NULLPTR)
    {
        this->setEnabled(false);
        return;
    }

    onDockClear();
    CDTExtractionLayer *extLayer = qobject_cast<CDTExtractionLayer *>(layer->getAncestor("CDTExtractionLayer"));
    if (extLayer)
    {
        qDebug("Find ancestor class of CDTExtractionLayer");
        setExtractionLayer(layer->id().toString());
        this->setEnabled(true);
        this->setVisible(true);
        this->raise();
    }
    else
        qDebug("Ancestor class of CDTExtractionLayer is not found");
}

void CDTExtractionDockWidget::onDockClear()
{
    modelExtractions->clear();
    vectorLayer = Q_NULLPTR;
    mapCanvas   = Q_NULLPTR;
    lastMapTool = Q_NULLPTR;
    currentMapTool = Q_NULLPTR;
    currentImagePath.clear();
    currentExtractionID.clear();
    this->setEnabled(false);
    this->setVisible(false);
}

void CDTExtractionDockWidget::updateDescription(int )
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

void CDTExtractionDockWidget::onFeatureChanged()
{
    setGeometryModified(true);
}

void CDTExtractionDockWidget::onActionStartEdit()
{
    if (currentExtractionID.isNull() || ui->comboBoxMethod->count()==0)
        return;
    start();
    ui->comboBoxExtraction->setEnabled(false);
    ui->comboBoxMethod->setEnabled(false);
}

void CDTExtractionDockWidget::onActionRollBack()
{
    if (isGeometryModified)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(Q_NULLPTR, tr("Extraction"),
                                   tr("The extraction layer has been modified.\n"
                                      "Do you want to roolback your changes without save?"),
                                   QMessageBox::Reset | QMessageBox::Cancel);
        if (ret == QMessageBox::Reset)
        {
            ///RollBack changes......
            ///
            rollback();
            vectorLayer->startEditing();
        }
    }
}

void CDTExtractionDockWidget::onActionSave()
{
    save();
    vectorLayer->startEditing();
}

void CDTExtractionDockWidget::onActionStop()
{
    if (isGeometryModified)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(Q_NULLPTR, tr("Extraction"),
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
            rollback();
            stop();
        }
        else
            return;
    }
    else stop();
    ui->comboBoxExtraction->setEnabled(true);
    ui->comboBoxMethod->setEnabled(true);
}


void CDTExtractionDockWidget::start()
{
    if (currentEditState == EDITING)
        return;
    lastMapTool = mapCanvas->mapTool();
    currentMapTool = extractionPlugins[ui->comboBoxMethod->currentIndex()]->mapTool(mapCanvas,currentImagePath,vectorLayer);

    mapCanvas->setMapTool(currentMapTool);
    mapCanvas->refresh();

    lastCursor = mapCanvas->cursor();
    mapCanvas->setCursor(QCursor(Qt::CrossCursor));

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
    vectorLayer->rollBack();

    setGeometryModified(false);
    qDebug()<<"rollback";
}

void CDTExtractionDockWidget::save()
{
    if (currentEditState == LOCKED)
        return;
    ///save
    ///
    vectorLayer->commitChanges();
    setGeometryModified(false);
    qDebug()<<"save";
}

void CDTExtractionDockWidget::stop()
{
    if (currentEditState == LOCKED)
        return;
    /// stop
    ///
    vectorLayer->commitChanges();
    mapCanvas->setMapTool(lastMapTool);
    if (currentMapTool)
        delete currentMapTool;
    mapCanvas->setCursor(lastCursor);

    setEditState(LOCKED);
    qDebug()<<"stop";
}

void CDTExtractionDockWidget::setExtractionLayer(QString id)
{
    if (id == currentExtractionID)
        return;

    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("select name,imageID from extractionlayer where id = ?");
    query.addBindValue(id);
    query.exec();

    if (query.next()==false)
        return;

    QString name = query.value(0).toString();
    QString imageID = query.value(1).toString();
    currentImagePath = CDTImageLayer::getLayer(imageID)->absolutPath();


    modelExtractions->setQuery(QString("select name,id from extractionlayer where imageID = '%1'" )
                               .arg(imageID),QSqlDatabase::database("category"));

    int index = ui->comboBoxExtraction->findText(name);
    if (index <0)
        return;
    ui->comboBoxExtraction->setCurrentIndex(index);
    currentExtractionID = id;
    CDTExtractionLayer *extractionLayer = CDTExtractionLayer::getLayer(id);

    if (vectorLayer)
        disconnect(vectorLayer,SIGNAL(featureAdded(QgsFeatureId)),this,SLOT(onFeatureChanged()));

    vectorLayer = (QgsVectorLayer*)(extractionLayer->canvasLayer());
    connect(vectorLayer,SIGNAL(featureAdded(QgsFeatureId)),SLOT(onFeatureChanged()));
    mapCanvas   = extractionLayer->canvas();

    CDTUndoWidget *undoWidget = MainWindow::getUndoWidget();
    undoWidget->setCurrentLayer(extractionLayer);
}
