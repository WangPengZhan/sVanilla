
#include <QHBoxLayout>
#include <QtWidgets/QLabel>

#include "Aria2SettingItemWidget.h"

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
        m_checkBobx = new QCheckBox(this);
        m_checkBobx->setText("Enable");
        layout->addWidget(m_checkBobx);
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
    layout->setStretch(0,1);
    layout->setStretch(1,2);

}