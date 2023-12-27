#pragma once

#include <QWidget>

namespace Ui
{
class Toast;
};

class Toast : public QWidget
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

    Toast(QWidget* parent = nullptr);
    ~Toast();

private:
    void SetUi();
    void SignalsAndSlots();

private:
    Ui::Toast* ui;
};
