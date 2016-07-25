#ifndef CDTSNAKEMAPTOOL_H
#define CDTSNAKEMAPTOOL_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <qgsmaptool.h>

class QgsMapCanvas;
class QgsRubberBand;
class QgsVectorLayer;

class CDTSnakeMapTool : public QgsMapTool
{
    Q_OBJECT
public:
    friend class SnakeInterface;
    explicit CDTSnakeMapTool(QgsMapCanvas *canvas);
    ~CDTSnakeMapTool();

    virtual void canvasMoveEvent ( QgsMapMouseEvent * e ) override;
    virtual void canvasPressEvent( QgsMapMouseEvent * e ) override;
signals:

public slots:

private:
    QgsRubberBand   *mRubberBand;
    QString         imagePath;
    QgsVectorLayer  *vectorLayer;
};

#endif // CDTSNAKEMAPTOOL_H
