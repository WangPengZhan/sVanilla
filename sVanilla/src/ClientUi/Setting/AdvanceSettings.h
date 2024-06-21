#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AdvanceSettings;
}
class QButtonGroup;
QT_END_NAMESPACE

class AdvanceSettings : public QWidget
{
    Q_OBJECT

public:
    explicit AdvanceSettings(QWidget* parent = nullptr);
    ~AdvanceSettings();

private:
    void setUi();
    void signalsAndSlots();
    void setAria2SettingVisiable(bool visiable) const;

private:
    Ui::AdvanceSettings* ui;
    QButtonGroup* m_videoOrderGroup;
    QButtonGroup* m_videoDefaultGroup;
};
