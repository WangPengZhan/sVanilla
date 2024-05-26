#include <QAction>
#include <QFileDialog>
#include <QPainter>
#include <QResizeEvent>
#include <QToolButton>
#include <QValidator>
#include "FileSelector.h"
#include "ui_FileSelector.h"

class ModeValidator : public QValidator
{
    Q_OBJECT
public:
    explicit ModeValidator(FileSelector* selector, QObject* parent)
        : QValidator(parent)
        , m_selector(selector)
    {
    }

    State validate(QString& input, int& /*pos*/) const override
    {
        if (input.isEmpty())
        {
            return Acceptable;
        }
        switch (m_selector->mode())
        {
        case FileSelector::Directory:
            return QFileInfo(input).isDir() ? Acceptable : Intermediate;
        case FileSelector::FileOpen:
            return QFileInfo(input).isFile() ? Acceptable : Intermediate;
        }
        Q_UNREACHABLE();
    }

private:
    FileSelector* m_selector = nullptr;
};

FileSelector::FileSelector(QWidget* parent)
    : FileSelector(Directory, parent)
{
}

FileSelector::FileSelector(Mode mode, QWidget* parent)
    : QLineEdit(parent)
    , ui(new Ui::FileSelector)
    , m_validator(new ModeValidator(this, this))
{
    m_action = new QAction(this);
    m_action->setIcon(QIcon(":icon/download/folder.svg"));
    addAction(m_action, QLineEdit::TrailingPosition);
    setMode(mode);
    setValidator(m_validator);
    connect(m_action, &QAction::triggered, this, &FileSelector::chooseFile);
}

void FileSelector::chooseFile()
{
    const QString fileName = [this] {
        switch (mode())
        {
        case FileOpen:
            return QFileDialog::getOpenFileName(this, QString(), text());
        case Directory:
            return QFileDialog::getExistingDirectory(this, QString(), text());
        }
        Q_UNREACHABLE();
    }();

    if (!fileName.isEmpty())
    {
        setText(fileName);
        QString name = fileName;
        if (int pos = 0; m_validator->validate(name, pos) == QValidator::Acceptable)
        {
            emit editingFinished();
        }
    }
}

void FileSelector::setMode(Mode mode)
{
    const QString text = [mode] {
        switch (mode)
        {
        case Mode::FileOpen:
            return "Choose file to open";
        case Mode::Directory:
            return "Choose Directory";
        }
        Q_UNREACHABLE();
    }();
    setPlaceholderText(text);

    if (mode != m_mode)
    {
        m_mode = mode;
        emit modeChanged(mode);
    }
}

FileSelector::Mode FileSelector::mode() const
{
    return m_mode;
}

FileSelector::~FileSelector()
{
    delete ui;
}
#include "FileSelector.moc"