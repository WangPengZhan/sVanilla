#include "AccountsInfo.h"
#include "ui_AccountsInfo.h"

#include "MainWindow/SApplication.h"
#include "Plugin/PluginManager.h"
#include "LoginProxy.h"

AccountsInfo::AccountsInfo(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AccountsInfo)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
    init();
}

AccountsInfo::~AccountsInfo()
{
    delete ui;
}

void AccountsInfo::addUserInfo(std::shared_ptr<LoginProxy> loginer)
{
    ui->listWidgetAccount->addLoginer(loginer);
}

AccountListWidget* AccountsInfo::listWidgetAccount() const
{
    return ui->listWidgetAccount;
}

void AccountsInfo::setUi()
{
}

void AccountsInfo::signalsAndSlots()
{
    connect(sApp, &SApplication::pluginsLoaded, this, [this](bool) {
        init();
    });
}

void AccountsInfo::init()
{
    ui->listWidgetAccount->clear();
    auto plugins = sApp->pluginManager().pluginsSnapshot();
    for (auto& plugin : plugins)
    {
        if (plugin->loginer().isLoggedIn())
        {
            addUserInfo(std::make_shared<LoginProxy>(plugin->loginer()));
        }
    }
}
