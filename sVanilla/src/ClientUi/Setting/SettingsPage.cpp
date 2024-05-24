#include "Aria2Net/AriaClient/AriaClient.h"
#include "ThreadPool/ThreadPool.h"
#include "SettingsPage.h"
#include "ui_SettingsPage.h"
#include "ClientUi/Utils/RunTask.h"

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

void SettingsPage::setUi()
{
    const QStringList horizonNavigationBtn{tr("Defalut"), tr("Account"), tr("Advanced"), tr("Plugins"), tr("Authors"), tr("License"), tr("About")};
    ui->horizonNavigation->setItemList(horizonNavigationBtn);
    ui->horizonNavigation->setUseIcon(false);
    constexpr int settingNavWidth = 80;
    ui->horizonNavigation->setColumnWidth(settingNavWidth);
}

void SettingsPage::signalsAndSlots()
{
    connect(ui->horizonNavigation, &Vanilla::ToggleButton::currentItemChanged, ui->settingStackedPage, &QStackedWidget::setCurrentIndex);
    connect(ui->defaultSettings, &DefaultSettings::updateTheme, this, &SettingsPage::updateTheme);
    connect(ui->defaultSettings, &DefaultSettings::enableTray, this, &SettingsPage::enableTray);
}

void SettingsPage::updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version) const
{
    ui->defaultSettings->updateAria2Version(version);
}