//
// Created by Alanus Meminius on 2024/1/7.
//

#ifndef _ACCOUNTSETTING_H_
#define _ACCOUNTSETTING_H_

#include "BiliApi/BilibiliClient.h"

#include <QWidget>
#include <QtWidgets/qlabel.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AccountSetting;
}
QT_END_NAMESPACE

class AccountSetting : public QWidget
{
    Q_OBJECT

public:
    explicit AccountSetting(QWidget* parent = nullptr);
    ~AccountSetting() override;

private:
    Ui::AccountSetting* ui;

    std::string m_qrcode_key;
    void refreshQrCode(QLabel* label, const std::string& url);
    void checkLoginStatus(const std::string& qrcode);
    BiliApi::BilibiliClient& m_biliClient = BiliApi::BilibiliClient::globalClient();
};

#endif  //_ACCOUNTSETTING_H_
