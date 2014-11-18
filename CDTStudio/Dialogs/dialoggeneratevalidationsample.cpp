#include "dialoggeneratevalidationsample.h"
#include "ui_dialoggeneratevalidationsample.h"

#include "stable.h"


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
    updateCombobox();
}

DialogGenerateValidationSample::~DialogGenerateValidationSample()
{
    delete ui;
}

QStringList DialogGenerateValidationSample::getGeneratedValidationPointsetName(QString projectID, QWidget *parent)
{
    QStringList info;
    DialogGenerateValidationSample dlg(projectID,parent);
    dlg.exec();
    info << dlg.ui->lineEditName->text();
    info << dlg.ui->comboBoxPointset->currentText();
    return info;
}

void DialogGenerateValidationSample::onComboBoxChanged(QString pointsetName)
{
    try{
        if (pointsetName.isEmpty())
            throw std::exception("pointsetName is empty");

        QSqlQuery query(QSqlDatabase::database("category"));
        if (query.exec(QString("select count(*) from points_project where pointset_name='%1'")
                       .arg(pointsetName))==false)
                throw std::exception(QString("Excute sql failed. Error:%1")
                                     .arg(query.lastError().text()).toLocal8Bit().constData());
        query.next();
        int count = query.value(0).toInt();
        if (count<0)
            throw std::exception(QString("pointsetName: %1 is cnot found")
                                 .arg(pointsetName).toLocal8Bit().constData());
        qDebug()<<pointsetName;
        if (query.exec(QString("select count(*) from points where pointset_name = '%1'")
                   .arg(pointsetName))==false)
            throw std::exception(QString("Excute sql failed. Error:%1")
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


void DialogGenerateValidationSample::updateCombobox()
{
    model->setQuery(QString("select pointset_name from points_project "
                            "left join imagelayer "
                            "using('projectID') "
                            "where imagelayer.id = '%1'")
                    .arg(imgID),QSqlDatabase::database("category"));
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
