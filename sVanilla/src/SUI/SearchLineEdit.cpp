#include "ui_SearchLineEdit.h"
#include "SearchLineEdit.h"
#include "ClientUi/Event.h"

SearchLineEdit::SearchLineEdit(QWidget* parent)
    : QLineEdit(parent),
      ui(new Ui::SearchLineEdit())
{
    ui->setupUi(this);
    SetUi();

    SignalsAndSlots();

    SetEditFinishedSearch(false);
}

SearchLineEdit::~SearchLineEdit()
{
    delete ui;
}

void SearchLineEdit::SetEditFinishedSearch(bool enabled)
{
    if (enabled)
    {
        disconnect(this, &QLineEdit::editingFinished, this, &SearchLineEdit::Complete);
        connect(this, &QLineEdit::editingFinished, this, &SearchLineEdit::Complete);
    }
    else
    {
        disconnect(this, &QLineEdit::editingFinished, this, &SearchLineEdit::Complete);
    }
}

void SearchLineEdit::resizeEvent(QResizeEvent* event)
{
    ui->ClearBtn->resize(height(), height());
    ui->ClearBtn->move(width() - 60, 0);
    ui->SearchBtn->resize(height(), height());
    ui->SearchBtn->move(width() - 25, 0);
    return QLineEdit::resizeEvent(event);
}

void SearchLineEdit::SetUi()
{
    ui->ClearBtn->setVisible(false);
}

void SearchLineEdit::SignalsAndSlots()
{
    connect(ui->SearchBtn, &QPushButton::clicked, this, &SearchLineEdit::Complete);
    connect(this, &QLineEdit::textChanged, this, [this](const QString& text) {
        ui->ClearBtn->setVisible(!text.isEmpty());
    });

}
