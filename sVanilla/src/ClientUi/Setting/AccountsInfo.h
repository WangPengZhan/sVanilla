#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AccountsInfo;
}
QT_END_NAMESPACE

struct UserInfo;
class AbstractLogin;
class AccountListWidget;

class AccountsInfo : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsInfo(QWidget* parent = nullptr);
    ~AccountsInfo();

    void addUserInfo(const UserInfo& userInfo);
    void addUserInfo(std::shared_ptr<AbstractLogin> loginer);
    [[nodiscard]] AccountListWidget* listWidgetAccount() const;

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::AccountsInfo* ui;
};
