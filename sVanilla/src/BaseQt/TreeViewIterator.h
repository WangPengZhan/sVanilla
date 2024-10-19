#pragma once
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QTreeView>

class TreeViewIterator
{
public:
    TreeViewIterator(const TreeViewIterator& it);
    explicit TreeViewIterator(QAbstractItemModel* model);
    explicit TreeViewIterator(QTreeView* treeView);
    ~TreeViewIterator();

    bool hasNext() const;

    TreeViewIterator& operator=(const TreeViewIterator& it);

    TreeViewIterator& operator++();
    const TreeViewIterator operator++(int);
    TreeViewIterator& operator+=(int n);

    TreeViewIterator& operator--();
    const TreeViewIterator operator--(int);
    TreeViewIterator& operator-=(int n);

    template <typename T>
    T* operator*() const;

    template <typename T>
    T* data() const;

    QModelIndex currentIndex() const;

private:
    QModelIndex getNextIndex(const QModelIndex& index) const;
    QModelIndex getPreviousIndex(const QModelIndex& index) const;

private:
    const QAbstractItemModel* m_model;
    QModelIndex m_currentIndex;
};

template <typename T>
inline T* TreeViewIterator::operator*() const
{
    return reinterpret_cast<T*>(m_currentIndex.internalPointer());
}

template <typename T>
inline T* TreeViewIterator::data() const
{
    return reinterpret_cast<T*>(m_currentIndex.internalPointer());
}
