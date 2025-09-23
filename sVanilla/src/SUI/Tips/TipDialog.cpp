#include "ui_TipDialog.h"
#include "TipDialog.h"

#include <QMouseEvent>

TipDialog::TipDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::TipDialog())
{
    ui->setupUi(this);
    setUi();

    signalsAndSlots();
}

TipDialog::~TipDialog()
{
    delete ui;
}

void TipDialog::showTip(const QString& text, Level level)
{
    setTipText(text);
    setLevel(level);
}

TipDialog::Level TipDialog::level() const
{
    return m_level;
}

void TipDialog::setLevel(Level level)
{
    m_level = level;

    switch (m_level)
    {
    case Success:
    {
        setWindowTitle(tr("Success"));
        ui->labelIcon->setStyleSheet("image: url(:/icon/tip/success.svg);");
        break;
    }
    case Info:
    {
        setWindowTitle(tr("Info"));
        ui->labelIcon->setStyleSheet("image: url(:/icon/tip/info.svg);");
        break;
    }
    case Warn:
    {
        setWindowTitle(tr("Warn"));
        ui->labelIcon->setStyleSheet("image: url(:/icon/tip/warn.svg);");
        break;
    }
    case Error:
    {
        setWindowTitle(tr("Error"));
        ui->labelIcon->setStyleSheet("image: url(:/icon/tip/error.svg);");
        break;
    }
    default:
        break;
    }
}

void TipDialog::setTipText(const QString& text)
{
    ui->labelText->setText(text);
}

QString TipDialog::tipText() const
{
    return ui->labelText->text();
}

void TipDialog::setNoCancelButton(bool noCancel)
{
    ui->btnNo->setVisible(!noCancel);
}
bool TipDialog::noCancelButton() const
{
    return !ui->btnNo->isVisible();
}

void TipDialog::setNoOkButton(bool noOk)
{
    ui->btnOk->setVisible(!noOk);
}

bool TipDialog::noOkButton() const
{
    return !ui->btnOk->isVisible();
}

void TipDialog::setUi()
{
}

void TipDialog::signalsAndSlots()
{
    connect(ui->btnOk, &QPushButton::clicked, this, &TipDialog::accept);
    connect(ui->btnNo, &QPushButton::clicked, this, &TipDialog::reject);
}
