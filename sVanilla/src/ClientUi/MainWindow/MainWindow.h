#pragma once

#include <QtWidgets/QMainWindow>
#include <QEvent>
#include <QWidget>
#include <QStyle>

namespace QWK {
class WidgetWindowAgent;
class StyleAgent;
}

namespace Ui
{
class MainWindow;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    enum Theme {
        Dark,
        Light,
    };
    Q_ENUM(Theme)

Q_SIGNALS:
    void themeChanged();

protected:
    bool event(QEvent *event) override;

    void SearchUrl();

private:
    void SetUi();
    void SignalsAndSlots();

private:
//    Ui::MainWindow* ui;
    void installWindowAgent();
    void installStyleAgent();
    void loadStyleSheet(Theme theme);

    Theme currentTheme{};

    QWK::WidgetWindowAgent *windowAgent{};
    QWK::StyleAgent *styleAgent{};
};
