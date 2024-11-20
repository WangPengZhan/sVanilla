#include <QDir>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>
#include <QScrollBar>
#include <QMenu>
#include <QShortcut>
#include <QContextMenuEvent>
#include <QStyleOption>
#include <QMovie>

#include "ui_DownloadNumber.h"
#include "DownloadNumber.h"

DownloadNumber::DownloadNumber(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadNumber)
    , m_movie(new QMovie(":/icon/downloading_loop.svg"))
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

DownloadNumber::~DownloadNumber()
{
    delete ui;
    delete m_movie;
}

void DownloadNumber::setDownloadingNumber(int downloading)
{
    if (downloading == 0)
    {
        ui->labelDownloadingIcon->hide();
        ui->labelDownloadingNumber->clear();
        ui->labelDownloadingNumber->hide();
        m_movie->stop();
    }
    else
    {
        if (ui->labelDownloadingIcon->isHidden())
        {
            ui->labelDownloadingIcon->show();
            ui->labelDownloadingNumber->show();
            m_movie->start();
        }
        ui->labelDownloadingNumber->setText(QString::number(downloading));
    }
}

void DownloadNumber::setDownloadErrorNumber(int dowloadError)
{
    if (dowloadError == 0)
    {
        ui->labelDownloadError->hide();
        ui->labelDownloadErrorNumber->clear();
        ui->labelDownloadErrorNumber->hide();
    }
    else
    {
        if (ui->labelDownloadError->isHidden())
        {
            ui->labelDownloadError->show();
            ui->labelDownloadErrorNumber->show();
        }
        ui->labelDownloadErrorNumber->setText(QString::number(dowloadError));
    }
}

int DownloadNumber::downloadingNumber() const
{
    return ui->labelDownloadingNumber->text().toInt();
}

int DownloadNumber::downloadErrorNumber() const
{
    return ui->labelDownloadErrorNumber->text().toInt();
}

void DownloadNumber::resizeEvent(QResizeEvent* event)
{
    int width = height() - contentsMargins().top() - contentsMargins().bottom();

    ui->labelDownloadingIcon->setFixedSize(QSize(width, width));
    ui->labelDownloadingNumber->setFixedSize(QSize(width, width));
    ui->labelDownloadError->setFixedSize(QSize(width, width));
    ui->labelDownloadErrorNumber->setFixedSize(QSize(width, width));

    return QWidget::resizeEvent(event);
}

void DownloadNumber::setUi()
{
    // ui->labelDownloadingIcon->setMovie(m_movie);
    ui->labelDownloadingIcon->setPixmap(QPixmap(":/icon/downloading_loop.svg"));
    ui->labelDownloadError->setPixmap(QPixmap(":/icon/download_error.svg"));

    setDownloadingNumber(0);
    setDownloadErrorNumber(0);
}

void DownloadNumber::signalsAndSlots()
{
}
