#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class About;
}
QT_END_NAMESPACE

class About : public QWidget
{
    Q_OBJECT

public:
    explicit About(QWidget* parent = nullptr);
    ~About();

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::About* ui;
};
