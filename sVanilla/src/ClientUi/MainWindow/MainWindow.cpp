#include <QDir>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <QWKCore/styleagent.h>
#include <QWKWidgets/widgetwindowagent.h>
#include "BiliApi/BilibiliClient.h"
#include "MainWindow.h"
#include "MainWindowlog.h"
#include "ClientUi/Setting/SettingPage.h"
#include "Sqlite/SQLiteManager.h"
// #include "Util/UrlProcess.h"
#include "ClientUi/Download/DownloadingListWidget.h"
#include "ClientUi/Event.h"
#include <QStackedWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      windowAgent(new QWK::WidgetWindowAgent(this)),
      styleAgent(new QWK::StyleAgent(this)),
      windowBar(new WindowBar(this)),
      stackedPage(new QStackedWidget(this)),
      homePage(new HomePage(this)),
      galleryPage(new QWidget(this)),
      downloadPage(new DownloadingListWidget(this)),
      settingPage(new SettingPage(this))
{
    installWindowAgent();
    installStyleAgent();
    stackedPage->setContentsMargins(0, 50, 0, 0);
    setCentralWidget(stackedPage);

    stackedPage->insertWidget(0, homePage);
    stackedPage->insertWidget(1, galleryPage);
    stackedPage->insertWidget(2, downloadPage);
    stackedPage->insertWidget(3, settingPage);

    windowAgent->setWindowAttribute(QStringLiteral("blur-effect"), "light");
    loadStyleSheet(Light);
    SignalsAndSlots();
    resize(800, 600);

    auto downloadItem = new DownloadingItemWidget(downloadPage);
    downloadPage->addListItem(downloadItem);
    auto downloadItemTwo = new DownloadingItemWidget(downloadPage);
    downloadPage->addListItem(downloadItemTwo);
    auto downloadItemTree = new DownloadingItemWidget(downloadPage);
    downloadPage->addListItem(downloadItemTree);
}

MainWindow::~MainWindow() = default;

void MainWindow::installWindowAgent()
{
    windowAgent->setup(this);
    windowAgent->setTitleBar(windowBar);
    windowAgent->setHitTestVisible(windowBar->GetHitWidget(), true);

    setMenuWidget(windowBar);
}

void MainWindow::installStyleAgent()
{
    styleAgent = new QWK::StyleAgent(this);
}

void MainWindow::SearchUrl()
{
}

void MainWindow::SignalsAndSlots()
{
    connect(Event::getInstance(), &Event::BarBtnClick, this, [this](int index) {
        if (index == 0) {
            // account
        }
        else {
            stackedPage->setCurrentIndex(index-1);

        }

    });
    connect(stackedPage, &QStackedWidget::currentChanged,Event::getInstance(), &Event::StackedPageChanged);

    connect(Event::getInstance(), &Event::SearchBtnClick, this, [this]() {
        stackedPage->setCurrentIndex(1);
    });

}

void MainWindow::loadStyleSheet(const Theme theme)
{
    if (!styleSheet().isEmpty() && theme == currentTheme)
        return;
    currentTheme = theme;

    if (QFile qss(theme == Dark ? QStringLiteral(":/style/dark.qss") : QStringLiteral(":/style/light.qss")); qss.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        setStyleSheet(QString::fromUtf8(qss.readAll()));
        Q_EMIT themeChanged();
    }
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
    // auto windowBar = new Ui::WindowBar();
    //
    // auto minButton = new Ui::WindowButton();
    // minButton->setObjectName(QStringLiteral("min-button"));
    // minButton->setProperty("system-button", true);
    // minButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //
    // auto maxButton = new Ui::WindowButton();
    // maxButton->setCheckable(true);
    // maxButton->setObjectName(QStringLiteral("max-button"));
    // maxButton->setProperty("system-button", true);
    // maxButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //
    // auto closeButton = new Ui::WindowButton();
    // closeButton->setObjectName(QStringLiteral("close-button"));
    // closeButton->setProperty("system-button", true);
    // closeButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //
    // windowBar->setMinButton(minButton);
    // windowBar->setMaxButton(maxButton);
    // windowBar->setCloseButton(closeButton);
    //
    // windowAgent->setSystemButton(QWK::WindowAgentBase::Minimize, minButton);
    // windowAgent->setSystemButton(QWK::WindowAgentBase::Maximize, maxButton);
    // windowAgent->setSystemButton(QWK::WindowAgentBase::Close, closeButton);
    //
    //
    // // 3. Adds simulated mouse events to the title bar buttons
    //
    // // Emulate Window system menu button behaviors
    //
    // connect(windowBar, &Ui::WindowBar::minimizeRequested, this, &QWidget::showMinimized);
    // connect(windowBar, &Ui::WindowBar::maximizeRequested, this, [this, maxButton](bool max) {
    //     if (max)
    //     {
    //         showMaximized();
    //     }
    //     else
    //     {
    //         showNormal();
    //     }
    //
    //     // It's a Qt issue that if a QAbstractButton::clicked triggers a window's maximization,
    //     // the button remains to be hovered until the mouse move. As a result, we need to
    //     // manually send leave events to the button.
    //     emulateLeaveEvent(maxButton);
    // });
    // connect(windowBar, &Ui::WindowBar::closeRequested, this, &QWidget::close);
}
