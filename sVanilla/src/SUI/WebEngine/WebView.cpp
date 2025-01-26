#include "WebView.h"

#include <QAuthenticator>
#include <QContextMenuEvent>
#include <QDebug>
#include <QKeySequence>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QShortcut>
#include <QStyle>
#include <QTimer>
#include <QWebEngineCertificateError>
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
#    include <QWebEngineFileSystemAccessRequest>
#endif
#include <QWebEngineRegisterProtocolHandlerRequest>

#include "WebPage.h"

WebView::WebView(QWidget* parent)
    : QWebEngineView(parent)
{
    setMouseTracking(true);
    // installEventFilter(this);
    setAcceptDrops(false);

    showDevToolPage();
    F5RefreshPage();
    connect(this, &QWebEngineView::loadStarted, [this]() {
        m_loadProgress = 0;
        emit favIconChanged(favIcon());
    });
    connect(this, &QWebEngineView::loadProgress, [this](int progress) {
        m_loadProgress = progress;
    });
    connect(this, &QWebEngineView::loadFinished, [this](bool success) {
        m_loadProgress = success ? 100 : -1;
        emit favIconChanged(favIcon());
    });
    connect(this, &QWebEngineView::iconChanged, [this](const QIcon&) {
        emit favIconChanged(favIcon());
    });

    connect(this, &QWebEngineView::renderProcessTerminated, [this](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
        QString status;
        switch (termStatus)
        {
        case QWebEnginePage::NormalTerminationStatus:
            status = tr("Render process normal exit");
            break;
        case QWebEnginePage::AbnormalTerminationStatus:
            status = tr("Render process abnormal exit");
            break;
        case QWebEnginePage::CrashedTerminationStatus:
            status = tr("Render process crashed");
            break;
        case QWebEnginePage::KilledTerminationStatus:
            status = tr("Render process killed");
            break;
        }
        QMessageBox::StandardButton btn = QMessageBox::question(window(), status,
                                                                tr("Render process exited with code: %1\n"
                                                                   "Do you want to reload the page ?")
                                                                    .arg(statusCode));
        if (btn == QMessageBox::Yes)
            QTimer::singleShot(0, this, &WebView::reload);
    });
}

WebView::~WebView()
{
}

inline QString questionForFeature(QWebEnginePage::Feature feature)
{
    switch (feature)
    {
    case QWebEnginePage::Geolocation:
        return QObject::tr("Allow %1 to access your location information?");
    case QWebEnginePage::MediaAudioCapture:
        return QObject::tr("Allow %1 to access your microphone?");
    case QWebEnginePage::MediaVideoCapture:
        return QObject::tr("Allow %1 to access your webcam?");
    case QWebEnginePage::MediaAudioVideoCapture:
        return QObject::tr("Allow %1 to access your microphone and webcam?");
    case QWebEnginePage::MouseLock:
        return QObject::tr("Allow %1 to lock your mouse cursor?");
    case QWebEnginePage::DesktopVideoCapture:
        return QObject::tr("Allow %1 to capture video of your desktop?");
    case QWebEnginePage::DesktopAudioVideoCapture:
        return QObject::tr("Allow %1 to capture audio and video of your desktop?");
    case QWebEnginePage::Notifications:
        return QObject::tr("Allow %1 to show notification on your desktop?");
    }
    return QString();
}

void WebView::setPage(WebPage* page)
{
    if (auto oldPage = qobject_cast<WebPage*>(QWebEngineView::page()))
    {
        disconnect(oldPage, &QWebEnginePage::authenticationRequired, this, &WebView::handleAuthenticationRequired);
        disconnect(oldPage, &QWebEnginePage::featurePermissionRequested, this, &WebView::handleFeaturePermissionRequested);
        disconnect(oldPage, &QWebEnginePage::proxyAuthenticationRequired, this, &WebView::handleProxyAuthenticationRequired);
        disconnect(oldPage, &QWebEnginePage::registerProtocolHandlerRequested, this, &WebView::handleRegisterProtocolHandlerRequested);
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
        disconnect(oldPage, &QWebEnginePage::fileSystemAccessRequested, this, &WebView::handleFileSystemAccessRequested);
#endif
    }
    createWebActionTrigger(page, QWebEnginePage::Forward);
    createWebActionTrigger(page, QWebEnginePage::Back);
    createWebActionTrigger(page, QWebEnginePage::Reload);
    createWebActionTrigger(page, QWebEnginePage::Stop);
    QWebEngineView::setPage(page);
    connect(page, &QWebEnginePage::authenticationRequired, this, &WebView::handleAuthenticationRequired);
    connect(page, &QWebEnginePage::featurePermissionRequested, this, &WebView::handleFeaturePermissionRequested);
    connect(page, &QWebEnginePage::proxyAuthenticationRequired, this, &WebView::handleProxyAuthenticationRequired);
    connect(page, &QWebEnginePage::registerProtocolHandlerRequested, this, &WebView::handleRegisterProtocolHandlerRequested);
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    connect(page, &QWebEnginePage::fileSystemAccessRequested, this, &WebView::handleFileSystemAccessRequested);
#endif
}

int WebView::loadProgress() const
{
    return m_loadProgress;
}

void WebView::createWebActionTrigger(QWebEnginePage* page, QWebEnginePage::WebAction webAction)
{
    QAction* action = page->action(webAction);
    connect(action, &QAction::changed, [this, action, webAction] {
        emit webActionEnabledChanged(webAction, action->isEnabled());
    });
}

bool WebView::isWebActionEnabled(QWebEnginePage::WebAction webAction) const
{
    return page()->action(webAction)->isEnabled();
}

QIcon WebView::favIcon() const
{
    QIcon favIcon = icon();
    if (!favIcon.isNull())
        return favIcon;

    if (m_loadProgress < 0)
    {
        static QIcon errorIcon("");
        return errorIcon;
    }
    if (m_loadProgress < 100)
    {
        static QIcon loadingIcon("");
        return loadingIcon;
    }

    static QIcon defaultIcon("");
    return defaultIcon;
}

void WebView::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = new QMenu(this);
    const QList<QAction*> actions = menu->actions();
    auto inspectElement = std::find(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::InspectElement));
    if (inspectElement == actions.cend())
    {
        auto viewSource = std::find(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::ViewSource));
        if (viewSource == actions.cend())
            menu->addSeparator();

        QAction* action = menu->addAction("Open inspector in new window");
        connect(action, &QAction::triggered, [this]() {
            emit devToolsRequested(page());
        });
    }
    else
    {
        (*inspectElement)->setText(tr("Inspect element"));
    }
    menu->popup(event->globalPos());
}

void WebView::handleCertificateError(QWebEngineCertificateError error)
{
}

void WebView::handleAuthenticationRequired(const QUrl& requestUrl, QAuthenticator* auth)
{
}

void WebView::handleFeaturePermissionRequested(const QUrl& securityOrigin, QWebEnginePage::Feature feature)
{
    QString title = tr("Permission Request");
    QString question = questionForFeature(feature).arg(securityOrigin.host());
    if (!question.isEmpty() && QMessageBox::question(window(), title, question) == QMessageBox::Yes)
        page()->setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
    else
        page()->setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);
}

void WebView::handleProxyAuthenticationRequired(const QUrl&, QAuthenticator* auth, const QString& proxyHost)
{
}

//! [registerProtocolHandlerRequested]
void WebView::handleRegisterProtocolHandlerRequested(QWebEngineRegisterProtocolHandlerRequest request)
{
    auto answer =
        QMessageBox::question(window(), tr("Permission Request"), tr("Allow %1 to open all %2 links?").arg(request.origin().host()).arg(request.scheme()));
    if (answer == QMessageBox::Yes)
        request.accept();
    else
        request.reject();
}
//! [registerProtocolHandlerRequested]

#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
void WebView::handleFileSystemAccessRequested(QWebEngineFileSystemAccessRequest request)
{
    QString accessType;
    switch (request.accessFlags())
    {
    case QWebEngineFileSystemAccessRequest::Read:
        accessType = "read";
        break;
    case QWebEngineFileSystemAccessRequest::Write:
        accessType = "write";
        break;
    case QWebEngineFileSystemAccessRequest::Read | QWebEngineFileSystemAccessRequest::Write:
        accessType = "read and write";
        break;
    default:
        Q_UNREACHABLE();
    }

    auto answer = QMessageBox::question(window(), tr("File system access request"),
                                        tr("Give %1 %2 access to %3?").arg(request.origin().host()).arg(accessType).arg(request.filePath().toString()));
    if (answer == QMessageBox::Yes)
        request.accept();
    else
        request.reject();
}
#endif  // QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)

bool WebView::event(QEvent* ev)
{
    return QWebEngineView::event(ev);
}

bool WebView::eventFilter(QObject* obj, QEvent* ev)
{
    return QWebEngineView::eventFilter(obj, ev);
}

QWebEngineView* WebView::showDevToolPage()
{
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_F12), this);
    QWebEngineView* newView = new QWebEngineView();
    QObject::connect(shortcut, &QShortcut::activated, this, [this, newView]() mutable {
        newView = new QWebEngineView();
        this->page()->setDevToolsPage(newView->page());
        this->page()->triggerAction(QWebEnginePage::InspectElement);
        newView->setAttribute(Qt::WA_DeleteOnClose);
        newView->show();
    });
    return newView;
}

void WebView::F5RefreshPage()
{
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_F5), this);
    QObject::connect(shortcut, &QShortcut::activated, this, [this]() {
        this->reload();
    });
    return;
}

QWebEngineView* WebView::createWindow(QWebEnginePage::WebWindowType type)
{
    return QWebEngineView::createWindow(type);
}
