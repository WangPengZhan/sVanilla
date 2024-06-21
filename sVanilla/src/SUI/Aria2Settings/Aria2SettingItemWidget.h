#pragma once
#include "Aria2SettingItem.h"

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>

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
    QCheckBox* m_checkBobx = nullptr;
};
