#include <QDragEnterEvent>
#include <QMimeData>

#include "NameRuleEditWidget.h"

NameRuleEditWidget::NameRuleEditWidget(QWidget* parent)
    : QLineEdit(parent)
{
    setAcceptDrops(true);
}

void NameRuleEditWidget::dragMoveEvent(QDragMoveEvent* event)
{
    event->setDropAction(Qt::MoveAction);
    event->accept();
}

void NameRuleEditWidget::dragEnterEvent(QDragEnterEvent* event)
{
    event->setDropAction(Qt::MoveAction);
    event->accept();
}

void NameRuleEditWidget::dropEvent(QDropEvent* event)
{
    if (const auto rule = event->mimeData()->text(); !rule.isEmpty())
    {
        appendRule(rule);
        event->accept();
    }
}

void NameRuleEditWidget::appendRule(const QString& rule)
{
    const auto previousText = text();
    const auto addText = previousText + "$" + rule + "$";
    setText(addText);
    emit textChanged(addText);
}
