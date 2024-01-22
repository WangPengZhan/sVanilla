//
// Created by Alanus Meminius on 2024/1/7.
//

#ifndef _ACCOUNTSETTING_H_
#define _ACCOUNTSETTING_H_

#include <QWidget>

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
};

#endif  //_ACCOUNTSETTING_H_
