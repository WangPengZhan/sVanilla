//
// Created by Alanus Meminius on 2024/1/5.
//

#ifndef _WINDOWBAR_H_
#define _WINDOWBAR_H_

#include <QFrame>
#include <QToolButton>
QT_BEGIN_NAMESPACE
namespace Ui
{
class WindowBar;
}
QT_END_NAMESPACE

class WindowBar : public QFrame
{
    Q_OBJECT

public:
    explicit WindowBar(QWidget* parent = nullptr);
    ~WindowBar() override;
    [[nodiscard]] QWidget* GetHitWidget() const;

private:
    Ui::WindowBar* ui;
    void SignalsAndSlots();
    QButtonGroup* m_barBtnGroup;
};

#endif  //_WINDOWBAR_H_
