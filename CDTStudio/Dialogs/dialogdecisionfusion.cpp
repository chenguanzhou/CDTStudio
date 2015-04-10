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
    model->setQuery(QString("select name,id from classificationlayer where segmentationID = '%1' and method <> 'decision fusion'")
                    .arg(segID),QSqlDatabase::database("category"));
    ui->listView->setModel(model);
    connect(ui->listView->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),SLOT(onSelectionChanged()));
    ui->listView->selectAll();

    connect(ui->pushButton,SIGNAL(clicked()),SLOT(startDecisionFusion()));
}

DialogDecisionFusion::~DialogDecisionFusion()
{
    delete ui;
}

void DialogDecisionFusion::onSelectionChanged()
{
    ui->pushButton->setEnabled( ui->listView->selectionModel()->selectedRows().size()>1 );
}

void DialogDecisionFusion::startDecisionFusion()
{
    int objCount = 0;
    QList<CDTClassificationLayer *> layers;

    QVariantList result;
    QList<QVariantList> labels;
    auto selectedRows = ui->listView->selectionModel()->selectedRows();
    foreach (QModelIndex index, selectedRows) {
        auto model = ui->listView->model();
        QString id = model->data(model->index(index.row(),1)).toString();
        CDTClassificationLayer *layer = CDTClassificationLayer::getLayer(QUuid(id));
        if (layer==NULL)
            return;
        layers<<layer;
        labels.push_back(layer->data());
    }
    objCount = labels[0].size();

    for (int i=0;i<objCount;++i)
    {
        QVector<double> votes(labels.size(),0.);
        for(int k=0;k<labels.size();++k)
        {
            votes[labels[k][i].toInt()] += 1;
        }
        int winner = std::max_element(votes.begin(),votes.end())-votes.begin();

        result.push_back(winner);
    }

    name = "decision fusion";
    method = "decision fusion";
    label = result;
    categoryID_Index = layers[0]->clsInfo();

}
