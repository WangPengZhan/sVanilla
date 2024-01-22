#pragma once

#include "WindowBar.h"
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
    Ui::MainWindow* ui;
    void installWindowAgent();
    void loadStyleSheet(Theme theme);

    void loadWindowsSystemButton();
    Theme currentTheme{};

    QWK::WidgetWindowAgent* windowAgent;
    WindowBar* windowBar;
};
