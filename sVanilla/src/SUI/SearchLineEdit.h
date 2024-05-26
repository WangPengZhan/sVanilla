#pragma once

#include <QLineEdit>

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

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::SearchLineEdit* ui;
};

