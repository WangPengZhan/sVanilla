#pragma once

#include "WindowBar.h"
#include "Aria2Net/Protocol/Protocol.h"
#include "Util/Setting.h"
#include <QtWidgets/QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

namespace QWK
{
class WidgetWindowAgent;
}  // namespace QWK

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum Theme
    {
        Dark,
        Light,
    };
    Q_ENUM(Theme)

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void updateHomeMsg(const std::string& msg) const;
    void updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version) const;
    void updateDownloadStatus(const std::shared_ptr<aria2net::AriaTellStatus>& status) const;
    void AddDownloadTask(const std::string& gid) const;
Q_SIGNALS:
    void AddUri(const std::list<std::string>& uris);
    void onSettingPage();
    void themeChanged();

public slots:
    void SwitchTheme(int theme);

protected:
    bool event(QEvent* event) override;

    void SearchUrl();

private:
    void signalsAndSlots();

private:
    Ui::MainWindow* ui;
    void installWindowAgent();
    void loadStyleSheet(Theme theme);

    void loadWindowsSystemButton();
    Theme currentTheme{};

    QWK::WidgetWindowAgent* windowAgent;
    WindowBar* windowBar;
};
