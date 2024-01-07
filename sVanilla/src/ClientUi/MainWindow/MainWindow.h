#pragma once

#include "ClientUi/Download/DownloadingListWidget.h"
#include "ClientUi/Home//HomePage.h"
#include "ClientUi/Setting/SettingPage.h"
#include "WindowBar.h"
#include "Util/Setting.h"
#include <QtWidgets/QMainWindow>
#include <QStackedWidget>

namespace QWK
{
class WidgetWindowAgent;
class StyleAgent;
}  // namespace QWK

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    enum Theme
    {
        Dark,
        Light,
    };
    Q_ENUM(Theme)

Q_SIGNALS:
    void themeChanged();

protected:
    bool event(QEvent* event) override;

    void SearchUrl();

private:
    void SetUi();
    void SignalsAndSlots();

private:
    //    Ui::MainWindow* ui;
    void installWindowAgent();
    void installStyleAgent();
    void loadStyleSheet(Theme theme);

    void loadWindowsSystemButton();
    Theme currentTheme{};

    QWK::WidgetWindowAgent* windowAgent;
    QWK::StyleAgent* styleAgent;
    WindowBar* windowBar;
    QStackedWidget* stackedPage;
    HomePage* homePage;
    QWidget* galleryPage;
    DownloadingListWidget* downloadPage;
    SettingPage* settingPage;

};
