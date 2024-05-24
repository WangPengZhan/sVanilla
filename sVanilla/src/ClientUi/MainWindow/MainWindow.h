#pragma once

#include <QSystemTrayIcon>
#include <QMainWindow>

#include "WindowBar.h"
#include "Util/Setting.h"
#include "Aria2Net/Protocol/Protocol.h"
#include "ClientUi/Utils/UriProcess.h"

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
class StyleAgent;
}  // namespace QWK

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum BlurEffect
    {
        NoBlur,

        DWMBlur,
        AcrylicMaterial,
        Mica,
        MicaAlt,

        DarkBlur,
        LightBlur,
    };

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();
    void setUpShortcuts();
    void setLightTheme();
    void setDarkTheme();
    void setAutoTheme();
    void setBlurEffect(MainWindow::BlurEffect effect);
    void setTheme(int theme);
#ifndef __APPLE__
    void loadSystemButton();
#endif
    void installWindowAgent();
    void createTrayIcon();
    void setTrayIconVisible(int state);

    void startLoading(const UriProcess::UriInfo& uriInfo);

signals:
    void onSettingPage();
    void downloadBtnClick(const std::shared_ptr<Adapter::BaseVideoView>& videoView);

private:
    Ui::MainWindow* ui;
    QWK::WidgetWindowAgent* windowAgent;
    QWK::StyleAgent* styleAgent;
    WindowBar* windowBar;
    QSystemTrayIcon* systemTray;
    std::list<std::string> m_history;
    UriProcess* m_uriProcess;
#ifndef __APPLE__
    QString currentBlurEffect;
#endif
};
