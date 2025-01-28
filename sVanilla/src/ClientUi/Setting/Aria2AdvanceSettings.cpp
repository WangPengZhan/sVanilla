#include <QSettings>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

#include "Aria2AdvanceSettings.h"

namespace
{
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
}  // namespace

Aria2SettingItem::Aria2SettingItem(QString key, QString value, Unit unit)
    : m_key(std::move(key))
    , m_value(std::move(value))
    , m_unit(unit)
{
}

QString Aria2SettingItem::key()
{
    return m_key;
}

QString Aria2SettingItem::value()
{
    return m_value;
}

Aria2SettingItem::Unit Aria2SettingItem::unit() const
{
    return m_unit;
}

void Aria2SettingItem::setKey(QString key)
{
    m_key = std::move(key);
}

void Aria2SettingItem::setValue(QString value)
{
    m_value = std::move(value);
}

void Aria2SettingItem::setUnit(Unit unit)
{
    m_unit = unit;
}

void Aria2SettingItem::setWidget(QWidget* widget)
{
    m_widget = widget;
}

QWidget* Aria2SettingItem::getWidget() const
{
    return m_widget;
}

Aria2SettingItemWidget::Aria2SettingItemWidget(QWidget* parent)
    : QWidget(parent)
{
}

Aria2SettingItemWidget::Aria2SettingItemWidget(QWidget* parent, const std::shared_ptr<Aria2SettingItem>& item)
    : QWidget(parent)
    , m_item(item)
{
    setUi();
}

const std::shared_ptr<Aria2SettingItem>& Aria2SettingItemWidget::getSettingItem()
{
    return m_item;
}

void Aria2SettingItemWidget::setUi()
{
    const auto layout = new QHBoxLayout;
    setLayout(layout);
    const auto nameWidget = new QLabel(this);
    nameWidget->setText(m_item->key());
    layout->addWidget(nameWidget);
    switch (m_item->unit())
    {
    case Aria2SettingItem::Bool:
    {
        m_checkBox = new QCheckBox(this);
        m_checkBox->setText("Enable");
        layout->addWidget(m_checkBox);
        break;
    }
    case Aria2SettingItem::String:
    {
        m_lineEdit = new QLineEdit(this);
        m_lineEdit->setText(m_item->value());
        layout->addWidget(m_lineEdit);
        break;
    }
    case Aria2SettingItem::Number:
    {
        m_spinBox = new QSpinBox(this);
        m_spinBox->setValue(std::stoi(m_item->value().toStdString()));
        layout->addWidget(m_spinBox);
        break;
    }
    }
    layout->setStretch(0, 1);
    layout->setStretch(1, 2);
}

Aria2AdvanceSettings::Aria2AdvanceSettings(QWidget* parent)
    : QListWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSize();
}

void Aria2AdvanceSettings::setSettings(std::shared_ptr<QSettings> aria2Config)
{
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
    for (int i = 0; i < count(); ++i)
    {
        totalHeight += sizeHintForRow(i);
    }
    setFixedHeight(totalHeight);
}
