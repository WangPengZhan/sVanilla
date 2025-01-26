#include "WebPage.h"

#include <QMessageBox>
#include <QTimer>
#include <QWebEngineCertificateError>
#include <QtDebug>
#include <QWebEngineCookieStore>
#include <QWebEngineProfile>

WebPage::WebPage(QWebEngineProfile* profile, QObject* parent)
    : QWebEnginePage(profile, parent)
{
    sigalsAndSlots();
}

QString WebPage::cookies() const
{
    QString cookieStr;
    for (auto it = m_cookies.begin(); it != m_cookies.end(); ++it)
    {
        cookieStr += it.key();
        if (!it.value().isEmpty())
        {
            cookieStr += "=" + it.value();
        }
        cookieStr += "; ";
    }

    return cookieStr;
}

bool WebPage::containsCookie(const QString& name) const
{
    return m_cookies.contains(name);
}

void WebPage::sigalsAndSlots()
{
    connect(this, &QWebEnginePage::selectClientCertificate, this, &WebPage::handleSelectClientCertificate);

    QWebEngineCookieStore* cookieStore = profile()->cookieStore();
    connect(cookieStore, &QWebEngineCookieStore::cookieAdded, this, &WebPage::onCookieAdded);
    connect(cookieStore, &QWebEngineCookieStore::cookieRemoved, this, &WebPage::onCookieRemoved);
}

void WebPage::handleSelectClientCertificate(QWebEngineClientCertificateSelection selection)
{
    // Just select one.
    selection.select(selection.certificates().at(0));
}

void WebPage::onCookieAdded(const QNetworkCookie& cookie)
{
    m_cookies.insert(cookie.name(), cookie.value());
    emit cookieAdded(cookie);
}

void WebPage::onCookieRemoved(const QNetworkCookie& cookie)
{
    qDebug() << "remove: " << cookie.name();
    m_cookies.remove(cookie.name());
}
