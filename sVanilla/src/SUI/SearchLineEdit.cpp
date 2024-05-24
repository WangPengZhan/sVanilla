#include <QPushButton>

#include "ui_SearchLineEdit.h"
#include "SearchLineEdit.h"

static constexpr int iconMargin = 30;

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

void SearchLineEdit::setWebsiteIcon(const QString& iconPath) const
{
    ui->btnWebsite->setIcon(QIcon(iconPath));
}

void SearchLineEdit::resizeEvent(QResizeEvent* event)
{
    ui->btnWebsite->resize(height(), height());
    ui->btnWebsite->move(0, 0);
    ui->btnClear->resize(height(), height());
    ui->btnClear->move(width() - 2 * iconMargin, 0);
    ui->btnSearch->resize(height(), height());
    ui->btnSearch->move(width() - iconMargin, 0);
    QLineEdit::resizeEvent(event);
}

void SearchLineEdit::setUi()
{
    setTextMargins(iconMargin - 5, 0, 2 * iconMargin, 0);
    ui->btnClear->setVisible(false);
}

void SearchLineEdit::signalsAndSlots()
{
    connect(ui->btnSearch, &QPushButton::clicked, this, &SearchLineEdit::Complete);
    connect(this, &QLineEdit::textChanged, this, [this](const QString& text) {
        ui->btnClear->setVisible(!text.isEmpty());
    });
    connect(ui->btnClear, &QPushButton::clicked, this, [this] {
        this->clear();
        ui->btnClear->setVisible(false);
    });
}
