#ifndef CDTEXTRACTIONDOCKWIDGET_H
#define CDTEXTRACTIONDOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class CDTExtractionDockWidget;
}

class CDTExtractionDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit CDTExtractionDockWidget(QWidget *parent = 0);
    ~CDTExtractionDockWidget();

private:
    Ui::CDTExtractionDockWidget *ui;
};

#endif // CDTEXTRACTIONDOCKWIDGET_H
