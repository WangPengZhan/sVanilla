#include "SettingPage.h"
#include <QVBoxLayout>

SettingPage::SettingPage(QWidget* parent)
    : QWidget(parent),
      horizonNavigation(new HorizonNavigation),
      settingStacked(new QStackedWidget),
      defaultPage(new DefaultSetting)
{
    const auto layout = new QVBoxLayout();
    layout->setContentsMargins(20, 20, 20, 10);
    this->setLayout(layout);
    QStringList horizonNavigationBtn;
    horizonNavigationBtn << "Default"
                         << "Account"
                         << "Advanced";
    horizonNavigation->addItems(horizonNavigationBtn);
    layout->addWidget(horizonNavigation);

    layout->addWidget(settingStacked);
    settingStacked->insertWidget(0, defaultPage);
}
