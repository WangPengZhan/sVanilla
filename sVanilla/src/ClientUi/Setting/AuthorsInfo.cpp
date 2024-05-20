#include <QFile>

#include "AuthorsInfo.h"
#include "ui_AuthorsInfo.h"

AuthorsInfo::AuthorsInfo(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AuthorsInfo)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();

    loadAuthors();
}

AuthorsInfo::~AuthorsInfo()
{
    delete ui;
}

void AuthorsInfo::resizeEvent(QResizeEvent* event)
{
    if (!m_resized)
    {
        if (ui->tableAuthors->columnCount() > 1)
        {
            for (int i = 0; i < ui->tableAuthors->columnCount() - 1; ++i)
            {
                ui->tableAuthors->setColumnWidth(i, ui->tableAuthors->width() / ui->tableAuthors->columnCount());
            }
        }

        m_resized = true;
    }

    return QWidget::resizeEvent(event);
}

void AuthorsInfo::setUi()
{
    ui->tableAuthors->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
}

void AuthorsInfo::signalsAndSlots()
{
}

void AuthorsInfo::loadAuthors()
{
    const QString& path = QApplication::applicationDirPath() + "/authors.csv";

    QFile authorsFile(path);
    authorsFile.open(QFile::ReadOnly | QFile::Text);
    QVector<QStringList> authors;
    while (!authorsFile.atEnd())
    {
        const QString context = authorsFile.readLine();
        const QStringList list = context.split(",");
        if (list.size() == ui->tableAuthors->columnCount())
        {
            authors.push_back(list);
        }
    }

    ui->tableAuthors->setRowCount(authors.size());
    int i = 0;
    for (const auto& author : authors)
    {
        auto authorStr = author[0].trimmed();
        auto emailStr = author[1].trimmed();
        ui->tableAuthors->setItem(i, 0, new QTableWidgetItem(authorStr));
        ui->tableAuthors->setItem(i, 1, new QTableWidgetItem(emailStr));
        i++;
    }
}
