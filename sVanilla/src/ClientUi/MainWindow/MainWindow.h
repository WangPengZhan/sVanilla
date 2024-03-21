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

private:
    void setLightTheme();
    void setDarkTheme();
    void setAutoTheme();
    void setBlurEffect(MainWindow::BlurEffect effect);
    Q_SLOT void setTheme(int theme);
#ifndef __APPLE__
    void loadSystemButton();
    QString currentBlurEffect;
#endif

public:
    void updateHomeMsg(const std::string& msg) const;
    void updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version) const;
    void AddDownloadTask(const std::string& gid) const;
    void updateVideoPage(const std::shared_ptr<Adapter::BaseVideoView>& videoView) const;
Q_SIGNALS:
    void AddUri(const std::string& uri);
    void onSettingPage();
    void downloadBtnClick(const std::shared_ptr<Adapter::BaseVideoView>& videoView);

public slots:
    void ClearVideo(bool flag);
protected:
    bool event(QEvent* event) override;
    void SearchUrl();

private:
    void signalsAndSlots();

private:
    Ui::MainWindow* ui;
    void installWindowAgent();
    QWK::WidgetWindowAgent* windowAgent;
    QWK::StyleAgent* styleAgent;
    WindowBar* windowBar;
};
