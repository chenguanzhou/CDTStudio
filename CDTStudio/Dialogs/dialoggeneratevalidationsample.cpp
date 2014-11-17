#include "dialoggeneratevalidationsample.h"
#include "ui_dialoggeneratevalidationsample.h"

#include "stable.h"

DialogGenerateValidationSample::DialogGenerateValidationSample(QString projectID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogGenerateValidationSample),
    model(new QSqlQueryModel(this)),
    prjID(projectID)
{
    ui->setupUi(this);    
    ui->comboBoxPointset->setModel(model);
    updateCombobox();
    QStringList validationNames = getValidationNames();
    CDTBlackListValidator *validator = new CDTBlackListValidator(validationNames,this);
    ui->lineEditName->setValidator(validator);
}

DialogGenerateValidationSample::~DialogGenerateValidationSample()
{
    delete ui;
}

void DialogGenerateValidationSample::updateCombobox()
{
    model->setQuery(QString("select pointset_name from points_project where projectid = '%1'")
                    .arg(prjID),QSqlDatabase::database("category"));
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
