#include <QDir>
#include <QDebug>
#include <QFile>
#include <QMenu>
#include <QTimer>
#include <QStackedWidget>
#include <QShortcut>
#include <QKeySequence>
#include <QPushButton>
#include <QApplication>

#include <VanillaStyle/Style.h>
#include <VanillaStyle/Style/VanillaStyle.h>

#include <QWKCore/styleagent.h>
#include <QWKWidgets/widgetwindowagent.h>

#include "ClientUi/Setting/SettingsPage.h"
#include "ClientUi/VideoList/VideoWidget.h"
#include "ClientUi/MainWindow/SApplication.h"
#include "SUI/Tips/Toast.h"
#include "MainWindow.h"
#include "MainWindowlog.h"
#include "ui_MainWindow.h"

static constexpr int systemButtonSize = 14;
constexpr char softwareName[] = "sVanilla";

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , windowAgent(new QWK::WidgetWindowAgent(this))
    , styleAgent(new QWK::StyleAgent(this))
    , windowBar(new WindowBar(this))
    , systemTray(new QSystemTrayIcon(this))
    , m_uriProcess(new UriProcess(this))
{
    ui->setupUi(this);
    installWindowAgent();
    setWindowTitle(softwareName);
    setWindowIcon(QIcon(":/appIcon/svanilla.ico"));
    setUi();
    signalsAndSlots();
    setUpShortcuts();
}

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (systemTray->isVisible() && ui->settingPage->isEnableMinimizeTray() == Qt::Checked)
    {
        hide();
        event->ignore();
    }
    else
    {
        QMainWindow::closeEvent(event);
    }
}

void MainWindow::setUi()
{
    ui->stackedWidget->setCurrentWidget(ui->homePage);
    constexpr QSize defaultSize(800, 600);
    resize(defaultSize);
    Toast::create(this);
    setLightTheme();
    createTrayIcon();

    if (ui->settingPage->isSaveMainWindow() == Qt::Checked)
    {
        QString projectPath = SApplication::appDir() + "/.sVanilla";
        QFile projectFile(projectPath + "/MainWindow");
        if (projectFile.open(QIODevice::ReadOnly))
        {
            restoreGeometry(projectFile.readAll());
            projectFile.close();
        }
    }
}

void MainWindow::signalsAndSlots()
{
    // tab bar btn click event to change stacked page
    connect(windowBar, &WindowBar::barBtnClick, ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    connect(ui->stackedWidget, &QStackedWidget::currentChanged, [this](const int index) {
        if (index == 3)
        {
            ui->settingPage->connectAria2Server();
        }
    });
    connect(ui->homePage, &HomePage::updateWebsiteIcon, m_uriProcess, &UriProcess::updateWebsiteIcon);
    connect(ui->videoPage, &VideoWidget::updateWebsiteIcon, m_uriProcess, &UriProcess::updateWebsiteIcon);
    connect(m_uriProcess, &UriProcess::setWebsiteIcon, ui->homePage, &HomePage::setWebsiteIcon);
    connect(m_uriProcess, &UriProcess::setWebsiteIcon, ui->videoPage, &VideoWidget::setWebsiteIcon);

    connect(ui->homePage, &HomePage::parseUri, ui->videoPage, &VideoWidget::searchUrl);
    connect(ui->videoPage, &VideoWidget::parseUri, m_uriProcess, &UriProcess::parseUri);
    connect(m_uriProcess, &UriProcess::uriProcessComplete, this, &MainWindow::startLoading);

    connect(ui->videoPage, &VideoWidget::createBiliDownloadTask, ui->downloadPage, &DownloadWidget::getBiliUrl);

    connect(ui->downloadPage, &DownloadWidget::downloadingCountChanged, ui->videoPage, &VideoWidget::setDownloadingNumber);
    connect(ui->downloadPage, &DownloadWidget::downloadedCountChanged, ui->videoPage, &VideoWidget::setDownloadedNumber);

    connect(ui->settingPage, &SettingsPage::updateTheme, this, &MainWindow::setTheme);
    connect(ui->settingPage, &SettingsPage::enableTray, this, &MainWindow::setTrayIconVisible);
    connect(qApp, &QApplication::aboutToQuit, this, [&]() {
        if (ui->settingPage->isSaveMainWindow() != Qt::Checked)
        {
            return;
        }

        QString projectPath = SApplication::appDir() + "/.sVanilla";
        QDir projectDir(projectPath);
        if (!projectDir.exists())
        {
            projectDir.mkdir(projectPath);
        }

        QFile projectFile(projectPath + "/MainWindow");
        if (projectFile.open(QIODevice::WriteOnly))
        {
            projectFile.write(saveGeometry());
            projectFile.close();
        }
    });
}

void MainWindow::setUpShortcuts()
{
    auto* shortcutTabFirst = new QShortcut(this);
    auto* shortcutTabSecond = new QShortcut(this);
    auto* shortcutTabThird = new QShortcut(this);
    auto* shortcutTabFourth = new QShortcut(this);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    shortcutTabFirst->setKey(QKeySequence(Qt::CTRL | Qt::Key_1));
    shortcutTabSecond->setKey(QKeySequence(Qt::CTRL | Qt::Key_2));
    shortcutTabThird->setKey(QKeySequence(Qt::CTRL | Qt::Key_3));
    shortcutTabFourth->setKey(QKeySequence(Qt::CTRL | Qt::Key_4));
#else
    shortcutTabFirst->setKey(QKeySequence(Qt::CTRL + Qt::Key_1));
    shortcutTabSecond->setKey(QKeySequence(Qt::CTRL + Qt::Key_2));
    shortcutTabThird->setKey(QKeySequence(Qt::CTRL + Qt::Key_3));
    shortcutTabFourth->setKey(QKeySequence(Qt::CTRL + Qt::Key_4));
#endif

    const std::array tabShortcuts{shortcutTabFirst, shortcutTabSecond, shortcutTabThird, shortcutTabFourth};
    for (int i = 0; i < tabShortcuts.size(); i++)
    {
        connect(tabShortcuts[i], &QShortcut::activated, [this, i]() {
            ui->stackedWidget->setCurrentIndex(i);
            emit windowBar->tabChanged(i);
        });
    }
}

void MainWindow::setLightTheme()
{
    Vanilla::Style::setStyleFromAppDir("light");
#ifdef _WIN32
    setBlurEffect(AcrylicMaterial);
#elif __APPLE__
    setBlurEffect(LightBlur);
#endif
}

void MainWindow::setDarkTheme()
{
    Vanilla::Style::setStyleFromAppDir("dark");

#ifdef _WIN32
    setBlurEffect(AcrylicMaterial);
#elif __APPLE__
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
    if (const bool isAutoTheme = (theme == 2); !isAutoTheme)
    {
        disconnect(styleAgent, &QWK::StyleAgent::systemThemeChanged, this, &MainWindow::setAutoTheme);
        (theme == 0) ? setLightTheme() : setDarkTheme();
    }
    else
    {
        connect(styleAgent, &QWK::StyleAgent::systemThemeChanged, this, &MainWindow::setAutoTheme);
        setAutoTheme();
    }
}

void MainWindow::installWindowAgent()
{
    windowAgent->setup(this);
    windowAgent->setTitleBar(windowBar);
    windowAgent->setHitTestVisible(windowBar->getHitWidget(), true);

    setMenuWidget(windowBar);

#ifndef __APPLE__
    loadSystemButton();
#endif
}

void MainWindow::createTrayIcon()
{
    auto* const trayIconMenu = new QMenu(this);

    auto* const restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);
    trayIconMenu->addAction(restoreAction);

    auto* const minimizeAction = new QAction(tr("Mi&nimize"), this);
    trayIconMenu->addAction(minimizeAction);
    connect(minimizeAction, &QAction::triggered, this, &QMainWindow::hide);

    trayIconMenu->addSeparator();

    auto* const quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    trayIconMenu->addAction(quitAction);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    restoreAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    minimizeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_M));
    quitAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
#else
    restoreAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    minimizeAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
    quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
#endif
    systemTray->setContextMenu(trayIconMenu);

    setTrayIconVisible(ui->settingPage->getTrayState());

    connect(systemTray, &QSystemTrayIcon::activated, this, [&](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger)
        {
            if (isVisible() || QApplication::activeWindow())
            {
                hide();
            }
            else
            {
                raise();
                activateWindow();
                show();
            }
        }
    });
}

void MainWindow::setTrayIconVisible(int state)
{
    if (state == Qt::Checked)
    {
        if (systemTray->icon().isNull())
        {
#ifndef __APPLE__
            systemTray->setIcon(QIcon(":/appIcon/sVanilla.svg"));
#else
            systemTray->setIcon(QIcon(":/appIcon/whiteFgWithNoBroder.svg"));
#endif
        }
        systemTray->show();
    }
    else
    {
        systemTray->hide();
    }
}

void MainWindow::startLoading(const UriProcess::UriInfo& uriInfo)
{
    if (uriInfo.type == "default")
    {
        ui->stackedWidget->setCurrentIndex(2);
        emit windowBar->tabChanged(2);
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(1);
        emit windowBar->tabChanged(1);

        ui->videoPage->prepareBiliVideoView(uriInfo.id);
    }
}

void MainWindow::setBlurEffect(const BlurEffect effect)
{
    switch (effect)
    {
    case NoBlur:
#ifdef _WIN32
        windowAgent->setWindowAttribute(currentBlurEffect, false);
        break;
#elif __APPLE__
        windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), "none");
#endif

#ifdef _WIN32
    case DWMBlur:
        windowAgent->setWindowAttribute(QStringLiteral("dwm-blur"), true);
        currentBlurEffect = QStringLiteral("dwm-blur");
        break;
    case AcrylicMaterial:
        windowAgent->setWindowAttribute(QStringLiteral("acrylic-material"), true);
        currentBlurEffect = QStringLiteral("acrylic-material");
        break;
    case Mica:
        windowAgent->setWindowAttribute(QStringLiteral("mica"), true);
        currentBlurEffect = QStringLiteral("mica");
        break;
    case MicaAlt:
        windowAgent->setWindowAttribute(QStringLiteral("mica-alt"), true);
        currentBlurEffect = QStringLiteral("mica-alt");
        break;
#elif __APPLE__
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

#ifndef __APPLE__
// clang-format off
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
// clang-format on
void MainWindow::loadSystemButton()
{
    auto const buttonConfigs = {std::make_tuple(QWK::WindowAgentBase::Minimize, ":/icon/bar/minimize.svg", &WindowBar::setMinButton),
                                std::make_tuple(QWK::WindowAgentBase::Maximize, ":/icon/bar/maximize.svg", &WindowBar::setMaxButton),
                                std::make_tuple(QWK::WindowAgentBase::Close, ":/icon/bar/close.svg", &WindowBar::setCloseButton)};

    for (const auto& config : buttonConfigs)
    {
        auto* const button = new QPushButton();
        button->setProperty("_vanillaStyle_Patch", QVariant("SystemButtonPatch"));
        button->setIcon(QIcon(std::get<1>(config)));
        button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

        const auto setterFunc = std::get<2>(config);
        (windowBar->*setterFunc)(button);
        windowAgent->setSystemButton(std::get<0>(config), button);
    }
    // 3. Adds simulated mouse events to the title bar buttons
    // Emulate Window system menu button behaviors

    connect(windowBar, &WindowBar::minimizeRequested, this, &QWidget::showMinimized);
    connect(windowBar, &WindowBar::maximizeRequested, this, [this]() {
        isMaximized() ? showNormal() : showMaximized();
        emulateLeaveEvent(windowBar->maxButton());
    });
    connect(windowBar, &WindowBar::closeRequested, this, &QWidget::close);
    installEventFilter(windowBar);
}
#endif