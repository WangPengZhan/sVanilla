#pragma once
#include <QLineEdit>

namespace Ui
{
class SearchLineEdit;
};

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(QWidget* parent = nullptr);
    ~SearchLineEdit();

    void SetEditFinishedSearch(bool enabled);

protected:
    void resizeEvent(QResizeEvent* event) override;

signals:
    void Complete();

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::SearchLineEdit* ui;
};
