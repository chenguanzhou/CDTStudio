#include "dialoggeneratevalidationsample.h"
#include "ui_dialoggeneratevalidationsample.h"

#include <ctime>
#include "stable.h"
#include "cdtprojectlayer.h"
#include "cdtimagelayer.h"

DialogGenerateValidationSample::DialogGenerateValidationSample(QString imageID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGenerateValidationSample),
    model(new QSqlQueryModel(this)),
    imgID(imageID)
{
    ui->setupUi(this);
    ui->buttonBox->setEnabled(false);

    QStringList validationNames = getValidationNames();
    CDTBlackListValidator *validator = new CDTBlackListValidator(validationNames,this);
    ui->lineEditName->setValidator(validator);

    ui->comboBoxPointset->setModel(model);
    connect(ui->comboBoxPointset,SIGNAL(currentIndexChanged(QString)),SLOT(onComboBoxChanged(QString)));
    connect(ui->toolButtonAddPointset,SIGNAL(clicked()),SLOT(onAddPointset()));
    updateCombobox();
}

DialogGenerateValidationSample::~DialogGenerateValidationSample()
{
    delete ui;
}

QStringList DialogGenerateValidationSample::getGeneratedValidationPointsetName(QString imageID, QWidget *parent)
{
    QStringList info;
    DialogGenerateValidationSample dlg(imageID,parent);
    dlg.adjustSize();
    dlg.exec();
    info << dlg.ui->lineEditName->text();
    info << dlg.ui->comboBoxPointset->currentText();
    return info;
}

void DialogGenerateValidationSample::onComboBoxChanged(QString pointsetName)
{
    try{
        if (pointsetName.isEmpty())
            throw std::invalid_argument("pointsetName is empty");

        QSqlQuery query(QSqlDatabase::database("category"));
        if (query.exec(QString("select count(*) from points_project where pointset_name='%1'")
                       .arg(pointsetName))==false)
                throw std::invalid_argument(QString("Excute sql failed. Error:%1")
                                     .arg(query.lastError().text()).toLocal8Bit().constData());
        query.next();
        int count = query.value(0).toInt();
        if (count<0)
            throw std::logic_error(QString("pointsetName: %1 is cnot found")
                                 .arg(pointsetName).toLocal8Bit().constData());
        qDebug()<<pointsetName;
        if (query.exec(QString("select count(*) from points where pointset_name = '%1'")
                   .arg(pointsetName))==false)
            throw std::invalid_argument(QString("Excute sql failed. Error:%1")
                                 .arg(query.lastError().text()).toLocal8Bit().constData());
        query.next();
        count = query.value(0).toInt();

        ui->labelPointCount->setText(tr("Point count:")+QString::number(count));
        ui->buttonBox->setEnabled(true);
    }
    catch (const std::exception &e)
    {
        logger()->debug(e.what());
        ui->labelPointCount->setText(tr("Pointset is invalid"));
        ui->buttonBox->setEnabled(false);
    }

}

void DialogGenerateValidationSample::onAddPointset()
{
    QString name = QInputDialog::getText(this,tr("New Pointset"),tr("Name:"),QLineEdit::Normal,tr("New Points"));
    if (name.isEmpty())
        return;
    for(int i=0;i<model->rowCount();++i)
    {
        if (name == model->data(model->index(i,0)).toString())
        {
            QMessageBox::critical(this,tr("Error"),tr("%1 is already exist!").arg(name));
            return;
        }
    }

    bool ok = false;
    int count = QInputDialog::getInt(this,tr("Input"),tr("Count of generating points"),50,1,100000,1,&ok);
    if (ok==false)
        return;


    QgsRectangle extent = CDTImageLayer::getLayer(imgID)->canvasLayer()->extent();
    QVector<QPointF> points = generatePoints(count,extent);
    insertPointsIntoDB(points,name);

    updateCombobox();
}


void DialogGenerateValidationSample::updateCombobox()
{
    model->setQuery(QString("select pointset_name from points_project "
                            "left join imagelayer "
                            "using('projectID') "
                            "where imagelayer.id = '%1'")
                    .arg(imgID),QSqlDatabase::database("category"));
}

QVector<QPointF> DialogGenerateValidationSample::generatePoints(int pointsCount, const QgsRectangle &extent)
{
    const int PRECISE = 10000;
    qsrand(std::clock());

    QVector<QPointF> points;
    for (int i=0;i<pointsCount;++i)
    {
        double x_dot = static_cast<double>(qrand()%PRECISE)/PRECISE;
        double y_dot = static_cast<double>(qrand()%PRECISE)/PRECISE;
        double x = x_dot*extent.width();
        double y = y_dot*extent.height();
        points.push_back(QPointF(x+extent.xMinimum(),y+extent.yMinimum()));
    }
    return points;
}

bool DialogGenerateValidationSample::insertPointsIntoDB(QVector<QPointF> points, QString pointset_name)
{
    CDTProjectLayer* prjLayer = qobject_cast<CDTProjectLayer*>(CDTImageLayer::getLayer(imgID)->parent());
    if (prjLayer==NULL)
        return false;

    QString prjID = prjLayer->id().toString();

    QSqlDatabase db = QSqlDatabase::database("category");
    QSqlQuery query(db);
    db.transaction();
    try
    {
        if (query.prepare("insert into points values(?,?,?,?)")==false)
            throw "prepare insert into points failed!";
        int i=0;
        foreach (QPointF pt, points) {
            query.addBindValue(i++);
            query.addBindValue(pt.x());
            query.addBindValue(pt.y());
            query.addBindValue(pointset_name);
            if (query.exec()==false)
                throw "insert into points failed!";
        }

        if (query.prepare("insert into points_project values(?,?)")==false)
                    throw "prepare insert into points_project failed!";
        query.addBindValue(pointset_name);
        query.addBindValue(prjID);
        if (query.exec()==false)
            throw "insert into points_project failed!";

        db.commit();
        return true;
    }
    catch (QString msg)
    {
        db.rollback();
        logger()->error(msg);
        return false;
    }
}

QStringList DialogGenerateValidationSample::getValidationNames() const
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.exec(QString("select name from image_validation_samples"));
    QStringList list;
    while (query.next()) {
        list<<query.value(0).toString();
    }
    return list;
}
