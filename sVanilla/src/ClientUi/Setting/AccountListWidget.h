#pragma once
#include <QWidget>
#include <QListWidget>

#include <BaseVideoView.h>

class LoginProxy;

struct UserInfo;
namespace Ui
{
class AccountItemWidget;
}  // namespace Ui

class AccountItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountItemWidget(std::shared_ptr<LoginProxy> loginer, QListWidgetItem* item, QWidget* parent = nullptr);
    ~AccountItemWidget();

    void setUserInfo(UserInfo userInfo);
    void setListWidgetItem(QListWidgetItem* item);

signals:
    void signalUserInfo(UserInfo userInfo);
    void signalHistoryInfo(adapter::VideoView views);
    void signalLogout(bool bSucceed);

private:
    void setUi();
    void signalsAndSlots();
    void requestUserInfo();

private:
    Ui::AccountItemWidget* ui;
    QListWidgetItem* m_item;
    std::shared_ptr<LoginProxy> m_loginer;
};

class AccountListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit AccountListWidget(QWidget* parent = nullptr);

    void addLoginer(std::shared_ptr<LoginProxy> loginer);

signals:
    void signalHistoryInfo(adapter::VideoView views);
};
