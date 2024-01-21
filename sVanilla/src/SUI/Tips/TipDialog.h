#pragma once

#include <QDialog>

namespace Ui
{
class TipDialog;
}

class TipDialog : public QDialog
{
    Q_OBJECT
public:
    enum Level
    {
        Success,
        Info,
        Warn,
        Error,
    };

    explicit TipDialog(QWidget* parent = nullptr);
    ~TipDialog();

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::TipDialog* ui;
};
