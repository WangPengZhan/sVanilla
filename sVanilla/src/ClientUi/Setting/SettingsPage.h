#pragma once

#include <QWidget>

#include "Aria2Net/Protocol/Protocol.h"
#include "Login/login.h"
#include "ClientUi/Adapter/BaseVideoView.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class SettingsPage;
}
QT_END_NAMESPACE

class AbstractLogin;

class SettingsPage final : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsPage(QWidget* parent = nullptr);

    void updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version) const;
    void connectAria2Server();

    [[nodiscard]] Qt::CheckState getTrayState() const;
    [[nodiscard]] Qt::CheckState isEnableMinimizeTray() const;
    [[nodiscard]] Qt::CheckState isSaveMainWindow() const;

    void logined(UserInfo userInfo);
    void loginSucceed(std::shared_ptr<AbstractLogin> loginer);

private:
    void setUi();
    void signalsAndSlots();

signals:
    void updateTheme(int theme);
    void enableTray(int);
    void signalHistoryInfo(Adapter::Views views);

public:
    bool isConnect = false;

private:
    Ui::SettingsPage* ui;
};
