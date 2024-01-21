#pragma once
#include <QPushButton>
#include <QStackedWidget>
#include "SUI/HorizonNavigation.h"
#include "DefaultSetting.h"
#include "AccountSetting.h"
#include "AdvanceSetting.h"

class SettingPage final : public QWidget{
Q_OBJECT
public:
    explicit SettingPage(QWidget* parent = nullptr);
    HorizonNavigation *horizonNavigation;
    QStackedWidget* settingStacked;

    DefaultSetting* defaultPage;
    AccountSetting* accountPage;
    AdvanceSetting* advancePage;
};
