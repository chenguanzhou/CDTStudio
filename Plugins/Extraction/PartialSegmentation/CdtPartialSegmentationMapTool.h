#ifndef CDTGRABCUTMAPTOOL_H
#define CDTGRABCUTMAPTOOL_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <qgsmaptool.h>

class QgsMapCanvas;
class QgsRubberBand;
class QgsVectorLayer;
class CDTFileSystem;

class CDTPartialSegmentationMapTool : public QgsMapTool
{
    Q_OBJECT
public:
    friend class PartialSegmentationInterface;
    explicit CDTPartialSegmentationMapTool(QgsMapCanvas *canvas);
    ~CDTPartialSegmentationMapTool();

    virtual void canvasMoveEvent ( QgsMapMouseEvent * e );
    virtual void canvasPressEvent(QgsMapMouseEvent *e );
signals:

public slots:

private:
    QgsRubberBand   *mRubberBand;
    QString         imagePath;
    QgsVectorLayer  *vectorLayer;

};

#endif // CDTGRABCUTMAPTOOL_H
