#include <QTextEdit>
#include <QDateTime>

#include "SetCookieDialog.h"
#include "ui_SetCookieDialog.h"
#include "BaseQt/Utility.h"
#include "Plugin/PluginManager.h"
#include "Storage/SearchHistoryStorage.h"
#include "Storage/StorageManager.h"
#include "Storage/CookiesInfoStorage.h"
#include "Login/LoginMonitor.h"
#include "NetWork/CurlCpp/CurlCookies.h"
#include "NetWork/CurlCpp/CurlCookie.h"
#include "SUI/Tips/ToastTip.h"
#include "Utils/RunTask.h"
#include "LoginProxy.h"
#include "ClientLog.h"
#include "const_string.h"

namespace
{
bool isValidCookie(const std::string& cookie)
{
    std::istringstream ss(cookie);
    std::string token;
    while (std::getline(ss, token, ';'))
    {
        size_t pos = token.find('=');
        if (pos == std::string::npos)
        {
            return false;
        }
        std::string key = token.substr(0, pos);
        std::string value = token.substr(pos + 1);

        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        if (key.empty())
        {
            return false;
        }
    }
    return true;
}
}  // namespace

SetCookieDialog::SetCookieDialog(std::vector<std::shared_ptr<plugin::IPlugin>> plugins, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::SetCookieDialog)
    , m_plugins(std::move(plugins))
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
    pluginChanged();
}

std::shared_ptr<LoginProxy> SetCookieDialog::loginer() const
{
    return m_loginer;
}

SetCookieDialog::~SetCookieDialog()
{
    delete ui;
}

void SetCookieDialog::signalsAndSlots()
{
    connect(ui->btnOk, &QPushButton::clicked, this, &SetCookieDialog::slotAccept);
    connect(ui->btnNo, &QPushButton::clicked, this, &SetCookieDialog::reject);
    connect(ui->comboBoxPlugins, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SetCookieDialog::pluginChanged);
}

void SetCookieDialog::pluginChanged()
{
    std::shared_ptr<plugin::IPlugin> selectedPlugin;
    auto pluginid = ui->comboBoxPlugins->currentData().toInt();
    for (const auto& plugin : m_plugins)
    {
        if (plugin->pluginMessage().pluginId == pluginid)
        {
            selectedPlugin = plugin;
            break;
        }
    }

    setPluginInfo(selectedPlugin);
}

void SetCookieDialog::setPluginInfo(std::shared_ptr<plugin::IPlugin> plugin)
{
    if (!plugin)
    {
        return;
    }

    auto loginer = plugin->loginer();
    ui->labelWebsiteContent->setText(QString::fromStdString(plugin->pluginMessage().domain));
    ui->comboBoxDomian->clear();
    ui->comboBoxDomian->addItem(QString::fromStdString(loginer.domain()));
    auto cookies = loginer.cookies();
    auto cookie = network::CurlCookies(cookies).cookie(loginer.domain());
    ui->textEditCookie->setPlainText(QString::fromStdString(std::string(cookie)));
}

void SetCookieDialog::setUi()
{
    for (const auto& plugin : m_plugins)
    {
#ifdef _DEBUG
        if (true)
#else
        if (!plugin->loginer().isLoggedIn())
#endif
        {
            const auto& pluginMessage = plugin->pluginMessage();
            ui->comboBoxPlugins->addItem(QString::fromStdString(pluginMessage.name), QVariant(pluginMessage.pluginId));
        }
    }
}

void SetCookieDialog::writeCookieToDb(const AbstractLogin& login)
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

void SetCookieDialog::slotAccept()
{
    if (ui->textEditCookie->toPlainText().isEmpty())
    {
        ToastTip::showTip(tr("Cookie can not be empty"), ToastTip::Warn);
        return;
    }

    if (!isValidCookie(ui->textEditCookie->toPlainText().toStdString()))
    {
        ToastTip::showTip(tr("Cookie format error"), ToastTip::Warn);
        return;
    }

    std::shared_ptr<plugin::IPlugin> selectedPlugin;
    auto pluginid = ui->comboBoxPlugins->currentData().toInt();
    for (const auto& plugin : m_plugins)
    {
        if (plugin->pluginMessage().pluginId == pluginid)
        {
            selectedPlugin = plugin;
            break;
        }
    }

    if (!selectedPlugin)
    {
        ToastTip::showTip(tr("Please select a plugin"), ToastTip::Warn);
        return;
    }

    auto loginer = selectedPlugin->loginer();
    std::string cookies = loginer.domain() + ": " + "domain=" + loginer.domain() + "; " + ui->textEditCookie->toPlainText().toStdString() + "\r\n";
    loginer.setCookies(cookies);
    writeCookieToDb(loginer);
    m_loginer = std::make_shared<LoginProxy>(loginer);
    accept();
}
