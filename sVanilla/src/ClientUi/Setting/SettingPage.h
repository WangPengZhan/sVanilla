#pragma once
#include <QPushButton>
#include <QStackedWidget>
#include "SUI/HorizonNavigation.h"
#include "DefaultSetting.h"

class SettingPage final : public QWidget{
Q_OBJECT
public:
    explicit SettingPage(QWidget* parent = nullptr);
    HorizonNavigation *horizonNavigation;
    QStackedWidget* settingStacked;

private:
    DefaultSetting* defaultPage;
};
