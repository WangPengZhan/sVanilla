#include <QTimer>

#include "SearchLineEdit.h"
#include "ClientUi/Utils/Utility.h"

SearchLineEdit::SearchLineEdit(QWidget* parent)
    : QLineEdit(parent)
{
    setUi();
    signalsAndSlots();
}

SearchLineEdit::~SearchLineEdit()=default;

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
        util::moveAnimate(this, {geometry().topRight(), geometry().topLeft()});
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
    util::MoveStartEndValue changeValue = {geometry().topLeft(), geometry().topRight()};
    const auto animationFinished = [this]() {
        hide();
        emit readyHide();
    };
    util::moveAnimate(this, changeValue, animationFinished);
}
