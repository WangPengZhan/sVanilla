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

const QWidget* WindowBar::getHitWidget() const
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
    return qobject_cast<QPushButton*>(ui->systemBtnLayout->itemAt(1)->widget());
}

bool WindowBar::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::WindowStateChange:
    {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget)
        {
            widget->isMaximized() ? maxButton()->setIcon(QIcon(":/icon/bar/restore.svg")) : maxButton()->setIcon(QIcon(":/icon/bar/maximize.svg"));
        }
        return true;
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
    ui->Hit->setIconList(horizonNavigation);
    ui->Hit->setColumnWidth(columnWidth);
    ui->Hit->setEnableBackground(false);
    ui->systemBtnLayout->setSpacing(columnSpacing);
}
