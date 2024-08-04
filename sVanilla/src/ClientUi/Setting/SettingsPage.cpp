#include "Aria2Net/AriaClient/AriaClient.h"
#include "ThreadPool/ThreadPool.h"
#include "SettingsPage.h"
#include "ui_SettingsPage.h"
#include "Utils/RunTask.h"
#include "AccountListWidget.h"

SettingsPage::SettingsPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SettingsPage)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

void SettingsPage::connectAria2Server()
{
    auto taskFunc = [this]() {
        return aria2net::AriaClient::globalClient().GetAriaVersionAsync();
    };
    auto callback = [this](const aria2net::AriaVersion& result) {
        if (!result.id.empty() && result.error.message.empty())
        {
            isConnect = true;
        }
        updateAria2Version(std::make_shared<aria2net::AriaVersion>(result));
    };
    runTask(taskFunc, callback, this);
}

Qt::CheckState SettingsPage::getTrayState() const
{
    return ui->defaultSettings->getTrayState();
}

Qt::CheckState SettingsPage::isEnableMinimizeTray() const
{
    return ui->defaultSettings->isEnableMinimizeTray();
}

Qt::CheckState SettingsPage::isSaveMainWindow() const
{
    return ui->defaultSettings->isSaveMainWindow();
}

void SettingsPage::logined(UserInfo userInfo)
{
    ui->accountsInfo->addUserInfo(userInfo);
}

void SettingsPage::loginSucceed(std::shared_ptr<AbstractLogin> loginer)
{
    ui->settingStackedPage->setCurrentWidget(ui->accountsInfo);
    ui->horizonNavigation->setCurrentIndex(ui->settingStackedPage->currentIndex());
    ui->accountsInfo->addUserInfo(loginer);
}

void SettingsPage::setUi()
{
    const QStringList horizonNavigationBtn{tr("Default"), tr("Account"), tr("Advanced"), tr("Plugins"), tr("About")};
    ui->horizonNavigation->setItemList(horizonNavigationBtn);
    const QStringList iconList({QStringLiteral(":/icon/default.svg"), QStringLiteral(":/icon/account.svg"), QStringLiteral(":/icon/advanced.svg"),
                                QStringLiteral(":/icon/plugin.svg"), QStringLiteral(":/icon/about.svg")});
    ui->horizonNavigation->setIconList(iconList);
    constexpr int settingNavWidth = 120;
    ui->horizonNavigation->setColumnWidth(settingNavWidth);
    ui->horizonNavigation->setFixedHeight(30);
}

void SettingsPage::signalsAndSlots()
{
    connect(ui->horizonNavigation, &Vanilla::ToggleButton::currentItemChanged, ui->settingStackedPage, &QStackedWidget::setCurrentIndex);
    connect(ui->defaultSettings, &DefaultSettings::updateTheme, this, &SettingsPage::updateTheme);
    connect(ui->defaultSettings, &DefaultSettings::enableTray, this, &SettingsPage::enableTray);
    connect(ui->accountsInfo->listWidgetAccount(), &AccountListWidget::signalHistoryInfo, this, &SettingsPage::signalHistoryInfo);
}

void SettingsPage::updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version) const
{
    ui->defaultSettings->updateAria2Version(version);
}
