#pragma once
#include <QWidget>
#include <QListWidget>

#include "ClientUi/Adapter/BaseVideoView.h"

class AbstractLogin;

struct UserInfo;
namespace Ui
{
class AccountItemWidget;
}  // namespace Ui

class AccountItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountItemWidget(QWidget* parent = nullptr);
    explicit AccountItemWidget(std::shared_ptr<AbstractLogin> loginer, QListWidgetItem* item, QWidget* parent = nullptr);
    explicit AccountItemWidget(UserInfo userInfo, QListWidgetItem* item, QWidget* parent = nullptr);
    ~AccountItemWidget();

    void setLoginer(std::shared_ptr<AbstractLogin> loginer);
    std::shared_ptr<AbstractLogin> loginer() const;

    void setUserInfo(UserInfo userInfo);
    void setListWidgetItem(QListWidgetItem* item);

signals:
    void signalUserInfo(UserInfo userInfo);
    void signalHistoryInfo(Adapter::Views views);
    void signalLogout(bool bSucceed);

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::AccountItemWidget* ui;
    QListWidgetItem* m_item;
    std::shared_ptr<AbstractLogin> m_loginer;
};

class AccountListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit AccountListWidget(QWidget* parent = nullptr);

    void addLoginer(std::shared_ptr<AbstractLogin> loginer);

    void addUserInfo(const UserInfo& license);
    void addUserInfos(const std::vector<UserInfo>& licenses);

signals:
    void signalHistoryInfo(Adapter::Views views);
};
