#include "ui_SearchLineEdit.h"
#include "SearchLineEdit.h"

SearchLineEdit::SearchLineEdit(QWidget* parent)
    : QLineEdit(parent)
    , ui(new Ui::SearchLineEdit())
{
    ui->setupUi(this);
    setUi();

    signalsAndSlots();

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
    ui->ClearBtn->move(width() - 30, 0);
    //    ui->SearchBtn->resize(height(), height());
    //    ui->SearchBtn->move(width() - 25, 0);
    return QLineEdit::resizeEvent(event);
}

void SearchLineEdit::setUi()
{
    ui->ClearBtn->setVisible(false);
    ui->ClearBtn->setIcon(QIcon(":/icon/home/clear.svg"));
}

void SearchLineEdit::signalsAndSlots()
{
//    connect(ui->SearchBtn, &QPushButton::clicked, this, &SearchLineEdit::Complete);
    connect(this, &QLineEdit::textChanged, this, [this](const QString& text) {
        ui->ClearBtn->setVisible(!text.isEmpty());
    });
}
