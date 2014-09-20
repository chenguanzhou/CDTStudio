#include "dialogvalidator.h"
#include "ui_dialogvalidator.h"
#include <QtCore>
#include <QtXmlPatterns>
#include "xmlsyntaxhighlighter.h"

class MessageHandler : public QAbstractMessageHandler
{
public:
    MessageHandler()
        : QAbstractMessageHandler(0)
    {
    }

    QString statusMessage() const
    {
        return m_description;
    }

    int line() const
    {
        return m_sourceLocation.line();
    }

    int column() const
    {
        return m_sourceLocation.column();
    }

protected:
    virtual void handleMessage(QtMsgType type, const QString &description,
                               const QUrl &identifier, const QSourceLocation &sourceLocation)
    {
        Q_UNUSED(type);
        Q_UNUSED(identifier);

        m_messageType = type;
        m_description = description;
        m_sourceLocation = sourceLocation;
    }

private:
    QtMsgType m_messageType;
    QString m_description;
    QSourceLocation m_sourceLocation;
};

DialogValidator::DialogValidator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogValidator)
{
    ui->setupUi(this);

    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowMinimizeButtonHint;
    flags |=Qt::WindowMaximizeButtonHint;
    setWindowFlags(flags);

    new XmlSyntaxHighlighter(ui->textEditXML->document());
    new XmlSyntaxHighlighter(ui->textEditXSD->document());

    QFile fileXSD(":/contact.xsd");
    fileXSD.open(QFile::ReadOnly);
    ui->textEditXSD->setText(QString::fromUtf8(fileXSD.readAll()));
    fileXSD.close();

    QFile fileXML(":/valid_contact.xml");
    fileXML.open(QFile::ReadOnly);
    ui->textEditXML->setText(QString::fromUtf8(fileXML.readAll()));
    fileXML.close();
}

DialogValidator::~DialogValidator()
{
    delete ui;
}

void DialogValidator::moveCursor(int line, int column)
{
    ui->textEditXML->moveCursor(QTextCursor::Start);
    for (int i = 1; i < line; ++i)
        ui->textEditXML->moveCursor(QTextCursor::Down);

    for (int i = 1; i < column; ++i)
        ui->textEditXML->moveCursor(QTextCursor::Right);

    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;

    const QColor lineColor = QColor(Qt::red).lighter(160);
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = ui->textEditXML->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);

    ui->textEditXML->setExtraSelections(extraSelections);

    ui->textEditXML->setFocus();
}

void DialogValidator::on_pushButton_clicked()
{
    const QByteArray schemaData = ui->textEditXSD->toPlainText().toUtf8();
    const QByteArray instanceData = ui->textEditXML->toPlainText().toUtf8();

    MessageHandler messageHandler;
    QXmlSchema schema;
    schema.setMessageHandler(&messageHandler);
    schema.load(schemaData);

    bool errorOccurred = false;
    if (!schema.isValid()) {
        errorOccurred = true;
    } else {
        QXmlSchemaValidator validator(schema);
        if (!validator.validate(instanceData))
            errorOccurred = true;
    }

    if (errorOccurred) {
        ui->validationStatus->setText(messageHandler.statusMessage());
        moveCursor(messageHandler.line(), messageHandler.column());
    } else {
        ui->validationStatus->setText(tr("validation successful"));
    }

    const QString styleSheet = QString("QLabel {background: %1; padding: 3px}")
                                      .arg(errorOccurred ? QColor(Qt::red).lighter(160).name() :
                                                           QColor(Qt::green).lighter(160).name());
    ui->validationStatus->setStyleSheet(styleSheet);
}
