TEMPLATE = subdirs

SUBDIRS += \
    qwt\
    QPropertyEditor\
    CDTFileSystem\
    CDTHistogramPlot\
    QtColorPicker\    
    XMLSchemaValidator\
    CDTTableExporter\
    wwWidgets

CDTHistogramPlot.depends = qwt
