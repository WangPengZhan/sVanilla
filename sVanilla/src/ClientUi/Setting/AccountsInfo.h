#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AccountsInfo;
}
QT_END_NAMESPACE

class AccountsInfo : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsInfo(QWidget* parent = nullptr);
    ~AccountsInfo();

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::AccountsInfo* ui;
};
