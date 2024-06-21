#pragma once
#include <QListWidget>

#include "Aria2SettingItem.h"
#include "ClientUi/Config/SingleConfig.h"

class Aria2AdvanceSettings : public QListWidget
{
    Q_OBJECT
public:
    explicit Aria2AdvanceSettings(QWidget* parent = nullptr);

private:
    void setSettings();
    void setSize();

private:
    std::vector<std::shared_ptr<Aria2SettingItem>> m_settings;

};
