#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class SettingPage;
}
QT_END_NAMESPACE

class SettingPage final : public QWidget
{
    Q_OBJECT
public:
    explicit SettingPage(QWidget* parent = nullptr);

private:
    Ui::SettingPage* ui;
};
