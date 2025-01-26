#pragma once
#include <QFrame>

class WebPage;
class WebView;
class WebBridge;
class QWebEngineCookieStore;
class QNetworkCookie;

class WebFrame : public QFrame
{
    Q_OBJECT

public:
    WebFrame(QWidget* parent = nullptr);
    ~WebFrame();
    WebPage* page() const;
    WebView* view() const;
    void setWebChannelBridge(QObject* bridge);
    void load(const QUrl& url);
    void exitFullScreen();
    void reload();

    QString cookies() const;
    bool containsCookie(const QString& name) const;

signals:
    void cookieAdded(const QNetworkCookie& cookie);

private:
    void setUi();
    void signalsAndSlots();

private:
    WebPage* m_page;
    WebView* m_view;
    Qt::WindowStates m_windowState;
};
