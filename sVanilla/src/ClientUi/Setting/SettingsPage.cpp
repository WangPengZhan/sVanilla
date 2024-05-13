#include "Aria2Net/AriaClient/AriaClient.h"
#include "ThreadPool/ThreadPool.h"
#include "SettingsPage.h"
#include "ui_SettingsPage.h"

SettingsPage::SettingsPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SettingsPage)
{
    ui->setupUi(this);

    const QStringList horizonNavigationBtn{tr("Defalut"), tr("Account"), tr("Advanced"), tr("Plugins"), tr("Authors"), tr("License"), tr("About")};
    ui->horizonNavigation->setItemList(horizonNavigationBtn);
    ui->horizonNavigation->setUseIcon(false);
    ui->horizonNavigation->setColumnWidth(80);
    connect(ui->horizonNavigation, &Vanilla::ToggleButton::currentItemChanged, ui->settingStackedPage, &QStackedWidget::setCurrentIndex);
    connect(ui->defaultSettings, &DefaultSettings::UpdateTheme, this, &SettingsPage::UpdateTheme);
}

void SettingsPage::connectAria2Server()
{
    auto taskFunc = [this]() {
        return aria2net::AriaClient::globalClient().GetAriaVersionAsync();
    };
    const auto task = std::make_shared<TemplateSignalReturnTask<decltype(taskFunc)>>(taskFunc);
    connect(task.get(), &SignalReturnTask::result, this, [this](const std::any& res) {
        try
        {
            auto result = std::any_cast<aria2net::AriaVersion>(res);
            if (!result.id.empty() && result.error.message.empty())
            {
                isConnect = true;
            }
            updateAria2Version(std::make_shared<aria2net::AriaVersion>(result));
        }
        catch (const std::bad_any_cast& e)
        {
            ui->defaultSettings->updateStatus(e.what());
        }
    });
    ThreadPool::instance().enqueue(task);
}

void SettingsPage::updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version) const
{
    ui->defaultSettings->updateAria2Version(version);
}
