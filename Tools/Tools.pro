TEMPLATE = subdirs

SUBDIRS += \
    log4qt \
    QPropertyEditor\
    CDTFileSystem\
    CDTHistogramPlot\
    QtColorPicker\    
    CDTClassifierAssessmentWidget \
    XMLSchemaValidator \
    CDTTask

CDTFileSystem.depends = log4qt
