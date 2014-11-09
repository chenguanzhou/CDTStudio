TEMPLATE = subdirs

SUBDIRS += \
    log4qt \
    QPropertyEditor\
    CDTFileSystem\
    CDTHistogramPlot\
    QtColorPicker\    
    XMLSchemaValidator\
    CDTTableExporter

CDTFileSystem.depends = log4qt
