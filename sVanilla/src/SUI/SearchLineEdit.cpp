#include "ui_SearchLineEdit.h"
#include "SearchLineEdit.h"

SearchLineEdit::SearchLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , ui(new Ui::SearchLineEdit())
{
    ui->setupUi(this);
    SetUi();

    SignalsAndSlots();

    SetEditFinishedSearch(true);
}

SearchLineEdit::~SearchLineEdit()
{
    delete ui;
}

void SearchLineEdit::SetEditFinishedSearch(bool enabled)
{
    if (enabled)
    {
        disconnect(this, &QLineEdit::editingFinished, this, &SearchLineEdit::searchCliecked);
        connect(this, &QLineEdit::editingFinished, this, &SearchLineEdit::searchCliecked);
    }
    else
    {
        disconnect(this, &QLineEdit::editingFinished, this, &SearchLineEdit::searchCliecked);
    }
}

void SearchLineEdit::resizeEvent(QResizeEvent* event)
{
    ui->btnSearch->resize(height() - 8, height() - 8);
    ui->btnSearch->move(width() - 42, 4);
    return QLineEdit::resizeEvent(event);
}

void SearchLineEdit::SetUi()
{
    
}

void SearchLineEdit::SignalsAndSlots()
{
    connect(ui->btnSearch, &QPushButton::clicked, this, &SearchLineEdit::searchCliecked);
}
