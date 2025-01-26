#pragma once

#include <QIcon>
#include <QWebEngineCertificateError>
#include <QWebEngineView>
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
#    include <QWebEngineFileSystemAccessRequest>
#endif
#include <QWebEnginePage>
#include <QWebEngineRegisterProtocolHandlerRequest>

class WebPage;
class WebView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit WebView(QWidget* parent = nullptr);
    ~WebView();
    void setPage(WebPage* page);

    int loadProgress() const;
    bool isWebActionEnabled(QWebEnginePage::WebAction webAction) const;
    QIcon favIcon() const;

signals:
    void webActionEnabledChanged(QWebEnginePage::WebAction webAction, bool enabled);
    void favIconChanged(const QIcon& icon);
    void devToolsRequested(QWebEnginePage* source);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    QWebEngineView* createWindow(QWebEnginePage::WebWindowType type) override;
    bool event(QEvent* ev) override;
    bool eventFilter(QObject* obj, QEvent* ev) override;

private:
    void handleCertificateError(QWebEngineCertificateError error);
    void handleAuthenticationRequired(const QUrl& requestUrl, QAuthenticator* auth);
    void handleFeaturePermissionRequested(const QUrl& securityOrigin, QWebEnginePage::Feature feature);
    void handleProxyAuthenticationRequired(const QUrl& requestUrl, QAuthenticator* auth, const QString& proxyHost);
    void handleRegisterProtocolHandlerRequested(QWebEngineRegisterProtocolHandlerRequest request);

#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    void handleFileSystemAccessRequested(QWebEngineFileSystemAccessRequest request);
#endif

    void createWebActionTrigger(QWebEnginePage* page, QWebEnginePage::WebAction);
    QWebEngineView* showDevToolPage();
    void F5RefreshPage();

private:
    int m_loadProgress = 100;
};
