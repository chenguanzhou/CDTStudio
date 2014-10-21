#include "dialogdecisionfusion.h"
#include "ui_dialogdecisionfusion.h"

#include "stable.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"

DialogDecisionFusion::DialogDecisionFusion(QString segID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDecisionFusion)
{
    ui->setupUi(this);
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery(QString("select name,id from classificationlayer where segmentationID = '%1'")
                    .arg(segID),QSqlDatabase::database("category"));
    ui->listView->setModel(model);
    connect(ui->listView->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(onSelectionChanged()));
}

DialogDecisionFusion::~DialogDecisionFusion()
{
    delete ui;
}

void DialogDecisionFusion::onSelectionChanged()
{
    ui->pushButton->setEnabled( ui->listView->selectionModel()->selectedRows().size()>1 );
}
