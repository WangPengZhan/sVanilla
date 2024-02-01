#pragma once
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
    ~WindowBar();

    [[nodiscard]] QWidget* GetHitWidget() const;

private:
    void signalsAndSlots();

signals:
    void BarBtnClick(int index);

private:
    Ui::WindowBar* ui;
};
