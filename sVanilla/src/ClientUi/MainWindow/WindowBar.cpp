#include "WindowBar.h"
#include "ui_WindowBar.h"

static constexpr int columnWidth = 45;
static constexpr int columnSpacing = 15;

WindowBar::WindowBar(QWidget* parent)
    : QFrame(parent)
    , ui(new Ui::WindowBar)
{
    ui->setupUi(this);
    signalsAndSlots();
    setUi();
}

WindowBar::~WindowBar()
{
    delete ui;
}

QWidget* WindowBar::getHitWidget() const
{
    return ui->Hit;
}

void WindowBar::setMinButton(QAbstractButton* btn)
{
    ui->systemBtnLayout->insertWidget(0, btn);
    connect(btn, &QAbstractButton::clicked, this, &WindowBar::minimizeRequested);
}

void WindowBar::setMaxButton(QAbstractButton* btn)
{
    ui->systemBtnLayout->insertWidget(1, btn);
    connect(btn, &QAbstractButton::clicked, this, &WindowBar::maximizeRequested);
}

QPushButton* WindowBar::maxButton()
{
    auto* item = ui->systemBtnLayout->itemAt(1);
    return item ? qobject_cast<QPushButton*>(item->widget()) : nullptr;
}

bool WindowBar::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::WindowStateChange:
    {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (auto* button = maxButton(); widget && button)
        {
            button->setIcon(QIcon(widget->isMaximized() ? ":/icon/bar/restore.svg" : ":/icon/bar/maximize.svg"));
        }
        return false;
    }
    default:
        break;
    }

    return QWidget::eventFilter(obj, event);
}

void WindowBar::setCloseButton(QAbstractButton* btn)
{
    ui->systemBtnLayout->insertWidget(2, btn);
    connect(btn, &QAbstractButton::clicked, this, &WindowBar::closeRequested);
}

void WindowBar::signalsAndSlots()
{
    connect(ui->Hit, &Vanilla::ToggleButton::currentItemChanged, this, &WindowBar::barBtnClick);
    connect(this, &WindowBar::tabChanged, ui->Hit, &Vanilla::ToggleButton::setCurrentIndex);
}

void WindowBar::setUi() const
{
    const QStringList horizonNavigation({QStringLiteral(":/icon/bar/home.svg"), QStringLiteral(":/icon/bar/video.svg"),
                                         QStringLiteral(":/icon/bar/download.svg"), QStringLiteral(":/icon/bar/setting.svg")});
    const QStringList toolTips{tr("home"), tr("video"), tr("download"), tr("settings")};
    ui->Hit->setIconList(horizonNavigation);
    ui->Hit->setToolTips(toolTips);
    ui->Hit->setColumnWidth(columnWidth);
    ui->Hit->setEnableBackground(false);
    ui->systemBtnLayout->setSpacing(columnSpacing);
}
