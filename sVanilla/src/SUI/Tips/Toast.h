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

    static void showToast(const QString &msg, Level level = Info, int timeout = 3000, QWidget *parent  = nullptr);
    void showWithAnimation(int timeout);

private:
    void setUi();
    void signalsAndSlots();
    void setText(const QString &msg);

    Level m_level = Info;
private:
    Ui::Toast* ui;

protected:
    void paintEvent(QPaintEvent *event) override;
};
