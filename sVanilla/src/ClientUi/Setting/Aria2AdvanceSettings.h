#pragma once
#include <QListWidget>
#include <QCheckBox>
#include <QSpinBox>

#include "Config/SingleConfig.h"

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
    explicit Aria2SettingItem(QString key, QString value, Unit unit = String);

    QString key();
    QString value();
    Unit unit() const;
    void setKey(QString key);
    void setValue(QString value);
    void setUnit(Unit unit);
    void setWidget(QWidget* widget);
    QWidget* getWidget() const;

private:
    QString m_key;
    QString m_value;
    Unit m_unit = String;
    QWidget* m_widget = nullptr;
};

class Aria2SettingItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit Aria2SettingItemWidget(QWidget* parent);
    explicit Aria2SettingItemWidget(QWidget* parent, const std::shared_ptr<Aria2SettingItem>& item);

    const std::shared_ptr<Aria2SettingItem>& getSettingItem();

private:
    void setUi();

private:
    std::shared_ptr<Aria2SettingItem> m_item;
    QLineEdit* m_lineEdit = nullptr;
    QSpinBox* m_spinBox = nullptr;
    QCheckBox* m_checkBox = nullptr;
};

class Aria2AdvanceSettings : public QListWidget
{
    Q_OBJECT
public:
    explicit Aria2AdvanceSettings(QWidget* parent = nullptr);

    void setSettings(std::shared_ptr<QSettings> aria2Config);

private:
    void setSize();

private:
    std::vector<std::shared_ptr<Aria2SettingItem>> m_settings;
};
