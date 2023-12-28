#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QRect>
#include <QRubberBand>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QMdiSubWindow>
#include <QApplication>
#include <QDebug>

#include "TitleWidget.h"

class PrivateFramelessWidgetData;

/*****
 * FramelessHelperPrivate
 * 存储界面对应的数据集合，以及是否可移动、可缩放属性
 *****/
class FramelessHelper::PrivateData
{
    QTLIKE_DECLARE_PUBLIC(FramelessHelper)
    friend class PrivateFramelessWidgetData;

public:
    PrivateData(FramelessHelper* p);
    QHash<QWidget*, PrivateFramelessWidgetData*> m_widgetDataHash;
    bool m_bWidgetMovable : true;
    bool m_bWidgetResizable : true;
    bool m_bRubberBandOnResize : true;
    bool m_bRubberBandOnMove : true;
    bool m_bIsMaximum : true;
};

FramelessHelper::PrivateData::PrivateData(FramelessHelper* p) : q_ptr(p)
{
}

/*****
 * CursorPosCalculator
 * 计算鼠标是否位于左、上、右、下、左上角、左下角、右上角、右下角
 *****/
class SAPrivateFramelessCursorPosCalculator
{
public:
    explicit SAPrivateFramelessCursorPosCalculator();
    void reset();
    void recalculate(const QPoint& globalMousePos, const QRect& frameRect);

public:
    bool m_bOnEdges : true;
    bool m_bOnLeftEdge : true;
    bool m_bOnRightEdge : true;
    bool m_bOnTopEdge : true;
    bool m_bOnBottomEdge : true;
    bool m_bOnTopLeftEdge : true;
    bool m_bOnBottomLeftEdge : true;
    bool m_bOnTopRightEdge : true;
    bool m_bOnBottomRightEdge : true;

    static int m_nBorderWidth;
    static int m_nTitleHeight;
};

int SAPrivateFramelessCursorPosCalculator::m_nBorderWidth = 5;
int SAPrivateFramelessCursorPosCalculator::m_nTitleHeight = 30;

/***** CursorPosCalculator *****/
SAPrivateFramelessCursorPosCalculator::SAPrivateFramelessCursorPosCalculator()
{
    reset();
}

void SAPrivateFramelessCursorPosCalculator::reset()
{
    m_bOnEdges = false;
    m_bOnLeftEdge = false;
    m_bOnRightEdge = false;
    m_bOnTopEdge = false;
    m_bOnBottomEdge = false;
    m_bOnTopLeftEdge = false;
    m_bOnBottomLeftEdge = false;
    m_bOnTopRightEdge = false;
    m_bOnBottomRightEdge = false;
}

void SAPrivateFramelessCursorPosCalculator::recalculate(const QPoint& gMousePos, const QRect& frameRect)
{
    int globalMouseX = gMousePos.x();
    int globalMouseY = gMousePos.y();

    int frameX = frameRect.x();
    int frameY = frameRect.y();

    int frameWidth = frameRect.width();
    int frameHeight = frameRect.height();

    m_bOnLeftEdge = (globalMouseX >= frameX && globalMouseX <= frameX + m_nBorderWidth);

    m_bOnRightEdge = (globalMouseX >= frameX + frameWidth - m_nBorderWidth && globalMouseX <= frameX + frameWidth);

    m_bOnTopEdge = (globalMouseY >= frameY && globalMouseY <= frameY + m_nBorderWidth);

    m_bOnBottomEdge = (globalMouseY >= frameY + frameHeight - m_nBorderWidth && globalMouseY <= frameY + frameHeight);

    m_bOnTopLeftEdge = m_bOnTopEdge && m_bOnLeftEdge;
    m_bOnBottomLeftEdge = m_bOnBottomEdge && m_bOnLeftEdge;
    m_bOnTopRightEdge = m_bOnTopEdge && m_bOnRightEdge;
    m_bOnBottomRightEdge = m_bOnBottomEdge && m_bOnRightEdge;

    m_bOnEdges = m_bOnLeftEdge || m_bOnRightEdge || m_bOnTopEdge || m_bOnBottomEdge;
}

/*****
 * WidgetData
 * 更新鼠标样式、移动窗体、缩放窗体
 *****/
class PrivateFramelessWidgetData
{
public:
    explicit PrivateFramelessWidgetData(FramelessHelper::PrivateData* pd, QWidget* pTopLevelWidget);
    ~PrivateFramelessWidgetData();
    QWidget* widget();

    // 处理鼠标事件-划过、按下、释放、移动
    bool handleWidgetEvent(QEvent* event);

    // 更新橡皮筋状态
    void updateRubberBandStatus();

private:
    // 更新鼠标样式
    void updateCursorShape(const QPoint& gMousePos);

    // 重置窗体大小
    void resizeWidget(const QPoint& gMousePos);

    // 移动窗体
    void moveWidget(const QPoint& gMousePos);

    // 处理鼠标按下
    bool handleMousePressEvent(QMouseEvent* event);

    // 处理鼠标释放
    bool handleMouseReleaseEvent(QMouseEvent* event);

    // 处理鼠标移动
    bool handleMouseMoveEvent(QMouseEvent* event);

    // 处理鼠标离开
    bool handleLeaveEvent(QEvent* event);

    // 处理鼠标进入
    bool handleHoverMoveEvent(QHoverEvent* event);

    // 处理鼠标双击事件
    bool handleDoubleClickedMouseEvent(QMouseEvent* event);

private:
    FramelessHelper::PrivateData* d;
    QRubberBand* m_pRubberBand;
    QWidget* m_pWidget;
    QPoint m_ptDragPos;
    SAPrivateFramelessCursorPosCalculator m_pressedMousePos;
    SAPrivateFramelessCursorPosCalculator m_moveMousePos;
    bool m_bLeftButtonPressed;
    bool m_bCursorShapeChanged;
    bool m_bLeftButtonTitlePressed;
    Qt::WindowFlags m_windowFlags;
};

/***** WidgetData *****/
PrivateFramelessWidgetData::PrivateFramelessWidgetData(FramelessHelper::PrivateData* pd, QWidget* pTopLevelWidget)
{
    d = pd;
    m_pWidget = pTopLevelWidget;
    m_bLeftButtonPressed = false;
    m_bCursorShapeChanged = false;
    m_bLeftButtonTitlePressed = false;
    m_pRubberBand = NULL;

    m_windowFlags = m_pWidget->windowFlags();
    m_pWidget->setMouseTracking(true);
    m_pWidget->setAttribute(Qt::WA_Hover, true);

    updateRubberBandStatus();
}

PrivateFramelessWidgetData::~PrivateFramelessWidgetData()
{
    m_pWidget->setMouseTracking(false);
    m_pWidget->setWindowFlags(m_windowFlags);
    m_pWidget->setAttribute(Qt::WA_Hover, false);

    delete m_pRubberBand;
    m_pRubberBand = NULL;
}

QWidget* PrivateFramelessWidgetData::widget()
{
    return (m_pWidget);
}

bool PrivateFramelessWidgetData::handleWidgetEvent(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::MouseButtonPress:
        return (handleMousePressEvent(static_cast<QMouseEvent*>(event)));

    case QEvent::MouseButtonRelease:
        return (handleMouseReleaseEvent(static_cast<QMouseEvent*>(event)));

    case QEvent::MouseMove:
        return (handleMouseMoveEvent(static_cast<QMouseEvent*>(event)));

    case QEvent::Leave:
        return (handleLeaveEvent(static_cast<QMouseEvent*>(event)));

    case QEvent::HoverMove:
        return (handleHoverMoveEvent(static_cast<QHoverEvent*>(event)));

    case QEvent::MouseButtonDblClick:
        return (handleDoubleClickedMouseEvent(static_cast<QMouseEvent*>(event)));

    default:
        break;
    }
    return (false);
}

void PrivateFramelessWidgetData::updateRubberBandStatus()
{
    if (d->m_bRubberBandOnMove || d->m_bRubberBandOnResize)
    {
        if (NULL == m_pRubberBand)
        {
            m_pRubberBand = new QRubberBand(QRubberBand::Rectangle);
        }
    }
    else
    {
        delete m_pRubberBand;
        m_pRubberBand = NULL;
    }
}

void PrivateFramelessWidgetData::updateCursorShape(const QPoint& gMousePos)
{
    if (m_pWidget->isFullScreen() || m_pWidget->isMaximized())
    {
        if (m_bCursorShapeChanged)
        {
            m_pWidget->unsetCursor();
        }
        return;
    }

    QRect frameRect = m_pWidget->frameGeometry();
    if (!m_pWidget->isWindow())
    {
        frameRect = QRect(m_pWidget->parentWidget()->mapToGlobal(m_pWidget->frameGeometry().topLeft()), m_pWidget->frameGeometry().size());
    }

#ifdef _DEBUG
    // qDebug() << "m_moveMousePos:" << m_pWidget << m_pWidget->frameGeometry() << frameRect;
#endif

    m_moveMousePos.recalculate(gMousePos, frameRect);

    if (m_moveMousePos.m_bOnTopLeftEdge || m_moveMousePos.m_bOnBottomRightEdge)
    {
        m_pWidget->setCursor(Qt::SizeFDiagCursor);
        m_bCursorShapeChanged = true;
    }
    else if (m_moveMousePos.m_bOnTopRightEdge || m_moveMousePos.m_bOnBottomLeftEdge)
    {
        m_pWidget->setCursor(Qt::SizeBDiagCursor);
        m_bCursorShapeChanged = true;
    }
    else if (m_moveMousePos.m_bOnLeftEdge || m_moveMousePos.m_bOnRightEdge)
    {
        m_pWidget->setCursor(Qt::SizeHorCursor);
        m_bCursorShapeChanged = true;
    }
    else if (m_moveMousePos.m_bOnTopEdge || m_moveMousePos.m_bOnBottomEdge)
    {
        m_pWidget->setCursor(Qt::SizeVerCursor);
        m_bCursorShapeChanged = true;
    }
    else
    {
        if (m_bCursorShapeChanged)
        {
            m_pWidget->unsetCursor();
            m_bCursorShapeChanged = false;
        }
    }
}

void PrivateFramelessWidgetData::resizeWidget(const QPoint& gMousePos)
{
    QRect origRect;

    if (d->m_bRubberBandOnResize)
    {
        origRect = m_pRubberBand->frameGeometry();
    }
    else
    {
        origRect = m_pWidget->frameGeometry();
    }
    if (!m_pWidget->isWindow())
    {
        origRect = QRect(m_pWidget->parentWidget()->mapToGlobal(origRect.topLeft()), origRect.size());
    }

    int left = origRect.left();
    int top = origRect.top();
    int right = origRect.right();
    int bottom = origRect.bottom();

    origRect.getCoords(&left, &top, &right, &bottom);

    int minWidth = m_pWidget->minimumWidth();
    int minHeight = m_pWidget->minimumHeight();

    if (m_pressedMousePos.m_bOnTopLeftEdge)
    {
        left = gMousePos.x();
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomLeftEdge)
    {
        left = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnTopRightEdge)
    {
        right = gMousePos.x();
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomRightEdge)
    {
        right = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnLeftEdge)
    {
        left = gMousePos.x();
    }
    else if (m_pressedMousePos.m_bOnRightEdge)
    {
        right = gMousePos.x();
    }
    else if (m_pressedMousePos.m_bOnTopEdge)
    {
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomEdge)
    {
        bottom = gMousePos.y();
    }

    QRect newRect(QPoint(left, top), QPoint(right, bottom));
    if (!m_pWidget->isWindow())
    {
        newRect = QRect(m_pWidget->parentWidget()->mapFromGlobal(QPoint(left, top)), m_pWidget->parentWidget()->mapFromGlobal(QPoint(right, bottom)));
    }

    if (newRect.isValid())
    {
        if (minWidth > newRect.width())
        {
            if (left != origRect.left())
            {
                newRect.setLeft(origRect.left());
            }
            else
            {
                newRect.setRight(origRect.right());
            }
        }
        if (minHeight > newRect.height())
        {
            if (top != origRect.top())
            {
                newRect.setTop(origRect.top());
            }
            else
            {
                newRect.setBottom(origRect.bottom());
            }
        }

        // if(!m_pWidget->isWindow())
        // {
        //     newRect = QRect(QPoint(left, top), QPoint(right, bottom));
        // }

        if (d->m_bRubberBandOnResize)
        {
            m_pRubberBand->setGeometry(newRect);
        }
        else
        {
            m_pWidget->setGeometry(newRect);
        }
    }
}

void PrivateFramelessWidgetData::moveWidget(const QPoint& gMousePos)
{
    if (d->m_bRubberBandOnMove)
    {
        m_pRubberBand->move(gMousePos - m_ptDragPos);
    }
    else
    {
        m_pWidget->move(gMousePos - m_ptDragPos);
    }
}

bool PrivateFramelessWidgetData::handleMousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = true;
        m_bLeftButtonTitlePressed = event->pos().y() < m_moveMousePos.m_nTitleHeight;

        QRect frameRect = m_pWidget->frameGeometry();
        m_pressedMousePos.recalculate(event->globalPos(), frameRect);

        m_ptDragPos = event->globalPos() - frameRect.topLeft();
        if (!m_pWidget->isWindow())
        {
            QRect frameRectGlobal(m_pWidget->parentWidget()->mapToGlobal(m_pWidget->frameGeometry().topLeft()), m_pWidget->frameGeometry().size());
            QRect frameRect = m_pWidget->frameGeometry();
            m_pressedMousePos.recalculate(event->globalPos(), frameRectGlobal);

            // m_ptDragPos = event->globalPosition().toPoint() - frameRectGlobal.topLeft();
        }

        if (m_pressedMousePos.m_bOnEdges)
        {
            if (m_pWidget->isMaximized())
            {
                // 窗口在最大化状态时，点击边界不做任何处理
                return (false);
            }
            if (d->m_bRubberBandOnResize)
            {
                m_pRubberBand->setGeometry(frameRect);
                m_pRubberBand->show();
                return (true);
            }
        }
        else if (d->m_bRubberBandOnMove && m_bLeftButtonTitlePressed)
        {
            if (m_pWidget->isMaximized())
            {
                // 窗口在最大化状态时，点击标题栏不做任何处理
                return (false);
            }
            m_pRubberBand->setGeometry(frameRect);
            m_pRubberBand->show();
            return (true);
        }
    }
    return (false);
}

bool PrivateFramelessWidgetData::handleMouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = false;
        m_bLeftButtonTitlePressed = false;
        m_pressedMousePos.reset();
        if (m_pRubberBand && m_pRubberBand->isVisible())
        {
            m_pRubberBand->hide();
            m_pWidget->setGeometry(m_pRubberBand->geometry());
            return (true);
        }
    }
    return (false);
}

bool PrivateFramelessWidgetData::handleMouseMoveEvent(QMouseEvent* event)
{
    if (m_bLeftButtonPressed)
    {
        if (d->m_bWidgetResizable && m_pressedMousePos.m_bOnEdges)
        {
            if (m_pWidget->isMaximized())
            {
                // 窗口在最大化状态时，点击边界不做任何处理
                return (false);
            }
            resizeWidget(event->globalPos());
            return (true);
        }
        else if (d->m_bWidgetMovable && m_bLeftButtonTitlePressed)
        {
            if (m_pWidget->isMaximized())
            {
                // 先求出窗口到鼠标的相对位置
                QRect normalGeometry = m_pWidget->normalGeometry();
                m_pWidget->showNormal();
                QPoint p = event->globalPos();
                // if(!m_pWidget->isWindow())
                // {
                //     p = m_pWidget->parentWidget()->mapFromGlobal(p);
                //     p -= m_pWidget->window()->geometry().topLeft();
                // }
                p.ry() -= 10;
                p.rx() -= (normalGeometry.width() / 2);
                m_pWidget->move(p);
                // qDebug() << "p" << p;

                // 这时要重置m_ptDragPos
                m_ptDragPos = QPoint(normalGeometry.width() / 2, 10);
                return (true);
            }
            moveWidget(event->globalPos());
            return (true);
        }
        return (false);
    }
    else if (d->m_bWidgetResizable)
    {
        updateCursorShape(event->globalPos());
    }
    return (false);
}

bool PrivateFramelessWidgetData::handleLeaveEvent(QEvent* event)
{
    Q_UNUSED(event)
    if (!m_bLeftButtonPressed)
    {
        m_pWidget->unsetCursor();
        return (true);
    }
    return (false);
}

bool PrivateFramelessWidgetData::handleHoverMoveEvent(QHoverEvent* event)
{
    if (d->m_bWidgetResizable)
    {
        updateCursorShape(m_pWidget->mapToGlobal(event->pos()));
    }
    return (false);
}

bool PrivateFramelessWidgetData::handleDoubleClickedMouseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (m_pWidget)
        {
            QWidget* mainwindow = m_pWidget->window();
            if (mainwindow)
            {
                // 在最大化按钮显示时才进行shownormal处理
                bool titlePressed = event->pos().y() < m_moveMousePos.m_nTitleHeight;
                if (titlePressed && d->m_bIsMaximum)
                {
                    if (m_pWidget->isMaximized())
                    {
                        m_pWidget->showNormal();
                    }
                    else
                    {
                        m_pWidget->showMaximized();
                    }
                    return (true);
                }
            }
        }
    }
    return (false);
}

//===================================================
// FramelessHelper
//===================================================
FramelessHelper::FramelessHelper(QObject* parent) : QObject(parent), d_ptr(new FramelessHelper::PrivateData(this))
{
    d_ptr->m_bWidgetMovable = true;
    d_ptr->m_bWidgetResizable = true;
    d_ptr->m_bRubberBandOnResize = false;
    d_ptr->m_bRubberBandOnMove = false;
    if (parent)
    {
        QWidget* w = qobject_cast<QWidget*>(parent);
        if (w)
        {
            w->setWindowFlags(w->windowFlags() | Qt::FramelessWindowHint);
            setWidgetMovable(true);       // 设置窗体可移动
            setWidgetResizable(true);     // 设置窗体可缩放
            setRubberBandOnMove(false);   // 设置橡皮筋效果-可移动
            setRubberBandOnResize(true);  // 设置橡皮筋效果-可缩放
            activateOn(w);                // 激活当前窗体
        }
    }
}

FramelessHelper::~FramelessHelper()
{
    QList<QWidget*> keys = d_ptr->m_widgetDataHash.keys();
    int size = keys.size();

    for (int i = 0; i < size; ++i)
    {
        delete d_ptr->m_widgetDataHash.take(keys[i]);
    }
}

bool FramelessHelper::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::Leave: {
        PrivateFramelessWidgetData* data = d_ptr->m_widgetDataHash.value(static_cast<QWidget*>(obj));
        if (data)
        {
            return (data->handleWidgetEvent(event));
        }
        break;
    }

    default:
        break;
    }
    return (QObject::eventFilter(obj, event));
}

void FramelessHelper::activateOn(QWidget* topLevelWidget)
{
    if (!d_ptr->m_widgetDataHash.contains(topLevelWidget))
    {
        PrivateFramelessWidgetData* data = new PrivateFramelessWidgetData(d_ptr.get(), topLevelWidget);
        d_ptr->m_widgetDataHash.insert(topLevelWidget, data);

        topLevelWidget->installEventFilter(this);
    }
}

void FramelessHelper::removeFrom(QWidget* topLevelWidget)
{
    PrivateFramelessWidgetData* data = d_ptr->m_widgetDataHash.take(topLevelWidget);

    if (data)
    {
        topLevelWidget->removeEventFilter(this);
        delete data;
    }
}

void FramelessHelper::setRubberBandOnMove(bool movable)
{
    d_ptr->m_bRubberBandOnMove = movable;
    QList<PrivateFramelessWidgetData*> list = d_ptr->m_widgetDataHash.values();

    foreach (PrivateFramelessWidgetData* data, list)
    {
        data->updateRubberBandStatus();
    }
}

void FramelessHelper::setWidgetMovable(bool movable)
{
    d_ptr->m_bWidgetMovable = movable;
}

void FramelessHelper::setWidgetResizable(bool resizable)
{
    d_ptr->m_bWidgetResizable = resizable;
}

void FramelessHelper::setRubberBandOnResize(bool resizable)
{
    d_ptr->m_bRubberBandOnResize = resizable;
    QList<PrivateFramelessWidgetData*> list = d_ptr->m_widgetDataHash.values();

    foreach (PrivateFramelessWidgetData* data, list)
    {
        data->updateRubberBandStatus();
    }
}

void FramelessHelper::setMaximumEnable(bool maximumable)
{
    d_ptr->m_bIsMaximum = maximumable;
}

void FramelessHelper::setBorderWidth(int width)
{
    if (width > 0)
    {
        SAPrivateFramelessCursorPosCalculator::m_nBorderWidth = width;
    }
}

void FramelessHelper::setTitleHeight(int height)
{
    if (height > 0)
    {
        SAPrivateFramelessCursorPosCalculator::m_nTitleHeight = height;
    }
}

bool FramelessHelper::widgetMovable()
{
    return (d_ptr->m_bWidgetMovable);
}

bool FramelessHelper::widgetResizable()
{
    return (d_ptr->m_bWidgetResizable);
}

bool FramelessHelper::rubberBandOnMove()
{
    return (d_ptr->m_bRubberBandOnMove);
}

bool FramelessHelper::rubberBandOnResisze()
{
    return (d_ptr->m_bRubberBandOnResize);
}

uint FramelessHelper::borderWidth()
{
    return (SAPrivateFramelessCursorPosCalculator::m_nBorderWidth);
}

uint FramelessHelper::titleHeight()
{
    return (SAPrivateFramelessCursorPosCalculator::m_nTitleHeight);
}

TitleWidget::TitleWidget(QWidget* parent)
    : QWidget(parent), m_pLabelIcon(new QLabel(this)), m_pLabelTitle(new QLabel(this)), m_pBtnMin(new QPushButton(this)), m_pBtnMax(new QPushButton(this)),
      m_pBtnClose(new QPushButton(this)), m_pFramelessHelper(new FramelessHelper(topLevelWidget()))
{
    setAttribute(Qt::WA_StyledBackground);

    m_pBtnMax->setToolTip(tr("Maximize"));
    m_pBtnMax->setIcon(style()->standardPixmap(QStyle::SP_TitleBarMaxButton));
    m_pBtnMin->setToolTip("Minimize");
    m_pBtnMin->setIcon(style()->standardPixmap(QStyle::SP_TitleBarMinButton));
    m_pBtnClose->setToolTip("Close");
    m_pBtnClose->setIcon(style()->standardPixmap(QStyle::SP_TitleBarCloseButton));

    m_pFramelessHelper->setMaximumEnable(true);
    m_pFramelessHelper->setRubberBandOnResize(false);

    m_pLabelIcon->move(4, 4);
    m_pLabelTitle->setScaledContents(true);
    m_pLabelTitle->setAlignment(Qt::AlignVCenter);

    connect(m_pBtnMin, &QPushButton::clicked, parent, [this]() {
        if (auto pSubWindow = qobject_cast<QMdiSubWindow*>(parentWidget()->parentWidget()))
        {
            pSubWindow->showMinimized();
        }
        else
        {
            window()->showMinimized();
        }
    });

    connect(m_pBtnMax, &QPushButton::clicked, parent, [this]() {
        if (auto pSubWindow = qobject_cast<QMdiSubWindow*>(parentWidget()->parentWidget()))
        {
            pSubWindow->isMaximized() ? pSubWindow->showNormal() : pSubWindow->showMaximized();
        }
        else
        {
            window()->isMaximized() ? window()->showNormal() : window()->showMaximized();
        }
    });

    connect(m_pBtnClose, &QPushButton::clicked, parent, [this]() {
        if (auto pSubWindow = qobject_cast<QMdiSubWindow*>(parentWidget()->parentWidget()))
        {
            pSubWindow->close();
        }
        else
        {
            window()->close();
        }
    });
}

bool TitleWidget::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::WindowTitleChange: {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget)
        {
            setWindowTitle(widget->windowTitle());
            m_pLabelTitle->setText(widget->windowTitle());
            return true;
        }
    }
    case QEvent::WindowIconChange: {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget)
        {
            m_pLabelIcon->setPixmap(widget->windowIcon().pixmap(m_pLabelIcon->size()));
            return true;
        }
    }
    default:
        break;
    }

    return QWidget::eventFilter(obj, event);
}

void TitleWidget::resizeEvent(QResizeEvent* event)
{
    m_pLabelIcon->resize(height() - 4, height() - 4);
    m_pLabelTitle->resize(m_pLabelTitle->width(), height());
    m_pLabelTitle->move(4 + height(), 0);
    m_pBtnMin->setGeometry(QRect(width() - height() * 3, 0, height(), height()));
    m_pBtnMax->setGeometry(QRect(width() - height() * 2, 0, height(), height()));
    m_pBtnClose->setGeometry(QRect(width() - height() * 1, 0, height(), height()));
    m_pFramelessHelper->setTitleHeight(height());
    return QWidget::resizeEvent(event);
}
