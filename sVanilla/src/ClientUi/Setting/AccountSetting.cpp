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
    auto label = new QLabel("QRCode");
    layout->addWidget(label);

    const auto refreshBtn = new QPushButton("refresh");
    layout->addWidget(refreshBtn);
    connect(refreshBtn, &QPushButton::clicked, [this, label]() {
        const auto login = m_biliClient.GetLoginUrl();
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
    const auto image =  qrCodeGenerator.generateQR(QString::fromStdString(url));
    label->setPixmap(QPixmap::fromImage(image));
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
            m_biliClient.ParseCookie(loginStatus.data.url);
            qDebug() << "登录成功, cookie:\n" << QString::fromStdString(m_biliClient.m_cookie);
            break;

        }
    }

}
