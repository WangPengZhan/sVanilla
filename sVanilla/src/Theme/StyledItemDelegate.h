#pragma once
#include <QStyledItemDelegate>
#include <QPainter>

class CustomVideoListItemDelegate : public QStyledItemDelegate
{
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};