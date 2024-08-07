#include "TreeViewIterator.h"

TreeViewIterator::TreeViewIterator(const TreeViewIterator& it)
    : m_model(it.m_model)
    , m_currentIndex(it.m_currentIndex)
{
}

TreeViewIterator::TreeViewIterator(QAbstractItemModel* model)
    : m_model(model)
{
    if (model)
    {
        m_currentIndex = model->index(0, 0);
    }
}

TreeViewIterator::TreeViewIterator(QTreeView* treeView)
    : TreeViewIterator(treeView ? treeView->model() : nullptr)
{
}

TreeViewIterator::~TreeViewIterator()
{
}

TreeViewIterator& TreeViewIterator::operator=(const TreeViewIterator& it)
{
    m_model = it.m_model;
    m_currentIndex = it.m_currentIndex;
    return *this;
}

TreeViewIterator& TreeViewIterator::operator++()
{
    m_currentIndex = getNextIndex(m_currentIndex);
    return *this;
}

const TreeViewIterator TreeViewIterator::operator++(int)
{
    TreeViewIterator it = *this;
    ++(*this);
    return it;
}

TreeViewIterator& TreeViewIterator::operator+=(int n)
{
    if (n < 0)
    {
        return (*this) -= (-n);
    }

    while (hasNext() && n--)
    {
        ++(*this);
    }

    return *this;
}

TreeViewIterator& TreeViewIterator::operator--()
{
    m_currentIndex = getPreviousIndex(m_currentIndex);
    return *this;
}

const TreeViewIterator TreeViewIterator::operator--(int)
{
    TreeViewIterator it = *this;
    --(*this);
    return it;
}

TreeViewIterator& TreeViewIterator::operator-=(int n)
{
    if (n < 0)
    {
        return (*this) += (-n);
    }

    while (hasNext() && n--)
    {
        --(*this);
    }

    return *this;
}

bool TreeViewIterator::hasNext() const
{
    return m_currentIndex.isValid();
}

QModelIndex TreeViewIterator::currentIndex() const
{
    return m_currentIndex;
}

QModelIndex TreeViewIterator::getNextIndex(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    QModelIndex next = m_model->index(0, 0, m_currentIndex);
    if (next.isValid())
    {
        return next;
    }

    QModelIndex sibling = index.siblingAtRow(index.row() + 1);
    if (sibling.isValid())
    {
        return sibling;
    }

    QModelIndex parent = index.parent();
    QModelIndex current = index;
    while (parent.isValid())
    {
        sibling = parent.siblingAtRow(parent.row() + 1);
        if (sibling.isValid())
        {
            return sibling;
        }
        current = parent;
        parent = parent.parent();
    }

    return QModelIndex();
}

QModelIndex TreeViewIterator::getPreviousIndex(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    QModelIndex previous = index.siblingAtRow(index.row() - 1);
    if (previous.isValid())
    {
        while (m_model->index(m_model->rowCount(previous) - 1, 0).isValid())
        {
            previous = m_model->index(m_model->rowCount(previous) - 1, 0);
        }
        return previous;
    }

    QModelIndex parent = index.parent();
    if (parent.isValid())
    {
        return parent;
    }

    return QModelIndex();
}
