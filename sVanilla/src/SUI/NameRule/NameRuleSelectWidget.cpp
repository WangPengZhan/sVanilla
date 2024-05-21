#include <QPainter>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>

#include "NameRuleSelectWidget.h"

#include <QApplication>

NameRule::NameRule(QWidget* parent)
    : QWidget(parent)
{
}

void NameRule::setRule(const QString& rule)
{
    m_rule = rule;
    const QFontMetrics fm(font());
    m_width = fm.horizontalAdvance(rule) + 2 * m_padding;
    m_height = fm.height() + 2 * m_padding;
    resize(m_width, m_height);
}

void NameRule::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);
    const auto textRect = rect().adjusted(m_padding, m_padding, -m_padding, -m_padding);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, m_rule);
}

QSize NameRule::sizeHint() const
{
    return {m_width, m_height};
}

NameRuleSelectWidget::NameRuleSelectWidget(QWidget* parent)
    : QListWidget(parent)
{
    setUi();
}

void NameRuleSelectWidget::setUi()
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setEditTriggers(NoEditTriggers);
    setSelectionMode(NoSelection);
    setDragEnabled(true);
    setDragDropMode(DragOnly);
    setWrapping(false);
    setFlow(LeftToRight);
    setResizeMode(Adjust);
}

void NameRuleSelectWidget::setNameRules(const std::vector<QString>& rules)
{
    clear();
    for (const auto& item : rules)
    {
        makeItem(item);
    }
    adjustSize();
    constexpr int maxHeight = 30;
    setMaximumHeight(maxHeight);

}

void NameRuleSelectWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragPoint = event->pos();
        m_dragItem = this->itemAt(event->pos());
    }
    QListWidget::mousePressEvent(event);
}

void NameRuleSelectWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() != Qt::LeftButton)
    {
        return;
    }
    if (const QPoint temp = event->pos() - m_dragPoint; temp.manhattanLength() < QApplication::startDragDistance())
    {
        return;
    }
    auto* drag = new QDrag(this);
    auto* mimeData = new QMimeData;
    mimeData->setText(m_dragItem->data(Qt::UserRole + 1).toString());
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction | Qt::MoveAction);
    QListWidget::mouseMoveEvent(event);
}


void NameRuleSelectWidget::makeItem(const QString& name)
{
    auto* item = new QListWidgetItem(this);
    item->setData(Qt::UserRole + 1, name);
    auto *const nameRule = new NameRule(this);
    nameRule->setRule(name);
    item->setSizeHint(nameRule->sizeHint());

    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
    setItemWidget(item, nameRule);
}

void NameRuleSelectWidget::removeItem(const QListWidgetItem* item)
{
    const auto row = indexFromItem(item).row();
    auto *const widgetItem = takeItem(row);
    removeItemWidget(widgetItem);
    delete widgetItem;
}