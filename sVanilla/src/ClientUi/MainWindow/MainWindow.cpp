#include <QDir>
#include <QtWidgets/QPushButton>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <QWKCore/styleagent.h>
#include <QWKWidgets/widgetwindowagent.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#    include <QtGui/QActionGroup>
#else
#    include <QtWidgets/QActionGroup>
#endif

#include "windowbar.h"
#include "windowbutton.h"
#include "BiliApi/BilibiliClient.h"
#include "MainWindow.h"
#include "MainWindowlog.h"
#include "Sqlite/SQLiteManager.h"
#include "Util/UrlProcess.h"
#include "SUI/SearchLineEdit.h"
// #include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
//    , ui(new Ui::MainWindow)
{
    //    ui->setupUi(this);
    installWindowAgent();
    installStyleAgent();

    //    SetUi();
    loadStyleSheet(Light);
    //    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    //    setWindowTitle(tr("Bili Downloader"));
    SignalsAndSlots();
    resize(800, 600);
}

MainWindow::~MainWindow() = default;

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

bool MainWindow::event(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::WindowActivate: {
        auto menu = menuWidget();
        menu->setProperty("bar-active", true);
        style()->polish(menu);
        break;
    }

    case QEvent::WindowDeactivate: {
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

void MainWindow::installWindowAgent()
{
    windowAgent = new QWK::WidgetWindowAgent(this);
    windowAgent->setup(this);

    // 2. Construct your title bar
    auto menuBar = [this]() {
        auto menuBar = new QMenuBar();

        // Virtual menu
        auto file = new QMenu(tr("File(&F)"), menuBar);
        file->addAction(new QAction(tr("New(&N)"), menuBar));
        file->addAction(new QAction(tr("Open(&O)"), menuBar));
        file->addSeparator();

        auto edit = new QMenu(tr("Edit(&E)"), menuBar);
        edit->addAction(new QAction(tr("Undo(&U)"), menuBar));
        edit->addAction(new QAction(tr("Redo(&R)"), menuBar));

        // Theme action
        auto darkAction = new QAction(tr("Enable dark theme"), menuBar);
        darkAction->setCheckable(true);
        connect(darkAction, &QAction::triggered, this, [this](bool checked) {
            loadStyleSheet(checked ? Dark : Light);  //
        });
        connect(this, &MainWindow::themeChanged, darkAction, [this, darkAction]() {
            darkAction->setChecked(currentTheme == Dark);  //
        });

#ifdef Q_OS_WIN
        auto dwmBlurAction = new QAction(tr("Enable DWM blur"), menuBar);
        dwmBlurAction->setCheckable(true);
        connect(dwmBlurAction, &QAction::triggered, this, [this](bool checked) {
            if (!windowAgent->setWindowAttribute(QStringLiteral("dwm-blur"), checked))
            {
                return;
            }
            setProperty("custom-style", checked);
            style()->polish(this);
        });

        auto acrylicAction = new QAction(tr("Enable acrylic material"), menuBar);
        acrylicAction->setCheckable(true);
        connect(acrylicAction, &QAction::triggered, this, [this](bool checked) {
            if (!windowAgent->setWindowAttribute(QStringLiteral("acrylic-material"), true))
            {
                return;
            }
            setProperty("custom-style", checked);
            style()->polish(this);
        });

        auto micaAction = new QAction(tr("Enable mica"), menuBar);
        micaAction->setCheckable(true);
        connect(micaAction, &QAction::triggered, this, [this](bool checked) {
            if (!windowAgent->setWindowAttribute(QStringLiteral("mica"), checked))
            {
                return;
            }
            setProperty("custom-style", checked);
            style()->polish(this);
        });

        auto micaAltAction = new QAction(tr("Enable mica alt"), menuBar);
        micaAltAction->setCheckable(true);
        connect(micaAltAction, &QAction::triggered, this, [this](bool checked) {
            if (!windowAgent->setWindowAttribute(QStringLiteral("mica-alt"), checked))
            {
                return;
            }
            setProperty("custom-style", checked);
            style()->polish(this);
        });

        auto winStyleGroup = new QActionGroup(menuBar);
        winStyleGroup->addAction(dwmBlurAction);
        winStyleGroup->addAction(acrylicAction);
        winStyleGroup->addAction(micaAction);
        winStyleGroup->addAction(micaAltAction);
#endif

        // Real menu
        auto settings = new QMenu(tr("Settings(&S)"), menuBar);
        settings->addAction(darkAction);

        menuBar->addMenu(file);
        menuBar->addMenu(edit);
        menuBar->addMenu(settings);
        return menuBar;
    }();

    menuBar->setObjectName(QStringLiteral("win-menu-bar"));

#ifndef Q_OS_MAC
    auto iconButton = new Ui::WindowButton();
    iconButton->setObjectName(QStringLiteral("icon-button"));
    iconButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto minButton = new Ui::WindowButton();
    minButton->setObjectName(QStringLiteral("min-button"));
    minButton->setProperty("system-button", true);
    minButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto maxButton = new Ui::WindowButton();
    maxButton->setCheckable(true);
    maxButton->setObjectName(QStringLiteral("max-button"));
    maxButton->setProperty("system-button", true);
    maxButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto closeButton = new Ui::WindowButton();
    closeButton->setObjectName(QStringLiteral("close-button"));
    closeButton->setProperty("system-button", true);
    closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
#endif

    auto windowBar = new Ui::WindowBar();
#ifndef Q_OS_MAC
    windowBar->setIconButton(iconButton);
    windowBar->setMinButton(minButton);
    windowBar->setMaxButton(maxButton);
    windowBar->setCloseButton(closeButton);
#endif

    windowBar->setMenuBar(menuBar);

    auto titleLabel = new QLabel("sVanilla");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName(QStringLiteral("win-title-label"));

    auto searchLineEdit = new SearchLineEdit();
    windowBar->setBarWidget(searchLineEdit);
    windowBar->setTitleLabel(titleLabel);
    windowBar->setHostWidget(this);

    windowAgent->setTitleBar(windowBar);
    //    windowAgent->setHitTestVisible(menuBar, false);

    setMenuWidget(windowBar);

#ifndef Q_OS_MAC
    windowAgent->setSystemButton(QWK::WindowAgentBase::WindowIcon, iconButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Minimize, minButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Maximize, maxButton);
    windowAgent->setSystemButton(QWK::WindowAgentBase::Close, closeButton);
#endif
    // 3. Adds simulated mouse events to the title bar buttons
#ifdef Q_OS_WINDOWS
    // Emulate Window system menu button behaviors
    connect(iconButton, &QAbstractButton::clicked, windowAgent, [this, iconButton] {
        iconButton->setProperty("double-click-close", false);

        // Pick a suitable time threshold
        QTimer::singleShot(75, windowAgent, [this, iconButton]() {
            if (iconButton->property("double-click-close").toBool())
                return;
            windowAgent->showSystemMenu(iconButton->mapToGlobal(QPoint{0, iconButton->height()}));
        });
    });
    connect(iconButton, &QWK::WindowButton::doubleClicked, this, [iconButton, this]() {
        iconButton->setProperty("double-click-close", true);
        close();
    });
#endif

#ifndef Q_OS_MAC
    connect(windowBar, &Ui::WindowBar::minimizeRequested, this, &QWidget::showMinimized);
    connect(windowBar, &Ui::WindowBar::maximizeRequested, this, [this, maxButton](bool max) {
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
    connect(windowBar, &Ui::WindowBar::closeRequested, this, &QWidget::close);
#endif
}

void MainWindow::installStyleAgent()
{
    styleAgent = new QWK::StyleAgent(this);
}

void MainWindow::SearchUrl()
{
}

// void MainWindow::SetUi()
//{
//     installEventFilter(ui->widgetTitle);
// }

void MainWindow::SignalsAndSlots()
{
}

void MainWindow::loadStyleSheet(Theme theme)
{
    if (!styleSheet().isEmpty() && theme == currentTheme)
        return;
    currentTheme = theme;

    if (QFile qss(theme == Dark ? QStringLiteral(":/MainWindow/dark-style.qss") : QStringLiteral(":/MainWindow/light-style.qss")); qss.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        setStyleSheet(QString::fromUtf8(qss.readAll()));
        Q_EMIT themeChanged();
    }
}
