//
// Created by Alanus Meminius on 2024/1/6.
//

#ifndef _DEFAULTSETTING_H_
#define _DEFAULTSETTING_H_

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class DefaultSetting;
}
QT_END_NAMESPACE

class DefaultSetting : public QWidget
{
    Q_OBJECT

public:
    explicit DefaultSetting(QWidget* parent = nullptr);
    ~DefaultSetting() override;

private:
    Ui::DefaultSetting* ui;
    void SignalsAndSlots();
};

#endif  //_DEFAULTSETTING_H_
