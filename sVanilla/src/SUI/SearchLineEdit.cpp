#include <QTimer>
#include <QIcon>
#include <QAction>

#include "SearchLineEdit.h"
#include "BaseQt/Utility.h"

SearchLineEdit::SearchLineEdit(QWidget* parent)
    : QLineEdit(parent)
{
    setUi();
    signalsAndSlots();
}

SearchLineEdit::~SearchLineEdit() = default;

void SearchLineEdit::setFocusOutHide()
{
    focusOutHide = true;
}

void SearchLineEdit::focusOutEvent(QFocusEvent* event)
{
    if (focusOutHide)
    {
        emit startHide();
        startHideAnimation();
    }
    QLineEdit::focusOutEvent(event);
}

void SearchLineEdit::showEvent(QShowEvent* event)
{
    if (focusOutHide)
    {
        const auto maxWidth = width() * 2;
        util::animate(this, {0, maxWidth}, "maximumWidth", {}, 500);
    }
    QLineEdit::showEvent(event);
}

void SearchLineEdit::setUi()
{
    m_searchAction = addAction(QIcon(":/icon/search.svg"), LeadingPosition);
    m_clearAction = addAction(QIcon(":/icon/clear.svg"), TrailingPosition);
    m_clearAction->setVisible(false);
    setPlaceholderText(tr("Try to Search"));
}

void SearchLineEdit::signalsAndSlots()
{
    connect(this, &QLineEdit::textChanged, this, [this](const QString& text) {
        m_clearAction->setVisible(!text.isEmpty());
    });
    connect(m_clearAction, &QAction::triggered, this, [this] {
        this->clear();
        m_clearAction->setVisible(false);
    });
}

void SearchLineEdit::startHideAnimation()
{
    const auto maxWidth = this->width();
    const auto finished = [this, maxWidth]() {
        setMaximumWidth(maxWidth);
        hide();
        emit readyHide();
    };
    util::animate(this, {maxWidth, 0}, "maximumWidth", finished, 500);
}
