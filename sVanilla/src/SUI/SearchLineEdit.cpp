#include "ui_SearchLineEdit.h"
#include "SearchLineEdit.h"
#include "ClientUi/Event.h"

SearchLineEdit::SearchLineEdit(QWidget* parent)
    : QLineEdit(parent)
    , ui(new Ui::SearchLineEdit())
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
    ui->ClearBtn->resize(height(), height());
    ui->ClearBtn->move(width() - height()*2, 0);
    ui->SearchBtn->resize(height(), height());
    ui->SearchBtn->move(width() - height(), 0);
    return QLineEdit::resizeEvent(event);
}

void SearchLineEdit::SetUi()
{
}

void SearchLineEdit::SignalsAndSlots()
{
    connect(ui->SearchBtn, &QPushButton::clicked, this, &SearchLineEdit::searchCliecked);
    connect(ui->ClearBtn, &QPushButton::clicked, this, &SearchLineEdit::clear);
    connect(this, &SearchLineEdit::searchCliecked, this, [this]() {
        qDebug() << this->text();
        emit Event::getInstance() -> AddUri({this->text().toStdString()});
    });
}
