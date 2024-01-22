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

    void setMinButton(QAbstractButton* btn);
    void setMaxButton(QAbstractButton* btn);
    void setCloseButton(QAbstractButton* btn);
private:
    void signalsAndSlots();

signals:
    void BarBtnClick(int index);

    Q_SIGNALS:
    void minimizeRequested();
    void maximizeRequested(bool max = false);
    void closeRequested();

private:
    Ui::WindowBar* ui;
    QButtonGroup* m_barBtnGroup;
};
