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
#include "BaseQt/Utility.h"
#include "Plugin/PluginManager.h"
#include "Storage/SearchHistoryStorage.h"
#include "Storage/StorageManager.h"
#include "Storage/CookiesInfoStorage.h"
#include "Login/LoginMonitor.h"
#include "NetWork/CurlCpp/CurlCookies.h"
#include "NetWork/CurlCpp/CurlCookie.h"
#include "Utils/RunTask.h"
#include "BaseQt/Utility.h"
#include "LoginProxy.h"
#include "ClientLog.h"
#include "const_string.h"

LoginDialog::LoginDialog(std::shared_ptr<LoginProxy> loginer, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , m_monitor(new LoginMonitor(this))
    , m_loginer(loginer)
    , m_status(AbstractLoginApi::Unknow)
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

void LoginDialog::slotStatusChanged(AbstractLoginApi::LoginStatus status)
{
    if (status == m_status)
    {
        return;
    }

    m_status = status;
    m_movie.stop();
    ui->btnRefresh->hide();

    auto realLogin = reinterpret_cast<AbstractLoginApi*>(&m_loginer->realLogin());
    switch (status)
    {
    case AbstractLoginApi::Error:
    {
        MLogI(svanilla::cLoginModule, "LoginStatus Error!");
        auto svgContext = realLogin->resource(AbstractLoginApi::Refresh);
        auto pixmap = util::binToImage(svgContext, ui->btnRefresh->size());
        QIcon icon(pixmap);
        ui->btnRefresh->setIcon(icon);
        ui->labelStrTip->setText(tr("Error occurred, you can refresh QR code"));
        ui->btnRefresh->show();
        ui->btnRefresh->raise();
        break;
    }
    case AbstractLoginApi::Timeout:
    {
        MLogI(svanilla::cLoginModule, "LoginStatus Timeout!");
        auto svgContext = realLogin->resource(AbstractLoginApi::Refresh);
        auto pixmap = util::binToImage(svgContext, ui->btnRefresh->iconSize());
        QIcon icon(pixmap);
        ui->btnRefresh->setIcon(icon);
        ui->labelStrTip->setText(tr("Timeout, please refresh QR code"));
        ui->btnRefresh->show();
        ui->btnRefresh->raise();
        break;
    }
    case AbstractLoginApi::ScannedNoAck:
    {
        MLogI(svanilla::cLoginModule, "LoginStatus ScannedNoAck!");
        auto svgContext = realLogin->resource(AbstractLoginApi::WaitConfirm);
        auto pixmap = util::binToImage(svgContext, ui->btnRefresh->iconSize());
        QIcon icon(pixmap);
        ui->btnRefresh->setIcon(icon);
        ui->labelStrTip->setText(tr("Scanned, please confirm it in app"));
        ui->btnRefresh->show();
        ui->btnRefresh->raise();
        break;
    }
    case AbstractLoginApi::Success:
    {
        MLogI(svanilla::cLoginModule, "LoginStatus Success!");
        realLogin->loginSuccess();
        writeCookieToDb(*realLogin);
        QTimer::singleShot(1000, this, [this] {
            accept();
        });
        break;
    }
    case AbstractLoginApi::NoScan:
    default:
        break;
    }
}

void LoginDialog::slotBtnRefreshClicked()
{
    MLogI(svanilla::cLoginModule, "slotBtnRefreshClicked");
    if (m_status == AbstractLoginApi::Error || m_status == AbstractLoginApi::Timeout)
    {
        loadOrc();
    }
    else if (m_status == AbstractLoginApi::Success)
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
    if (obj == ui->labelOrc && event->type() == QEvent::Enter && m_status == AbstractLoginApi::NoScan)
    {
        ui->labelTooltip->show();
    }
    else if (obj == ui->labelTooltip && event->type() == QEvent::Leave && m_status == AbstractLoginApi::NoScan)
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
    auto realLogin = reinterpret_cast<AbstractLoginApi*>(&m_loginer->realLogin());
    auto svgContext = realLogin->resource(AbstractLoginApi::Background);
    auto pixmap = util::binToImage(svgContext, ui->labelBackground->size());
    ui->labelBackground->setPixmap(pixmap);
    ui->labelBackground->lower();

    svgContext = realLogin->resource(AbstractLoginApi::Confirmed);
    pixmap = util::binToImage(svgContext, ui->labelOrc->size());
    ui->labelOrc->setPixmap(pixmap);
    ui->labelOrc->raise();
    ui->labelOrc->installEventFilter(this);

    ui->btnRefresh->raise();
    svgContext = realLogin->resource(AbstractLoginApi::Loading);
    m_buffer.setData(reinterpret_cast<const char*>(svgContext.data()), svgContext.size());
    connect(&m_movie, &QMovie::frameChanged, this, [this]() {
        ui->btnRefresh->setIcon(QIcon(m_movie.currentPixmap()));
    });
    m_movie.setDevice(&m_buffer);
    m_movie.start();

    svgContext = realLogin->resource(AbstractLoginApi::Tip);
    pixmap = util::binToImage(svgContext, ui->labelTooltip->size());
    ui->labelTooltip->setPixmap(pixmap);
    ui->labelTooltip->raise();
    ui->labelTooltip->installEventFilter(this);
    ui->labelTooltip->hide();
}

void LoginDialog::loadOrc()
{
    m_status = AbstractLoginApi::Unknow;
    auto taskFunc = [this]() {
        auto realLogin = reinterpret_cast<AbstractLoginApi*>(&m_loginer->realLogin());
        return realLogin->getScanContext(m_context);
    };

    auto callback = [this](bool ret) {
        if (!ret || m_context.empty())
        {
            slotStatusChanged(AbstractLoginApi::Error);
            return;
        }
        QFile file(QString::fromStdString(m_context));
        file.open(QIODevice::ReadOnly);
        QByteArray data = file.readAll();
        std::vector<uint8_t> bin(data.begin(), data.end());
        auto pixmap = util::binToImage(bin, ui->labelOrc->size());
        ui->labelOrc->setPixmap(pixmap);
        m_status = AbstractLoginApi::NoScan;
        m_monitor->setLoginer(m_loginer);
        m_movie.stop();
        ui->btnRefresh->hide();
    };

    runTask(taskFunc, callback, this);
}

void LoginDialog::writeCookieToDb(const AbstractLogin& login)
{
    CookiesInfo cookiesInfo;
    cookiesInfo.pluginId = login.pluginId();
    cookiesInfo.updateTimestamp = std::chrono::system_clock::now().time_since_epoch().count();
    cookiesInfo.cookie = login.cookies();

    QDateTime earliest;
    network::CurlCookies cookies(cookiesInfo.cookie);
    for (const auto& domain : cookies.keys())
    {
        std::string expireTime = cookies.cookie(domain).expireDatatime();
        QDateTime dt = QDateTime::fromString(expireTime.c_str(), "ddd, dd-MMM-yyyy HH:mm:ss 'GMT'");
        dt.setTimeSpec(Qt::UTC);
        if (!earliest.isValid() || (dt.isValid() && dt < earliest))
        {
            earliest = dt;
            cookiesInfo.expires = expireTime;
        }
    }

    auto storage = sqlite::StorageManager::instance().cookiesInfoStorage();
    storage->insertOrUpdate(cookiesInfo);
}
