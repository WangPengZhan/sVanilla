
#include "SearchLineEdit.h"
#include "ui_SearchLineEdit.h"

static constexpr int iconMargin = 30;

SearchLineEdit::SearchLineEdit(QWidget* parent)
    : QLineEdit(parent)
    , ui(new Ui::SearchLineEdit)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

SearchLineEdit::~SearchLineEdit()
{
    delete ui;
}

void SearchLineEdit::resizeEvent(QResizeEvent* event)
{
    ui->btnSearch->resize(height(), height());
    ui->btnSearch->move(0, 0);
    ui->btnClear->resize(height(), height());
    ui->btnClear->move(width() - iconMargin, 0);
    QLineEdit::resizeEvent(event);
}

void SearchLineEdit::setUi()
{
    setTextMargins(iconMargin - 5, 0, iconMargin, 0);
    ui->btnClear->setVisible(false);
    setPlaceholderText(tr("Try to Search"));
}

void SearchLineEdit::signalsAndSlots()
{
    connect(this, &QLineEdit::textChanged, this, [this](const QString& text) {
        ui->btnClear->setVisible(!text.isEmpty());
    });
    connect(ui->btnClear, &QPushButton::clicked, this, [this] {
        this->clear();
        ui->btnClear->setVisible(false);
    });
}
