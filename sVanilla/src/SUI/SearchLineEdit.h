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
    ~SearchLineEdit() override;
    void SetEditFinishedSearch(bool enabled);

protected:
    void resizeEvent(QResizeEvent* event) override;

signals:
    void searchCliecked();

private:
    void SetUi();
    void SignalsAndSlots();

private:
    Ui::SearchLineEdit* ui;
};
