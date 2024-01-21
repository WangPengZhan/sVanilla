#pragma once

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
signals:
    void BarBtnClick(int index);

private:
    Ui::WindowBar* ui;
    void SignalsAndSlots();
    QButtonGroup* m_barBtnGroup;
};

