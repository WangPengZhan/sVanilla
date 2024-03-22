#include <QHBoxLayout>
#include <QLabel>
#include <SUI/QrCodeGenerator.h>
#include <QDebug>
#include <QPushButton>

#include <regex>

#include "AccountSetting.h"
#include "ui_AccountSetting.h"

AccountSetting::AccountSetting(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AccountSetting)
{
    ui->setupUi(this);
    auto layout = new QHBoxLayout(this);
    this->setLayout(layout);
    auto label = new QLabel("QRCode");
    layout->addWidget(label);

    const auto refreshBtn = new QPushButton("refresh");
    layout->addWidget(refreshBtn);
    connect(refreshBtn, &QPushButton::clicked, [this, label]() {
        const auto login = BiliApi::BilibiliClient::globalClient().GetLoginUrl();
        m_qrcode_key = login.data.qrcode_key;
        refreshQrCode(label, login.data.url);
    });

    const auto btn = new QPushButton("check");
    layout->addWidget(btn);
    connect(btn, &QPushButton::clicked, [this]() {
        checkLoginStatus(m_qrcode_key);
    });
}

AccountSetting::~AccountSetting()
{
    delete ui;
}
void AccountSetting::refreshQrCode(QLabel* label, const std::string& url)
{
    QrCodeGenerator qrCodeGenerator;
    const auto image = qrCodeGenerator.generateQR(QString::fromStdString(url));
    label->setPixmap(QPixmap::fromImage(image));
}
void AccountSetting::checkLoginStatus(const std::string& qrcode)
{
    const auto loginStatus = BiliApi::BilibiliClient::globalClient().GetLoginStatus(qrcode);

    while (true)
    {
        if (loginStatus.code == 86038)
        {
            qDebug() << "QR code expired";
            break;
        }
        else if (loginStatus.code == 86101)
        {
            qDebug() << "Wait for scan code";
            continue;
        }
        else if (loginStatus.code == 86090)
        {
            qDebug() << "Waiting for confirmation";
        }
        else
        {
            BiliApi::BilibiliClient::globalClient().ParseCookie(loginStatus.data.url);
            break;

        }
    }

}
