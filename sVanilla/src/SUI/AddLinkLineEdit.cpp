#include <QPushButton>
#include <QPainter>

#include "ui_AddLinkLineEdit.h"
#include "AddLinkLineEdit.h"

static constexpr int iconMargin = 30;
static constexpr int padding = 5;

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
    const auto iconSize = QSize(height(), height());
    resizeIcons(iconSize);
    ui->btnWebsite->move(0, 0);
    ui->btnClear->move(width() - 3 * iconMargin - padding, 0);
    ui->btnSearch->move(width() - 2 * iconMargin - padding, 0);
    ui->btnMore->move(width() - iconMargin, 0);

    QLineEdit::resizeEvent(event);
}

void AddLinkLineEdit::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::lightGray, 1.5));
    const auto dividingX = rect().right() - iconMargin;
    painter.drawLine(dividingX, rect().top() + padding + 1, dividingX, rect().bottom() - padding);
}

void AddLinkLineEdit::setUi()
{
    setTextMargins(iconMargin - 5, 0, 2 * iconMargin, 0);
    ui->btnClear->setVisible(false);
    connect(ui->btnMore, &QPushButton::clicked, this, [this]() {
        const auto upIcon = QIcon(QString(":/icon/setting/up.svg"));
        const auto downIcon = QIcon(QString(":/icon/setting/down.svg"));
        ui->btnMore->setIcon(ui->btnMore->isChecked() ? upIcon : downIcon);
    });
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

void AddLinkLineEdit::resizeIcons(const QSize& size)
{
    ui->btnWebsite->resize(size);
    ui->btnClear->resize(size);
    ui->btnSearch->resize(size);
    ui->btnMore->resize(size);
}
