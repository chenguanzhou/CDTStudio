#include "dialogcategoryinformation.h"
#include "ui_dialogcategoryinformation.h"
#include <QToolBar>
#include <QColorDialog>
#include <QMessageBox>
#include <QToolTip>

QDataStream &operator <<(QDataStream &out,const CategoryInformation &categoryInformation)
{
    out<<categoryInformation.categoryName<<categoryInformation.color;
    return out;
}

QDataStream &operator >>(QDataStream &in, CategoryInformation &categoryInformation)
{
    in>>categoryInformation.categoryName>>categoryInformation.color;
    return in;
}

DialogCategoryInformation::DialogCategoryInformation(CDTCategoryInformationList categoryInfo, bool isReadOnly, QWidget *parent) :
    QDialog(parent),
    categoryInformationList(categoryInfo),
    ui(new Ui::DialogCategoryInformation)
{
    ui->setupUi(this);

    if (isReadOnly == false)
    {
        ui->listWidget->addAction(ui->actionActionInsert);
        ui->listWidget->addAction(ui->actionActionRemove);


        QToolBar *menuBar = new QToolBar(tr("Categories"),this);
        menuBar->addAction(ui->actionActionInsert);
        menuBar->addAction(ui->actionActionRemove);
        menuBar->setIconSize(QSize(16,16));
        ui->verticalLayout->insertWidget(0,menuBar);
        ui->groupBox->hide();
        connect(ui->lineEdit,SIGNAL(textEdited(QString)),SLOT(onCategoryNameChanged(QString)));
        connect(this,SIGNAL(accepted()),SLOT(onAccepted()));
    }
    else
    {
        ui->groupBox->setEnabled(false);
    }

    this->adjustSize();



    initInfo();
}

DialogCategoryInformation::~DialogCategoryInformation()
{
    delete ui;
}

void DialogCategoryInformation::on_pushButton_clicked()
{
    QColor clr = QColorDialog::getColor(ui->pushButton->palette().color(QPalette::Button),this);
    if (clr.isValid())
    {
        setColor(clr);
        ui->listWidget->currentItem()->setData(Qt::UserRole,clr);
    }
}

void DialogCategoryInformation::on_actionActionInsert_triggered()
{
    QListWidgetItem* item = new QListWidgetItem(tr("untitled"),ui->listWidget);
    ui->listWidget->addItem(item);
}

void DialogCategoryInformation::on_actionActionRemove_triggered()
{
    if (ui->listWidget->count()<=0)
        return;
    if (ui->listWidget->currentRow()<0)
        return;
    QListWidgetItem* item = ui->listWidget->currentItem();
    ui->listWidget->removeItemWidget(item);
    delete item;
    if (ui->listWidget->count()==0)
    {
        ui->groupBox->hide();
        this->adjustSize();
    }
}

void DialogCategoryInformation::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (ui->listWidget->currentRow()>=0)
    {
        ui->groupBox->show();
        this->adjustSize();
        ui->lineEdit->setText(current->text());
        setColor(ui->listWidget->currentItem()->data(Qt::UserRole).value<QColor>());
    }
}

void DialogCategoryInformation::onCategoryNameChanged(const QString &name)
{    
    if (ui->listWidget->currentRow()<0)
        return;
    for(int i=0;i<ui->listWidget->count();++i)
    {
        if (i==ui->listWidget->currentRow()) continue;
        QListWidgetItem* item = ui->listWidget->item(i);
        if (item->text()==name)
        {
            QPalette palette = ui->lineEdit->palette();
            palette.setColor(QPalette::Text,QColor(Qt::red));
            ui->lineEdit->setPalette(palette);
            QToolTip::showText( ui->lineEdit->mapToGlobal( ui->lineEdit->rect().topLeft()),name +tr(" is exsit!"),ui->lineEdit);
            ui->buttonBox->setStandardButtons(ui->buttonBox->standardButtons()&~QDialogButtonBox::Ok);
            return;
        }
    }

    QPalette palette = ui->lineEdit->palette();
    palette.setColor(QPalette::Text,QColor(Qt::black));
    ui->lineEdit->setPalette(palette);
    ui->listWidget->currentItem()->setText(name);
    ui->buttonBox->setStandardButtons(ui->buttonBox->standardButtons()|QDialogButtonBox::Ok);
}

void DialogCategoryInformation::initInfo()
{
    foreach (CategoryInformation categoryInformation, categoryInformationList) {
        QListWidgetItem *item = new QListWidgetItem(categoryInformation.categoryName,ui->listWidget);
        ui->listWidget->addItem(item);
        item->setData(Qt::UserRole,categoryInformation.color);
    }
}

void DialogCategoryInformation::setColor(const QColor &color)
{
    QPalette palette =  ui->pushButton->palette();
    palette.setColor(QPalette::Button,color);
    QColor inverse(color);
    inverse.setRed(255-inverse.red());
    inverse.setGreen(255-inverse.green());
    inverse.setBlue(255-inverse.blue());
    inverse.darker();
    palette.setColor(QPalette::ButtonText,inverse);
    ui->pushButton->setPalette(palette);
    ui->pushButton->setText(color.name());
}

void DialogCategoryInformation::onAccepted()
{   
    CDTCategoryInformationList newList;
    for (int i=0;i<ui->listWidget->count();++i)
    {
        QListWidgetItem* item = ui->listWidget->item(i);
        newList<<CategoryInformation(item->text(),item->data(Qt::UserRole).value<QColor>());
    }

    QSet<QString> nameSet;
    foreach (CategoryInformation categoryInformation, newList) {
        nameSet<<categoryInformation.categoryName;
    }

    if (nameSet.size() != newList.size())
    {
        QMessageBox::warning(this,tr("Error"),tr("Categories can't share same name!"));
        return;
    }

    categoryInformationList = newList;
}
