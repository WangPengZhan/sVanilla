#include "LoginWebDialog.h"
#include "ui_LoginWebDialog.h"

#include "SUI/WebEngine/WebSetting.h"
#include "SUI/WebEngine/WebBridge.h"
#include "SUI/WebEngine/WebView.h"
#include "SUI/WebEngine/WebPage.h"

#include <QWebEngineCookieStore>
#include <QThread>

#include <LoginWeb.h>
#include <LoginProxy.h>

LoginWebDialog::LoginWebDialog(std::shared_ptr<LoginProxy> loginer, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::LoginWebDialog)
    , m_loginer(loginer)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

LoginWebDialog::~LoginWebDialog()
{
    delete ui;
}

void LoginWebDialog::signalsAndSlots()
{
    connect(ui->webFrame, &WebFrame::cookieAdded, this, &LoginWebDialog::cookiesChanged);
}

void LoginWebDialog::setUi()
{
    initDefaultWebSettings(ui->webFrame->view());
    auto realLogin = reinterpret_cast<AbstractLoginWeb*>(&m_loginer->realLogin());
    ui->webFrame->view()->setUrl(QString::fromStdString(realLogin->loginUrl()));
    auto bridge = new WebBridge(qobject_cast<QWebEnginePage*>(ui->webFrame->page()), this);
    ui->webFrame->setWebChannelBridge(bridge);
}

void LoginWebDialog::setUrl(const QUrl& url)
{
    ui->webFrame->view()->setUrl(url);
}

void LoginWebDialog::cookiesChanged(const QNetworkCookie& cookie)
{
    qDebug() << "cookieAdded:" << cookie.name() << ":" << cookie.value();
    auto realLogin = reinterpret_cast<AbstractLoginWeb*>(&m_loginer->realLogin());
    auto keys = realLogin->mustKeys();
    bool isAddMustKey = keys.find(cookie.name().toStdString()) != keys.end();
    if (!isAddMustKey)
    {
        return;
    }

    for (const auto key : keys)
    {
        if (!ui->webFrame->containsCookie(QString::fromStdString(key)))
        {
            return;
        }
    }

    QTimer::singleShot(0, this, [this, realLogin] {
        realLogin->setCookies(ui->webFrame->cookies().toStdString());
        accept();
    });
}
