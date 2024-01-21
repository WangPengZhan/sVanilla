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

Q_SIGNALS:
    void themeChanged();

public slots:
    void SwitchTheme(int theme);

protected:
    bool event(QEvent* event) override;

    void SearchUrl();

private:
    void setUi();
    void signalsAndSlots();

private:
    //    Ui::MainWindow* ui;
    void installWindowAgent();
    void loadStyleSheet(Theme theme);

    void loadWindowsSystemButton();
    Theme currentTheme{};

    QWK::WidgetWindowAgent* windowAgent;
    WindowBar* windowBar;
    QStackedWidget* stackedPage;
    HomePage* homePage;
    QWidget* galleryPage;
    DownloadingListWidget* downloadPage;
    SettingPage* settingPage;
};
