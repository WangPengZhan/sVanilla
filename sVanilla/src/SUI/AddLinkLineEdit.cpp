#include <QPushButton>
#include <QContextMenuEvent>
#include <QPainter>
#include <QMenu>
#include <QEvent>
#include <QTimer>

#include "AddLinkLineEdit.h"

static constexpr int iconMargin = 26;
static constexpr int padding = 5;
static std::unordered_map<QString, QString> contextMenuIcon{
    {"&Undo",      ":/icon/common/undo.svg"     },
    {"&Redo",      ":/icon/common/redo.svg"     },
    {"Cu&t",       ":/icon/common/cut.svg"      },
    {"&Copy",      ":/icon/common/copy.svg"     },
    {"Delete",     ":/icon/common/delete.svg"   },
    {"&Paste",     ":/icon/common/paste.svg"    },
    {"Select All", ":/icon/common/selectAll.svg"}
};

AddLinkLineEdit::AddLinkLineEdit(QWidget* parent)
    : QLineEdit(parent)
    , m_moreMenu(new QMenu(this))
{
    setUi();
    signalsAndSlots();
    SetEditFinishedSearch(true);
}

AddLinkLineEdit::~AddLinkLineEdit() = default;

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
    m_webSiteAction->setIcon(QIcon(iconPath));
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

bool AddLinkLineEdit::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_moreMenu && event->type() == QEvent::MouseButtonRelease)
    {
        auto action = qobject_cast<QMenu*>(watched)->activeAction();
        if (action && action->isCheckable())
        {
            action->trigger();
            return true;
        }
    }
    if (watched == m_moreMenu && event->type() == QEvent::Hide)
    {
        m_moreAction->setChecked(false);
    }
    return QLineEdit::eventFilter(watched, event);
}

void AddLinkLineEdit::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = createStandardContextMenu();
    for (QAction* action : menu->actions())
    {
        if (contextMenuIcon.contains(action->text()))
        {
            action->setIcon(QIcon(contextMenuIcon[action->text()]));
        }
    }
    menu->exec(event->globalPos());
    delete menu;
}

void AddLinkLineEdit::setUi()
{
    m_webSiteAction = addAction(QIcon(":icon/website/default.svg"), LeadingPosition);
    QIcon moreIcon;
    moreIcon.addFile(QString::fromUtf8(":/icon/setting/down.svg"), QSize(), QIcon::Normal, QIcon::Off);
    moreIcon.addFile(QString::fromUtf8(":/icon/setting/up.svg"), QSize(), QIcon::Normal, QIcon::On);
    m_moreAction = addAction(moreIcon, TrailingPosition);
    m_moreAction->setCheckable(true);
    m_enterAction = addAction(QIcon(":icon/home/enter.svg"), TrailingPosition);
    m_clearAction = addAction(QIcon(":icon/home/clear.svg"), TrailingPosition);

    m_clearAction->setVisible(false);
    m_moreAction->setCheckable(true);
    m_moreAction->setChecked(false);
    createMoreMenu();
    m_moreMenu->installEventFilter(this);
}

void AddLinkLineEdit::signalsAndSlots()
{
    connect(m_enterAction, &QAction::triggered, this, &AddLinkLineEdit::Complete);
    connect(this, &QLineEdit::textChanged, this, [this](const QString& text) {
        m_clearAction->setVisible(!text.isEmpty());
    });
    connect(m_clearAction, &QAction::triggered, this, [this] {
        this->clear();
        m_clearAction->setVisible(false);
    });
    connect(m_moreAction, &QAction::toggled, this, &AddLinkLineEdit::btnMoreClick);
}

void AddLinkLineEdit::btnMoreClick(bool checked)
{
    if (checked)
    {
        const auto menuX = width() - m_moreMenu->sizeHint().width();
        const QPoint pos = mapToGlobal(QPoint(menuX, height()));
        m_moreMenu->exec(pos);
    }
    else
    {
        m_moreMenu->hide();
    }
}

void AddLinkLineEdit::createMoreMenu()
{
    auto* const doNotParse = new QAction("Don't Parse Playlist\t⌘ P", this);
    doNotParse->setCheckable(true);
    m_moreMenu->addAction(doNotParse);

    auto* const advancedDownload = new QAction("Advanced Download\t⌘ V", this);
    m_moreMenu->addAction(advancedDownload);
}

void AddLinkLineEdit::setMoreBtnIcon(bool checked)
{
    m_moreAction->setIcon(QIcon(checked ? ":/icon/setting/up.svg" : ":/icon/setting/down.svg"));
}