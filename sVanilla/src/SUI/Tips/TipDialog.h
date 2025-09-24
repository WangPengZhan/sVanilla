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

    void showTip(const QString& text, Level level = Info);

    Level level() const;
    void setLevel(Level level);

    void setTipText(const QString& text);
    QString tipText() const;

    void setNoCancelButton(bool noCancel);
    bool noCancelButton() const;

    void setNoOkButton(bool noOk);
    bool noOkButton() const;

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::TipDialog* ui;
    Level m_level{};
};
