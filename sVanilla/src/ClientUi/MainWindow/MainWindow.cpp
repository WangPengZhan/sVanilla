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
#include "SUI/Tips/Toast.h"
#include <QStackedWidget>
#include <QtWidgets/QLabel>
#include <ui_MainWindow.h>

#include "VanillaStyle/Style.h"
#include "VanillaStyle/Style/VanillaStyle.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , windowAgent(new QWK::WidgetWindowAgent(this))
    , styleAgent(new QWK::StyleAgent(this))
    , windowBar(new WindowBar(this))
{
    installWindowAgent();
    ui->setupUi(this);
    windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), "light");
    signalsAndSlots();
    resize(800, 600);
    ui->downloadPage->addTaskItem({"https://testfile.org/files-5GB-zip"},
                                  {"https://testfile.org/files-5GB-zip"}, "local");
    setLightTheme();

    ui->downloadPage->addTaskItem({"https://testfile.org/files-5GB-zip"},
                                  {"https://testfile.org/files-5GB-zip"}, "local");
}

MainWindow::~MainWindow() = default;

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
    connect(ui->settingPage, &SettingPage::UpdateTheme, this, &MainWindow::setTheme);
    connect(ui->homePage, &HomePage::HasAdded, this, &MainWindow::ClearVideo);
    connect(ui->homePage, &HomePage::AddUri, this, &MainWindow::AddUri);
    connect(ui->VideoPage, &VideoWidget::downloadBtnClick, this, &MainWindow::downloadBtnClick);
}

void MainWindow::setLightTheme()
{
    VanillaStyle::Style::setStyleFromName("LightVanillaStyle");
#ifdef Q_OS_WIN
    setBlurEffect(AcrylicMaterial);
#elif defined(Q_OS_MAC)
    setBlurEffect(LightBlur);
#endif
}

void MainWindow::setDarkTheme()
{
    VanillaStyle::Style::setStyleFromName(QStringLiteral("DarkVanillaStyle"));
#ifdef Q_OS_WIN
    setBlurEffect(AcrylicMaterial);
#elif defined(Q_OS_MAC)
    setBlurEffect(DarkBlur);
#endif
}

void MainWindow::setAutoTheme()
{
    const auto theme = styleAgent->systemTheme();
    theme == QWK::StyleAgent::SystemTheme::Dark ? setDarkTheme() : setLightTheme();
}

void MainWindow::setTheme(const int theme)
{
    if (theme != 2)
    {
        disconnect(styleAgent, &QWK::StyleAgent::systemThemeChanged, this, &MainWindow::setAutoTheme);
        if (theme == 0)
        {
            setLightTheme();
        }
        else
        {
            setDarkTheme();
        }
    }
    else
    {
        connect(styleAgent, &QWK::StyleAgent::systemThemeChanged, this, &MainWindow::setAutoTheme);
        setAutoTheme();
    }
}

void MainWindow::updateHomeMsg(const std::string& msg) const
{
    ui->homePage->updateMsg(msg);
}

void MainWindow::updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version) const
{
    ui->settingPage->updateAria2Version(version);
}

void MainWindow::AddDownloadTask(const std::string& gid) const
{
    ui->downloadPage->addTaskItem({"http://192.168.2.88:10240/job/VBT_storage_upgrade_prerelease/lastSuccessfulBuild/artifact/origin/develop_vbt_v100.zip"},
                                  {"http://192.168.2.88:10240/job/VBT_storage_upgrade_prerelease/lastSuccessfulBuild/artifact/tsp2_installer_BN100.exe"}, "test");
}

void MainWindow::updateVideoPage(const std::shared_ptr<Adapter::BaseVideoView>& videoView) const
{
    ui->VideoPage->updateVideoItem(videoView);
}

void MainWindow::installWindowAgent()
{
    windowAgent->setup(this);
    windowAgent->setTitleBar(windowBar);
    windowAgent->setHitTestVisible(windowBar->GetHitWidget(), true);

    setMenuWidget(windowBar);

#ifdef Q_OS_WIN
    loadSystemButton();
#endif
}

void MainWindow::SearchUrl()
{
}

void MainWindow::setBlurEffect(const BlurEffect effect)
void MainWindow::setBlurEffect(const BlurEffect effect)
{
    switch (effect)
    {
    case NoBlur:
#ifdef Q_OS_WIN
        windowAgent->setWindowAttribute(currentBlurEffect, false);
        break;
#elif defined(Q_OS_MAC)
        windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), "none");
#endif

#ifdef Q_OS_WIN
    case DWMBlur:
        windowAgent->setWindowAttribute(QStringLiteral("dwm-blur"), true);
        currentBlurEffect = QStringLiteral("dwm-blur");
        break;
    case AcrylicMaterial:
        windowAgent->setWindowAttribute(QStringLiteral("acrylic-material"), true);
        currentBlurEffect = "acrylic-material");
        break;
    case Mica:
        windowAgent->setWindowAttribute(QStringLiteral("mica"), true);
        currentBlurEffect = QStringLiteral("mica");
        break;
    case MicaAlt:
        windowAgent->setWindowAttribute(QStringLiteral("mica-alt"), true);
        currentBlurEffect = QStringLiteral("mica-alt");
        break;
#elif defined(Q_OS_MAC)
    case DarkBlur:
        windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), "dark");
        break;
    case LightBlur:
        windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), "light");
        break;
#endif
    default:
        break;
    }
    style()->polish(this);
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

#ifndef Q_OS_MAC
static inline void emulateLeaveEvent(QWidget* widget)
{
    Q_ASSERT(widget);
    QTimer::singleShot(0, widget, [widget]() {
#    if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        const QScreen* screen = widget->screen();
#    else
        const QScreen *screen = widget->windowHandle()->screen();
#    endif
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
#    if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
                const auto event = new QHoverEvent(QEvent::HoverLeave, scenePos, globalPos, oldPos, modifiers);
                Q_UNUSED(localPos);
#    elif (QT_VERSION >= QT_VERSION_CHECK(6, 3, 0))
                const auto event =  new QHoverEvent(QEvent::HoverLeave, localPos, globalPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#    else
                const auto event =  new QHoverEvent(QEvent::HoverLeave, localPos, oldPos, modifiers);
                Q_UNUSED(scenePos);
#    endif
                QCoreApplication::postEvent(widget, event);
            }
        }
    });
}

void MainWindow::loadSystemButton()
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
#endif

void MainWindow::ClearVideo(bool flag)
{
    if (!flag)
    {
        ui->VideoPage->clearVideo();
    }
}