#include "cdtvalidationsampledockwidget.h"
#include <ctime>
#include "stable.h"
#include "mainwindow.h"
#include "cdtprojectwidget.h"
#include "cdtprojectlayer.h"
#include "cdtimagelayer.h"

CDTValidationSampleDockWidget::CDTValidationSampleDockWidget(QWidget *parent) :
    CDTDockWidget(parent),
    groupBox(new QGroupBox(tr("Edit validation sample sets"),this)),
    toolBar(new QToolBar(this)),
    listView(new QListView(this)),
    sampleModel(new QSqlQueryModel(this)),
    lastMapTool(NULL),
    currentMapTool(NULL)
{
    this->setEnabled(false);
    this->setWindowTitle(tr("Validation sample sets"));

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    this->setWidget(widget);
    layout->addWidget(groupBox);

    QVBoxLayout *groupboxLayout = new QVBoxLayout(groupBox);
    groupboxLayout->addWidget(toolBar);
    groupboxLayout->addWidget(listView);
    groupBox->setLayout(groupboxLayout);
    groupBox->setCheckable(true);
    groupBox->setChecked(false);

    listView->setModel(sampleModel);

    QAction *actionRename = new QAction(QIcon(":/Icon/Rename.png"),tr("Rename"),this);
    QAction *actionAddNew = new QAction(QIcon(":/Icon/Add.png"),tr("Add a sample set"),this);
    QAction *actionRemove = new QAction(QIcon(":/Icon/Remove.png"),tr("Remove"),this);
    toolBar->addActions(QList<QAction*>()<<actionRename<<actionAddNew<<actionRemove);
    toolBar->setIconSize(MainWindow::getIconSize());

    connect(groupBox,SIGNAL(toggled(bool)),SLOT(onGroupBoxToggled(bool)));
    connect(actionAddNew,SIGNAL(triggered()),SLOT(onActionAdd()));
    logger()->info("Constructed");
}

CDTValidationSampleDockWidget::~CDTValidationSampleDockWidget()
{
//    onDockClear();
}

void CDTValidationSampleDockWidget::setCurrentLayer(CDTBaseLayer *layer)
{
    if (layer==NULL)
        return;

    if (layer->id()==imageID)
        return;

    onDockClear();
    CDTImageLayer *imageLayer = qobject_cast<CDTImageLayer *>(layer->getAncestor("CDTImageLayer"));
    if (imageLayer == NULL)
    {
        logger()->info("No CDTImageLayer ancestor!");        
        return;
    }
    else
    {
        this->setEnabled(true);
        imageID = imageLayer->id();
        logger()->info("Find CDTImageLayer ancestor");
    }
}

void CDTValidationSampleDockWidget::onDockClear()
{
    this->setEnabled(false);
//    sampleModel->clear();
    if (currentMapTool)
    {
        delete currentMapTool;
        currentMapTool = NULL;
    }
}

void CDTValidationSampleDockWidget::onGroupBoxToggled(bool toggled)
{
    toolBar->setEnabled(toggled);
    listView->setEnabled(toggled);
    if (toggled)
    {
        lastMapTool = MainWindow::getCurrentMapCanvas()->mapTool();
        currentMapTool =
                new QgsMapToolPan(MainWindow::getCurrentMapCanvas());
//        connect(MainWindow::getCurrentMapCanvas(),SIGNAL(destroyed()),currentMapTool,SLOT(clearRubberBand()));
        MainWindow::getCurrentMapCanvas()->setMapTool(currentMapTool);
        MainWindow::getCurrentProjectWidget()->menuBar()->setEnabled(false);
        //if ().....
    }
    else
    {
        MainWindow::getCurrentMapCanvas()->setMapTool(lastMapTool);
        if(currentMapTool) delete currentMapTool;
        currentMapTool = NULL;
        MainWindow::getCurrentProjectWidget()->menuBar()->setEnabled(true);
    }
}

void CDTValidationSampleDockWidget::onActionAdd()
{
    QUuid id = QUuid::createUuid();
    QString name = QInputDialog::getText(this,tr("New validation sample name"),
                          tr("Name:"),QLineEdit::Normal,tr("New Sample"));
    if (name.isEmpty())
        return;

    QString pointsSetName = QInputDialog::getText(this,tr("Name of new points set"),
                          tr("Name:"),QLineEdit::Normal,tr("New points set"));
    if (pointsSetName.isEmpty())
        return;

    bool ok;
    int pointsCount = QInputDialog::getInt(this,tr("Count of validation points"),tr("Points count"),30,1,10000,1,&ok);
    if (ok==false)
        return;

    if (imageID.isNull())
        return;

    CDTImageLayer *imageLayer = CDTImageLayer::getLayer(imageID);
    if (imageLayer==NULL)
        return;

    QgsRectangle extent = imageLayer->canvasLayer()->extent();
    QVector<QPointF> points = generatePoints(pointsCount,extent);
    if (insertPointsIntoDB(points,pointsSetName)==false)
        return;
    MainWindow::getCurrentProjectWidget()->setWindowModified(true);
}

QVector<QPointF> CDTValidationSampleDockWidget::generatePoints(int pointsCount, const QgsRectangle &extent)
{
    const int PRECISE = 1000000;
    qsrand(std::clock());

    QVector<QPointF> points;
    for (int i=0;i<pointsCount;++i)
    {
        double x = static_cast<double>(qrand()%PRECISE)/PRECISE*extent.width();
        double y = static_cast<double>(qrand()%PRECISE)/PRECISE*extent.height();
        points.push_back(QPointF(x+extent.xMinimum(),y+extent.yMinimum()));
    }

    return points;
}

bool CDTValidationSampleDockWidget::insertPointsIntoDB(QVector<QPointF> points, const QString &pointsSetName)
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);

    CDTProjectLayer *prj = qobject_cast<CDTProjectLayer*>(CDTImageLayer::getLayer(imageID)->parent());

    db.transaction();
    if (query.exec(QString("insert into points_project values('%1','%2')")
                   .arg(pointsSetName).arg(prj->id().toString()))==false)
    {
        logger()->error("Execute SQL of 'insert into points_project' failed!");
        db.rollback();
        return false;
    }

    if (query.prepare("insert into points values(?,?,?,?)")==false)
    {
        logger()->error("Prepare SQL failed!");
        db.rollback();
        return false;
    }

    int i=1;
    foreach (QPointF pt, points) {
        query.bindValue(0,i++);
        query.bindValue(1,pt.x());
        query.bindValue(2,pt.y());
        query.bindValue(3,pointsSetName);
        if (query.exec()==false)
        {
            logger()->error("Insert point:(%1,%2) into DB failed. Reason:%3"
                            ,pt.x(),pt.y(),query.lastError().text());
            db.rollback();
            return false;
        }
    }
    db.commit();
    logger()->info("Insert points set %1 into DB succeeded!",pointsSetName);
    return true;
}
