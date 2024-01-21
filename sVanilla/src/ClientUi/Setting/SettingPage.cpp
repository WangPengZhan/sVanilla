#include "SettingPage.h"
#include <QVBoxLayout>

SettingPage::SettingPage(QWidget* parent)
    : QWidget(parent)
    , horizonNavigation(new HorizonNavigation)
    , settingStacked(new QStackedWidget)
    , defaultPage(new DefaultSetting)
    , accountPage(new AccountSetting)
    , advancePage(new AdvanceSetting)
{
    const auto layout = new QVBoxLayout();
    layout->setContentsMargins(20, 20, 0, 0);
    this->setLayout(layout);
    QStringList horizonNavigationBtn;
    horizonNavigationBtn << "Default"
                         << "Account"
                         << "Advanced";
    horizonNavigation->addItems(horizonNavigationBtn);
    layout->addWidget(horizonNavigation);

    layout->addWidget(settingStacked);
    settingStacked->insertWidget(0, defaultPage);
    settingStacked->insertWidget(1, accountPage);
    settingStacked->insertWidget(2, advancePage);

    connect(horizonNavigation, &HorizonNavigation::currentItemChanged, this, [this](const int index) {
        settingStacked->setCurrentIndex(index);
    });
}
