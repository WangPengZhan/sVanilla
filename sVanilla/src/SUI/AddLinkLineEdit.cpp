#include <QPushButton>

#include "ui_AddLinkLineEdit.h"
#include "AddLinkLineEdit.h"

static constexpr int iconMargin = 30;

AddLinkLineEdit::AddLinkLineEdit(QWidget* parent)
    : QLineEdit(parent)
    , ui(new Ui::AddLinkLineEdit())
{
    ui->setupUi(this);
    setUi();

    signalsAndSlots();

    SetEditFinishedSearch(true);
}

AddLinkLineEdit::~AddLinkLineEdit()
{
    delete ui;
}

void AddLinkLineEdit::SetEditFinishedSearch(bool enabled)
{
    if (enabled)
    {
        disconnect(this, &QLineEdit::editingFinished, this, &AddLinkLineEdit::Complete);
        connect(this, &QLineEdit::editingFinished, this, &AddLinkLineEdit::Complete);
    }
    else
    {
        disconnect(this, &QLineEdit::editingFinished, this, &AddLinkLineEdit::Complete);
    }
}

void AddLinkLineEdit::setWebsiteIcon(const QString& iconPath) const
{
    ui->btnWebsite->setIcon(QIcon(iconPath));
}

void AddLinkLineEdit::resizeEvent(QResizeEvent* event)
{
    ui->btnWebsite->resize(height(), height());
    ui->btnWebsite->move(0, 0);
    ui->btnClear->resize(height(), height());
    ui->btnClear->move(width() - 2 * iconMargin, 0);
    ui->btnSearch->resize(height(), height());
    ui->btnSearch->move(width() - iconMargin, 0);
    QLineEdit::resizeEvent(event);
}

void AddLinkLineEdit::setUi()
{
    setTextMargins(iconMargin - 5, 0, 2 * iconMargin, 0);
    ui->btnClear->setVisible(false);
}

void AddLinkLineEdit::signalsAndSlots()
{
    connect(ui->btnSearch, &QPushButton::clicked, this, &AddLinkLineEdit::Complete);
    connect(this, &QLineEdit::textChanged, this, [this](const QString& text) {
        ui->btnClear->setVisible(!text.isEmpty());
    });
    connect(ui->btnClear, &QPushButton::clicked, this, [this] {
        this->clear();
        ui->btnClear->setVisible(false);
    });
}
