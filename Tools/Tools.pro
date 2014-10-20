TEMPLATE = subdirs

SUBDIRS += \
    log4qt \
    QPropertyEditor\
    CDTFileSystem\
    CDTHistogramPlot\
    QtColorPicker\    
    XMLSchemaValidator

CDTFileSystem.depends = log4qt
