#include <QClipboard>
#include <QTimer>
#include <QMenu>
#include <QPushButton>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QFileDialog>
#include <QSvgRenderer>
#include <QImageReader>
#include <QBuffer>
#include <QPainter>
#include <QRect>

#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include "BiliApi/BilibiliUrl.h"
#include "Utils/Utility.h"
#include "Plugin/PluginManager.h"
#include "Storage/SearchHistoryStorage.h"
#include "Storage/StorageManager.h"
#include "Login/LoginMonitor.h"
#include "SUI/QrCodeGenerator.h"
#include "Utils/RunTask.h"

LoginDialog::LoginDialog(std::shared_ptr<AbstractLogin> loginer, QDialog* parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , m_monitor(new LoginMonitor(this))
    , m_loginer(loginer)
    , m_status(AbstractLogin::Unknow)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
    loadOrc();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::slotStatusChanged(AbstractLogin::LoginSatus status)
{
    if (status == m_status)
    {
        return;
    }
    m_status = status;
    m_movie.stop();
    ui->btnRefresh->hide();

    switch (status)
    {
    case AbstractLogin::Error:
    {
        auto svgContext = m_loginer->resource(AbstractLogin::Refresh);
        auto pixmap = binToImage(svgContext, ui->btnRefresh->size());
        QIcon icon(pixmap);
        ui->btnRefresh->setIcon(icon);
        ui->labelStrTip->setText(tr("Error occured, you can refresh QR code"));
        ui->btnRefresh->show();
        ui->btnRefresh->raise();
        break;
    }
    case AbstractLogin::Timeout:
    {
        auto svgContext = m_loginer->resource(AbstractLogin::Refresh);
        auto pixmap = binToImage(svgContext, ui->btnRefresh->iconSize());
        QIcon icon(pixmap);
        ui->btnRefresh->setIcon(icon);
        ui->labelStrTip->setText(tr("Timeout, please refresh QR code"));
        ui->btnRefresh->show();
        ui->btnRefresh->raise();
        break;
    }
    case AbstractLogin::ScanedNoAck:
    {
        auto svgContext = m_loginer->resource(AbstractLogin::WaitConfirm);
        auto pixmap = binToImage(svgContext, ui->btnRefresh->iconSize());
        QIcon icon(pixmap);
        ui->btnRefresh->setIcon(icon);
        ui->labelStrTip->setText(tr("Scaned, please confirm it in app"));
        ui->btnRefresh->show();
        ui->btnRefresh->raise();
        break;
    }
    case AbstractLogin::Success:
    {
        m_loginer->loginSuccess();
        QTimer::singleShot(1000, this, [this] {
            accept();
        });
        break;
    }
    case AbstractLogin::NoScan:
    default:
        break;
    }
}

void LoginDialog::slotBtnRefreshClicked()
{
    if (m_status == AbstractLogin::Error || m_status == AbstractLogin::Timeout)
    {
        loadOrc();
    }
    else if (m_status == AbstractLogin::Success)
    {
        accept();
    }
}

void LoginDialog::resizeEvent(QResizeEvent* event)
{
    ui->labelBackground->setGeometry(0, 0, width(), height());
    auto gmty = ui->labelOrc->geometry();
    gmty.moveCenter(QPoint(width() / 2, height() / 2));
    ui->labelOrc->setGeometry(gmty);
    gmty = ui->labelTooltip->geometry();
    gmty.moveCenter(QPoint(width() / 2, height() / 2));
    ui->labelTooltip->setGeometry(gmty);
    gmty = ui->btnRefresh->geometry();
    gmty.moveCenter(QPoint(width() / 2, height() / 2));
    ui->btnRefresh->setGeometry(gmty);
    return QDialog::resizeEvent(event);
}

bool LoginDialog::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == ui->labelOrc && event->type() == QEvent::Enter && m_status == AbstractLogin::NoScan)
    {
        ui->labelTooltip->show();
    }
    else if (obj == ui->labelTooltip && event->type() == QEvent::Leave && m_status == AbstractLogin::NoScan)
    {
        ui->labelTooltip->hide();
    }

    return QDialog::eventFilter(obj, event);
}

void LoginDialog::signalsAndSlots()
{
    connect(m_monitor, &LoginMonitor::sigLoginStatus, this, &LoginDialog::slotStatusChanged);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &LoginDialog::slotBtnRefreshClicked);
}

void LoginDialog::setUi()
{
    auto svgContext = m_loginer->resource(AbstractLogin::Background);
    auto pixmap = binToImage(svgContext, ui->labelBackground->size());
    ui->labelBackground->setPixmap(pixmap);
    ui->labelBackground->lower();

    svgContext = m_loginer->resource(AbstractLogin::Confirmed);
    pixmap = binToImage(svgContext, ui->labelOrc->size());
    ui->labelOrc->setPixmap(pixmap);
    ui->labelOrc->raise();
    ui->labelOrc->installEventFilter(this);

    ui->btnRefresh->raise();
    svgContext = m_loginer->resource(AbstractLogin::Loading);
    m_buffer.setData(reinterpret_cast<const char*>(svgContext.data()), svgContext.size());
    connect(&m_movie, &QMovie::frameChanged, this, [this]() {
        ui->btnRefresh->setIcon(QIcon(m_movie.currentPixmap()));
    });
    m_movie.setDevice(&m_buffer);
    m_movie.start();

    svgContext = m_loginer->resource(AbstractLogin::Tip);
    pixmap = binToImage(svgContext, ui->labelTooltip->size());
    ui->labelTooltip->setPixmap(pixmap);
    ui->labelTooltip->raise();
    ui->labelTooltip->installEventFilter(this);
    ui->labelTooltip->hide();
}

void LoginDialog::loadOrc()
{
    if (!m_loginer)
    {
        return;
    }

    m_status = AbstractLogin::Unknow;
    auto taskFunc = [this]() {
        return m_loginer->getScanContext(m_context);
    };

    auto callback = [this](bool ret) {
        if (!ret || m_context.empty())
        {
            slotStatusChanged(AbstractLogin::Error);
            return;
        }
        auto image = QrCodeGenerator().generateQR(QString::fromStdString(m_context));
        auto pixmap = QPixmap::fromImage(image).scaled(ui->labelOrc->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labelOrc->setPixmap(pixmap);
        m_status = AbstractLogin::NoScan;
        m_monitor->setLoginer(m_loginer);
        m_movie.stop();
        ui->btnRefresh->hide();
    };

    runTask(taskFunc, callback, this);
}

QPixmap LoginDialog::binToImage(const std::vector<uint8_t>& bin, QSize size)
{
    QBuffer buffer;
    buffer.setData(reinterpret_cast<const char*>(bin.data()), bin.size());
    buffer.open(QIODevice::ReadOnly);

    QImageReader render(&buffer);
    QImage image = render.read();
    image = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap pixmap = QPixmap::fromImage(image);
    return pixmap;
}
