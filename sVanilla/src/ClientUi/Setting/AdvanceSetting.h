#ifndef _ADVANCESETTING_H_
#define _ADVANCESETTING_H_

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AdvanceSetting;
}
QT_END_NAMESPACE

class AdvanceSetting : public QWidget
{
    Q_OBJECT

public:
    explicit AdvanceSetting(QWidget* parent = nullptr);
    ~AdvanceSetting() override;

private:
    Ui::AdvanceSetting* ui;
};

#endif  //_ADVANCESETTING_H_
