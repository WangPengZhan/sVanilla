#include "StyledItemDelegate.h"
#include <QPainterPath>


void CustomVideoListItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();
    QRect rect = option.rect;
    QPainterPath path;
    path.addRoundedRect(rect.adjusted(0, 3, 0, -3), 5, 5); // 设置圆角半径为10
    if (index.row() % 2 == 0) {
        painter->fillPath(path, QColor(200, 200, 200)); // 偶数行的背景色为浅灰色
    } else {
        painter->fillPath(path, QColor(220, 220, 220)); // 奇数行的背景色为淡灰色
    }
    QStyleOptionViewItem opt = option;
    opt.rect = rect;
    QStyledItemDelegate::paint(painter, opt, index);
    painter->restore();
}