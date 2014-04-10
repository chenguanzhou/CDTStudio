#ifndef CDTTRAININGSAMPLESFORM_H
#define CDTTRAININGSAMPLESFORM_H

#include <QWidget>


namespace Ui {
class CDTTrainingSamplesForm;
}
class CDTImageLayer;
class CDTTrainingSamplesForm : public QWidget
{
    Q_OBJECT

public:
    explicit CDTTrainingSamplesForm( QWidget *parent = 0);
    ~CDTTrainingSamplesForm();

    void setImageLayer(CDTImageLayer *layer);
public slots:
    void clear();
    void updateCategoryInfo();

private:
    Ui::CDTTrainingSamplesForm *ui;
    CDTImageLayer* imageLayer;
//    void initForm();
};

#endif // CDTTRAININGSAMPLESFORM_H
