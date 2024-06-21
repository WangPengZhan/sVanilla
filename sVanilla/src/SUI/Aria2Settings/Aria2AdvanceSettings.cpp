#include <QSettings>

#include "Aria2AdvanceSettings.h"

#include "Aria2SettingItemWidget.h"

bool isInteger(const QString& str)
{
    if (str.isEmpty() || ((!str.at(0).isDigit()) && (str.at(0) != '-') && (str.at(0) != '+')) && !str.at(str.size() - 1).isDigit())
    {
        return false;
    }
    bool ok;
    str.toInt(&ok);
    return ok;
}

Aria2AdvanceSettings::Aria2AdvanceSettings(QWidget* parent)
    : QListWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSettings();
    setSize();
}

void Aria2AdvanceSettings::setSettings()
{
    const auto aria2Config = SingleConfig::instance().aria2AdvanceConfig();
    const auto allKeys = aria2Config->childKeys();
    std::vector<Aria2SettingItem> ariaSettings;
    for (const auto& key : allKeys)
    {
        const auto value = aria2Config->value(key).toString();
        Aria2SettingItem::Unit unit;
        if (value.toLower() == "true" || value.toLower() == "false")
        {
            unit = Aria2SettingItem::Bool;
        }
        else if (isInteger(value))
        {
            unit = Aria2SettingItem::Number;
        }
        else
        {
            unit = Aria2SettingItem::String;
        }
        const auto settingItem = std::make_shared<Aria2SettingItem>(key, value, unit);
        const auto widget = new Aria2SettingItemWidget(this, settingItem);
        const auto widgetItem = new QListWidgetItem(this);
        widgetItem->setSizeHint(widget->sizeHint());
        setItemWidget(widgetItem, widget);
    }
}

void Aria2AdvanceSettings::setSize()
{
    int totalHeight = 0;
    for (int i =0; i < count(); ++i)
    {
        totalHeight += sizeHintForRow(i);
    }
    setFixedHeight(totalHeight);
}
