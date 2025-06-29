#include <QDragEnterEvent>
#include <QMimeData>

#include "NameRuleEditWidget.h"

NameRuleEditWidget::NameRuleEditWidget(QWidget* parent)
    : QLineEdit(parent)
{
    setAcceptDrops(true);
}

void NameRuleEditWidget::setRules(const QMap<QString, QString>& rules)
{
    m_rules = rules;
}

const QMap<QString, QString>& NameRuleEditWidget::rules() const
{
    return m_rules;
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
    QString showRule = rule;
    if (m_rules.find(rule) != m_rules.end())
    {
        showRule = m_rules[rule];
    }

    const auto addText = text() + showRule;
    setText(addText);
    emit textChanged(addText);
}
