#include "WebFrame.h"

#include <QMouseEvent>
#include <QScreen>
#include <QUrlQuery>
#include <QVBoxLayout>
#include <QWebChannel>
#include <QWebEngineFullScreenRequest>
#include <QWebEnginePage>
#include <QWebEngineView>
#include <QWebEngineProfile>

#include "WebPage.h"
#include "WebView.h"

WebFrame::WebFrame(QWidget* parent)
    : QFrame(parent)
    , m_view(new WebView(this))
    , m_page(new WebPage(nullptr, this))
{
    setUi();
    signalsAndSlots();
}

WebFrame::~WebFrame()
{
    m_view->deleteLater();
    m_page->deleteLater();
}

WebPage* WebFrame::page() const
{
    return m_page;
}

WebView* WebFrame::view() const
{
    return m_view;
}

void WebFrame::setWebChannelBridge(QObject* bridge)
{
    QWebChannel* channel = new QWebChannel(this);
    channel->registerObject(bridge->objectName(), bridge);
    m_page->setWebChannel(channel);
}

void WebFrame::load(const QUrl& url)
{
    m_page->load(url);
}

void WebFrame::exitFullScreen()
{
    if (QWebEnginePage* page = m_view->page())
    {
        page->triggerAction(QWebEnginePage::ExitFullScreen, true);
    }
}

void WebFrame::reload()
{
    m_view->reload();
}

QString WebFrame::cookies() const
{
    return m_page->cookies();
}

bool WebFrame::containsCookie(const QString& name) const
{
    return m_page->containsCookie(name);
}

void WebFrame::setUi()
{
    m_view->setContextMenuPolicy(Qt::NoContextMenu);
    m_view->setPage(m_page);

    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->addWidget(m_view);
}

void WebFrame::signalsAndSlots()
{
    connect(m_page, &WebPage::cookieAdded, this, &WebFrame::cookieAdded);
}
