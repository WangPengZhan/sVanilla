#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AdvanceSettings;
}
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

private:
    Ui::AdvanceSettings* ui;
};
