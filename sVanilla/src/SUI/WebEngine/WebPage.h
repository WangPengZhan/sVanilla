#pragma once
#include <QWebEngineCertificateError>
#include <QWebEngineClientCertificateSelection>
#include <QWebEnginePage>
#include <QMap>

class QNetworkCookie;

class WebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    explicit WebPage(QWebEngineProfile* profile, QObject* parent = nullptr);

    QString cookies() const;
    bool containsCookie(const QString& name) const;

signals:
    void cookieAdded(const QNetworkCookie& cookie);

private:
    void sigalsAndSlots();

    void handleCertificateError(QWebEngineCertificateError error);
    void handleSelectClientCertificate(QWebEngineClientCertificateSelection clientCertSelection);

    void onCookieAdded(const QNetworkCookie& cookie);
    void onCookieRemoved(const QNetworkCookie& cookie);

private:
    QMap<QString, QString> m_cookies;
};
