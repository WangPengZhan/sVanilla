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
    ui->btnSearch->resize(height(), height());
    ui->btnSearch->move(width() - height(), 0);
    return QLineEdit::resizeEvent(event);
}

void SearchLineEdit::SetUi()
{
}

void SearchLineEdit::SignalsAndSlots()
{
    connect(ui->btnSearch, &QPushButton::clicked, this, &SearchLineEdit::searchCliecked);
    connect(ui->btnSearch, &QPushButton::clicked, this, [this]() {
        qDebug() << this->text();
        emit Event::getInstance()->addSingleUri(this->text().toStdString());
    });
}
