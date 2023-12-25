#pragma once

#include <QDialog>

namespace Ui
{
class TipDialog;
};

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

    TipDialog(QWidget* parent = nullptr);
    ~TipDialog();

private:
    void SetUi();
    void SignalsAndSlots();

private:
    Ui::TipDialog* ui;
};
