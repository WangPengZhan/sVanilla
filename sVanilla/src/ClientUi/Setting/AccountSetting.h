#ifndef _ACCOUNTSETTING_H_
#define _ACCOUNTSETTING_H_

#include <QWidget>
#include <QLabel>

#include "BiliApi/BilibiliClient.h"

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
    void refreshQrCode(QLabel* label, const std::string& url);
    void checkLoginStatus(const std::string& qrcode);

private:
    Ui::AccountSetting* ui;
    std::string m_qrcode_key;

};

#endif  //_ACCOUNTSETTING_H_
