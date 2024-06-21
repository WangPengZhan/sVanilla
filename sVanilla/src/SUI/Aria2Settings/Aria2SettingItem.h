#pragma once

#include <QVariant>
#include <QMetaType>
#include <QPainter>


class Aria2SettingItem
{
public:
    enum Unit
    {
        String,
        Number,
        Bool
    };
    explicit Aria2SettingItem() = default;
    explicit Aria2SettingItem(QString key, QString value, Unit unit = String)
        : m_key(std::move(key))
        , m_value(std::move(value))
        , m_unit(unit){};

    QString key()
    {
        return m_key;
    };
    QString value()
    {
        return m_value;
    };
    Unit unit()
    {
        return m_unit;
    };

    void setKey(QString key)
    {
        m_key = std::move(key);
    };
    void setValue(QString value)
    {
        m_value = std::move(value);
    };
    void setUnit(Unit unit)
    {
        m_unit = unit;
    };

    void setWidget(QWidget* widget)
    {
        m_widget = widget;
    }

    QWidget* getWidget()
    {
        return m_widget;
    }

private:
    QString m_key;
    QString m_value;
    Unit m_unit = String;
    QWidget* m_widget = nullptr;
};

