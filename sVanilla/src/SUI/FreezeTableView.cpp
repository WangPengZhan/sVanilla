#include "FreezeTableView.h"

#include <QEvent>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QScrollBar>

FreezeTreeView::FreezeTreeView(QWidget* parent)
    : QTableView(parent)
    , m_frozenTableView(new QTableView(this))
{
    setUi();
    signalsAndSlots();
}

FreezeTreeView::~FreezeTreeView()
{
}

void FreezeTreeView::setModel(QAbstractItemModel* model)
{
    QTableView::setModel(model);
    m_frozenTableView->setModel(model);
    m_frozenTableView->setSelectionMode(selectionMode());
}

void FreezeTreeView::setItemDelegate(QAbstractItemDelegate* delegate)
{
    QTableView::setItemDelegate(delegate);
    m_frozenTableView->setItemDelegate(delegate);
}

void FreezeTreeView::setUi()
{
    horizontalHeader()->installEventFilter(this);
    viewport()->stackUnder(m_frozenTableView);

    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_frozenTableView->setFocusPolicy(Qt::NoFocus);
    m_frozenTableView->setFrameStyle(NoFrame);
    m_frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_frozenTableView->setHorizontalScrollMode(horizontalScrollMode());
    m_frozenTableView->setVerticalScrollMode(verticalScrollMode());
    m_frozenTableView->setSelectionMode(selectionMode());
    m_frozenTableView->setShowGrid(showGrid());
    m_frozenTableView->setSelectionBehavior(selectionBehavior());
    m_frozenTableView->verticalHeader()->setVisible(verticalHeader()->isVisible());
    m_frozenTableView->horizontalHeader()->setMinimumSectionSize(horizontalHeader()->minimumSectionSize());
    m_frozenTableView->verticalHeader()->setMinimumSectionSize(verticalHeader()->minimumSectionSize());
    m_frozenTableView->verticalHeader()->setDefaultSectionSize(verticalHeader()->defaultSectionSize());
    resetFrozenTableView();
    m_frozenTableView->show();
    updateFrozenTableGeometry();
}

void FreezeTreeView::signalsAndSlots()
{
    connect(horizontalHeader(), &QHeaderView::sectionResized, this, &FreezeTreeView::updateSectionWidth);
    connect(m_frozenTableView->horizontalHeader(), &QHeaderView::sectionResized, this, &FreezeTreeView::updateselfGeometry);
    connect(verticalScrollBar(), &QAbstractSlider::valueChanged, m_frozenTableView->verticalScrollBar(), &QAbstractSlider::setValue);
    connect(m_frozenTableView->verticalScrollBar(), &QAbstractSlider::valueChanged, verticalScrollBar(), &QAbstractSlider::setValue);
    connect(m_frozenTableView, &QTableView::clicked, [this](const QModelIndex& index) {
        emit clicked(index);
    });
}

void FreezeTreeView::updateSectionWidth(int logicalIndex, int oldSize, int newSize)
{
    if (logicalIndex < m_frozenColCount)
    {
        m_frozenTableView->setColumnWidth(logicalIndex, newSize);
        updateFrozenTableGeometry();
    }
}

void FreezeTreeView::updateselfGeometry(int logicalIndex, int oldSize, int newSize)
{
    updateFrozenTableGeometry();

    if (logicalIndex < m_frozenColCount)
    {
        setColumnWidth(logicalIndex, newSize);
    }
}

void FreezeTreeView::resizeEvent(QResizeEvent* event)
{
    QTableView::resizeEvent(event);
    updateFrozenTableGeometry();
}

void FreezeTreeView::resetFrozenTableView()
{
    if (model())
    {
        for (int col = m_frozenColCount; col < model()->columnCount(); ++col)
        {
            m_frozenTableView->setColumnHidden(col, true);
        }

        for (int col = 0; col < m_frozenColCount; ++col)
        {
            m_frozenTableView->setColumnHidden(col, false);
            auto colWidth = columnWidth(col);
            m_frozenTableView->setColumnWidth(col, colWidth);
        }
    }
}

void FreezeTreeView::updateFrozenTableGeometry()
{
    int freezewidth = 0;
    for (int i = 0; i < m_frozenColCount; i++)
    {
        freezewidth += columnWidth(i);
    }

    m_frozenTableView->setGeometry(1, 1, freezewidth, height());
}

void FreezeTreeView::setFrozenColumnCount(int count)
{
    m_frozenColCount = count;
    resetFrozenTableView();
    updateFrozenTableGeometry();
}

int FreezeTreeView::frozenColumnCount() const
{
    return m_frozenColCount;
}

QTableView* FreezeTreeView::frozenTableView()
{
    return m_frozenTableView;
}

void FreezeTreeView::scrollTo(const QModelIndex& index, ScrollHint hint)
{
    if (index.column() >= m_frozenColCount)
    {
        QTableView::scrollTo(index, hint);
    }
}

bool FreezeTreeView::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::Resize)
    {
        updateFrozenTableGeometry();
    }
    return QTableView::eventFilter(obj, event);
}

void FreezeTreeView::setFrozenSectionResizeMode(int logicalIndex, QHeaderView::ResizeMode mode)
{
    horizontalHeader()->setSectionResizeMode(logicalIndex, mode);
    m_frozenTableView->horizontalHeader()->setSectionResizeMode(logicalIndex, mode);
}
