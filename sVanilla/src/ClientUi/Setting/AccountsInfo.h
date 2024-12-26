#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AccountsInfo;
}
QT_END_NAMESPACE

class LoginProxy;
class AccountListWidget;

class AccountsInfo : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsInfo(QWidget* parent = nullptr);
    ~AccountsInfo();

    void addUserInfo(std::shared_ptr<LoginProxy> loginer);
    [[nodiscard]] AccountListWidget* listWidgetAccount() const;

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::AccountsInfo* ui;
};
