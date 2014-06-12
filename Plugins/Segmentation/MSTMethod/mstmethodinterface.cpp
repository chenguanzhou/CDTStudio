#include "mstmethodinterface.h"
#include "mstsegmenter.h"

class MSTMethodPrivate
{
public:
    MSTMethodPrivate():threshold(20),minObjectCount(100),shieldNulValue(false){}
    double threshold;
    int minObjectCount;
    bool shieldNulValue;
};

MSTMethodInterface::MSTMethodInterface(QObject *parent)
    :CDTSegmentationInterface(parent),
      pData(new MSTMethodPrivate)
{
}

MSTMethodInterface::~MSTMethodInterface()
{
}

QString MSTMethodInterface::segmentationMethod() const
{
    return QString("mst");
}

void MSTMethodInterface::startSegmentation(QProgressBar *progressBar,QLabel *label)
{
    MSTSegmenter* mstSegmenter = new MSTSegmenter(
                inputImagePath,
                markfilePath,
                shapefilePath,
                //                    ui->comboBoxMergeRule->currentIndex(),
                pData->threshold,
                pData->minObjectCount,
                pData->shieldNulValue);


    connect(mstSegmenter, SIGNAL(finished()), this, SIGNAL(finished()));

    if (progressBar)
    {
        connect(mstSegmenter, SIGNAL(progressBarSizeChanged(int,int)),progressBar,SLOT(setRange(int,int)));
        connect(mstSegmenter, SIGNAL(progressBarValueChanged(int)),progressBar,SLOT(setValue(int)));
    }
    if (label)
    {
        connect(mstSegmenter, SIGNAL(currentProgressChanged(QString)),label,SLOT(setText(QString)));
    }

    mstSegmenter->start();
}

double MSTMethodInterface::threshold() const
{
    return pData->threshold;
}

int MSTMethodInterface::minObjectCount() const
{
    return pData->minObjectCount;
}

bool MSTMethodInterface::shieldNulValue() const
{
    return pData->shieldNulValue;
}

void MSTMethodInterface::setThreshold(double val)
{
    pData->threshold = val;
}

void MSTMethodInterface::setMinObjectCount(int val)
{
    pData->minObjectCount = val;
}

void MSTMethodInterface::setShieldNulValue(bool val)
{
    pData->shieldNulValue = val;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(MSTMethod, MSTMethodInterface)
#endif // QT_VERSION < 0x050000
