//
// Created by Alanus Meminius on 2024/1/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_AccountSetting.h" resolved

#include "AccountSetting.h"
#include "ui_AccountSetting.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <SUI/QrCodeGenerator.h>
#include <QDebug>
#include <regex>
#include <QtWidgets/QPushButton>
AccountSetting::AccountSetting(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AccountSetting)
{
    ui->setupUi(this);
    auto layout = new QHBoxLayout(this);
    this->setLayout(layout);
    auto label = new QLabel("AccountSetting");
    layout->addWidget(label);

    const auto loginUrl = m_biliClient.GetLoginUrl();
    const std::string url = loginUrl.data.url;

    qDebug() << QString::fromStdString(url);
    // std::string Url = "https://passport.bilibili.com/h5-app/passport/login/scan?navhide=1\u0026qrcode_key=358b647748030c2547aa94fb17be6eed\u0026from=";
    QrCodeGenerator qrCodeGenerator;
    const auto image = qrCodeGenerator.generateQR(QString::fromStdString(url));
    label->setPixmap(QPixmap::fromImage(image));

    auto btn = new QPushButton("check");
    layout->addWidget(btn);
    connect(btn, &QPushButton::clicked, [this, loginUrl]() {
        checkLoginStatus(loginUrl.data.qrcode_key);
    });
}

AccountSetting::~AccountSetting()
{
    delete ui;
}
void AccountSetting::checkLoginStatus(const std::string& qrcode)
{
    qDebug() << QString::fromStdString(qrcode);
    const auto loginStatus = m_biliClient.GetLoginStatus(qrcode);
    // if (loginStatus.data.url != "")
    qDebug() << QString::fromStdString(loginStatus.data.message);
    qDebug() << loginStatus.message;

    while (true)
    {
        if (loginStatus.code == 86038)
        {
            qDebug() << "二维码过期";
            break;
        }
        else if (loginStatus.code == 86101)
        {
            qDebug() << "等待扫码";
            continue;
        }
        else if (loginStatus.code == 86090)
        {
            qDebug() << "等待确认";
        }
        else
        {
            std::string SESSDATA = BiliApi::BilibiliClient::getSESSData(loginStatus.data.url);
            break;

        }
    }

}
