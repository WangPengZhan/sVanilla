#include "SettingPage.h"
#include "ui_SettingPage.h"
#include "Aria2Net/AriaClient/AriaClient.h"
#include "ThreadPool/ThreadPool.h"

SettingPage::SettingPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SettingPage)
{
    ui->setupUi(this);

    const QStringList horizonNavigationBtn({QStringLiteral("Defalut"), QStringLiteral("Account"), QStringLiteral("Advanced")});
    ui->horizonNavigation->setItemList(horizonNavigationBtn);
    ui->horizonNavigation->setUseIcon(false);
    ui->horizonNavigation->setColumnWidth(100);
    connect(ui->horizonNavigation, &VanillaStyle::ToggleButton::currentItemChanged, ui->settingStackedPage, &QStackedWidget::setCurrentIndex);
    connect(ui->defaultSetting, &DefaultSetting::UpdateTheme, this, &SettingPage::UpdateTheme);

}
void SettingPage::connectAria2Server()
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
            ui->defaultSetting->updateStatus(e.what());
        }
    });
    ThreadPool::instance().enqueue(task);
}

void SettingPage::updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version) const
{
    ui->defaultSetting->updateAria2Version(version);
}
