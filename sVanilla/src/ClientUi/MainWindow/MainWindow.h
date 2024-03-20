#pragma once

#include "WindowBar.h"
#include "Aria2Net/Protocol/Protocol.h"
#include "Util/Setting.h"
#include <QtWidgets/QMainWindow>

namespace Adapter
{
struct BaseVideoView;
}

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
    void AddDownloadTask(const std::string& gid) const;
    void addVideoCard(const std::string& bvid) const;
    void updateVideoPage(const std::shared_ptr<Adapter::BaseVideoView>& videoView) const;
Q_SIGNALS:
    void AddUri(const std::string& uri);
    void onSettingPage();
    void themeChanged();
    void downloadBtnClick(const std::shared_ptr<Adapter::BaseVideoView>& videoView);

public slots:
    void SwitchTheme(int theme);
    void ClearVideo(bool flag);
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
