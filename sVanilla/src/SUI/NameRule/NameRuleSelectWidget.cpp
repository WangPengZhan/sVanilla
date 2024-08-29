#include <QPainter>
#include <QMouseEvent>
#include <QMimeData>
#include <QApplication>

#include "NameRuleSelectWidget.h"


NameRuleSelectWidget::NameRuleSelectWidget(QWidget* parent)
    : QListWidget(parent)
{
    setUi();
}

void NameRuleSelectWidget::setUi()
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setEditTriggers(NoEditTriggers);
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
        const auto textItem = new QListWidgetItem(item);
        textItem->setSizeHint(QSize(-1, 30));
        addItem(textItem);
    }
    setMaximumHeight(35);
    adjustSize();
}

QMimeData* NameRuleSelectWidget::mimeData(const QList<QListWidgetItem*>& items) const
{
    const auto mimeData = new QMimeData();
    QStringList itemTexts;
    for (const QListWidgetItem *item : items)
    {
        itemTexts << item->text();
    }
    mimeData->setText(itemTexts.join("\n"));
    return mimeData;
}