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

    explicit Toast(QWidget* parent = nullptr);
    ~Toast();

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::Toast* ui;
};
