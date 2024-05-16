#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QStackedWidget>
#include <QShortcut>
#include <QKeySequence>
#include <QPushButton>

#include <VanillaStyle/Style.h>
#include <VanillaStyle/Style/VanillaStyle.h>
#include <VanillaStyle/Widgets/IconButton.h>

#include <QWKCore/styleagent.h>
#include <QWKWidgets/widgetwindowagent.h>

#include "ClientUi/Setting/SettingsPage.h"
#include "ClientUi/VideoList/VideoWidget.h"
#include "SUI/Tips/Toast.h"
#include "MainWindow.h"
#include "MainWindowlog.h"
#include "ui_MainWindow.h"

static constexpr int systemButtonSize = 14;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , windowAgent(new QWK::WidgetWindowAgent(this))
    , styleAgent(new QWK::StyleAgent(this))
    , windowBar(new WindowBar(this))
{
    ui->setupUi(this);
    installWindowAgent();
    setUi();
    signalsAndSlots();
    setUpShortcuts();
}

MainWindow::~MainWindow() = default;

void MainWindow::setUi()
{
    ui->stackedWidget->setCurrentWidget(ui->homePage);
    resize(800, 600);
    Toast::create(this);
    setLightTheme();
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
    connect(ui->settingPage, &SettingsPage::UpdateTheme, this, &MainWindow::setTheme);

    connect(ui->homePage, &HomePage::loadBiliViewView, [this](const std::string& uri) {
        ui->VideoPage->prepareBiliVideoView(uri);
        ui->stackedWidget->setCurrentIndex(1);
        emit windowBar->tabChanged(1);
    });

    connect(ui->VideoPage, &VideoWidget::createBiliDownloadTask, ui->downloadPage, &DownloadWidget::getBiliUrl);
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
    connect(windowBar, &WindowBar::maximizeRequested, this, [this](const bool max) {
        (max) ? showMaximized() : showNormal();
        emulateLeaveEvent(windowBar->maxButton());
    });
    connect(windowBar, &WindowBar::closeRequested, this, &QWidget::close);
}
#endif