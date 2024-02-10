#include <QDir>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QWKCore/styleagent.h>
#include <QWKWidgets/widgetwindowagent.h>
#include "MainWindow.h"
#include "MainWindowlog.h"
#include "ClientUi/Setting/SettingPage.h"
#include "Sqlite/SQLiteManager.h"
#include "ClientUi/Download/DownloadingListWidget.h"
#include "ClientUi/VideoList/VideoWidget.h"
#include "SUI/windowbutton.h"

#include <QStackedWidget>
#include <QtWidgets/QLabel>
#include <ui_MainWindow.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , windowAgent(new QWK::WidgetWindowAgent(this))
    , windowBar(new WindowBar(this))
    , ui(new Ui::MainWindow)
{
    installWindowAgent();
    ui->setupUi(this);
    windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), "light");
    loadStyleSheet(Light);
    signalsAndSlots();
    resize(800, 600);
}

MainWindow::~MainWindow() = default;

void MainWindow::updateHomeMsg(const std::string& msg) const
{
    ui->homePage->updateMsg(msg);
}
void MainWindow::updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version) const
{
    ui->settingPage->updateAria2Version(version);
}
void MainWindow::updateDownloadStatus(const std::shared_ptr<aria2net::AriaTellStatus>& status) const
{
    ui->downloadTab->updateItem(status);
}
void MainWindow::AddDownloadTask(const std::string& gid) const
{
    ui->downloadTab->addTaskItem(gid);
}

void MainWindow::installWindowAgent()
{
    windowAgent->setup(this);
    windowAgent->setTitleBar(windowBar);
    windowAgent->setHitTestVisible(windowBar->GetHitWidget(), true);

    setMenuWidget(windowBar);

#ifdef Q_OS_WIN
    loadWindowsSystemButton();
#endif
}

void MainWindow::SearchUrl()
{
}

void MainWindow::signalsAndSlots()
{
    // tab bar btn click event to change stacked page
    connect(windowBar, &WindowBar::BarBtnClick, ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    connect(ui->stackedWidget, &QStackedWidget::currentChanged, [this](const int index) {
        if (index == 3)
        {
            Q_EMIT onSettingPage();
        }
    });
    // theme QRadioBtn toggle event to change theme
    // connect(ui->settingPage->defaultPage, &DefaultSetting::UpdateTheme, this, &MainWindow::SwitchTheme);
    // connect(ui->homePage, &HomePage::updateMsg, this, &MainWindow::updateHomeMsg);

    connect(ui->homePage, &HomePage::AddUri, this, &MainWindow::AddUri);
}

void MainWindow::loadStyleSheet(const Theme theme)
{
    if (!styleSheet().isEmpty() && theme == currentTheme)
    {
        return;
    }

    currentTheme = theme;

    QString styleSheetPath = theme == Dark ? ":/style/dark.qss" : ":/style/light.qss";
    QFile qss(styleSheetPath);
    if (qss.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        setStyleSheet(QString::fromUtf8(qss.readAll()));
        emit themeChanged();
        qss.close();
    }
}

bool MainWindow::event(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::WindowActivate:
    {
        auto menu = menuWidget();
        menu->setProperty("bar-active", true);
        style()->polish(menu);
        break;
    }
    case QEvent::WindowDeactivate:
    {
        auto menu = menuWidget();
        menu->setProperty("bar-active", false);
        style()->polish(menu);
        break;
    }
    default:
        break;
    }
    return QMainWindow::event(event);
}

static inline void emulateLeaveEvent(QWidget* widget)
{
    Q_ASSERT(widget);
    QTimer::singleShot(0, widget, [widget]() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        const QScreen* screen = widget->screen();
#else
        const QScreen *screen = widget->windowHandle()->screen();
#endif
        const QPoint globalPos = QCursor::pos(screen);
        if (!QRect(widget->mapToGlobal(QPoint{0, 0}), widget->size()).contains(globalPos))
        {
            QCoreApplication::postEvent(widget, new QEvent(QEvent::Leave));
            if (widget->testAttribute(Qt::WA_Hover))
            {
                const QPoint localPos = widget->mapFromGlobal(globalPos);
                const QPoint scenePos = widget->window()->mapFromGlobal(globalPos);
                static constexpr const auto oldPos = QPoint{};
                const Qt::KeyboardModifiers modifiers = QGuiApplication::keyboardModifiers();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
                const auto event = new QHoverEvent(QEvent::HoverLeave, scenePos, globalPos, oldPos, modifiers);
                Q_UNUSED(localPos);
#elif (QT_VERSION >= QT_VERSION_CHECK(6, 3, 0))
                const auto event =  new QHoverEvent(QEvent::HoverLeave, localPos, globalPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#else
                const auto event =  new QHoverEvent(QEvent::HoverLeave, localPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#endif
                QCoreApplication::postEvent(widget, event);
            }
        }
    });
}

void MainWindow::loadWindowsSystemButton()
{
    const auto minButton = new QPushButton();
    minButton->setObjectName(QStringLiteral("min-button"));
    minButton->setProperty("system-button", true);
    minButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    windowBar->setMinButton(minButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Minimize, minButton);

    const auto maxButton = new QPushButton();
    maxButton->setCheckable(true);
    maxButton->setObjectName(QStringLiteral("max-button"));
    maxButton->setProperty("system-button", true);
    maxButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    windowBar->setMaxButton(maxButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Maximize, maxButton);

    const auto closeButton = new QPushButton();
    closeButton->setObjectName(QStringLiteral("close-button"));
    closeButton->setProperty("system-button", true);
    closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    windowBar->setCloseButton(closeButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Close, closeButton);

    // 3. Adds simulated mouse events to the title bar buttons
    // Emulate Window system menu button behaviors

    connect(windowBar, &WindowBar::minimizeRequested, this, &QWidget::showMinimized);
    connect(windowBar, &WindowBar::maximizeRequested, this, [this, maxButton](bool max) {
        if (max)
        {
            showMaximized();
        }
        else
        {
            showNormal();
        }

        // It's a Qt issue that if a QAbstractButton::clicked triggers a window's maximization,
        // the button remains to be hovered until the mouse move. As a result, we need to
        // manually send leave events to the button.
        emulateLeaveEvent(maxButton);
    });
    connect(windowBar, &WindowBar::closeRequested, this, &QWidget::close);
}

void MainWindow::SwitchTheme(const int theme)
{
#ifdef Q_OS_WIN

#elif defined(Q_OS_MAC)
    if (theme == 0)
    {
        windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), "light");
        loadStyleSheet(Light);
    }
    else if (theme == 1)
    {
        windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), "dark");
        loadStyleSheet(Dark);
    }
    setProperty("custom-style", true);
    style()->polish(this);

#else

#endif
}
