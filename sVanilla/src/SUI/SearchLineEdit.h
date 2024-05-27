#pragma once

#include <QLineEdit>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
namespace Ui
{
class SearchLineEdit;
}
QT_END_NAMESPACE

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(QWidget* parent = nullptr);
    ~SearchLineEdit();

    void setFocusOutHide();

signals:
    void readyHide();
    void startHide();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();
    void startHideAnimation();

private:
    Ui::SearchLineEdit* ui;
    bool focusOutHide = false;
};
