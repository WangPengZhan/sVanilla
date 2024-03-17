#include "SettingPage.h"
#include "ui_SettingPage.h"

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

void SettingPage::updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version)
{
    ui->defaultSetting->updateAria2Version(version);
}
