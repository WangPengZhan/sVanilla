#include "SettingPage.h"
#include "ui_SettingPage.h"

SettingPage::SettingPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SettingPage)
{
    ui->setupUi(this);

    QStringList horizonNavigationBtn;
    horizonNavigationBtn << QStringLiteral("Defalut")
                         << QStringLiteral("Account")
                         << QStringLiteral("Advanced");
    ui->horizonNavigation->addItems(horizonNavigationBtn);

    connect(ui->horizonNavigation, &HorizonNavigation::currentItemChanged, this, [this](const int index) {
        ui->settingStackedPage->setCurrentIndex(index);
    });
}
