#pragma once
#include <QList>

struct TreeItem
{
    TreeItem(TreeItem* parent = nullptr);
    TreeItem(TreeItem&& other) noexcept = default;
    TreeItem& operator=(TreeItem&& other) noexcept = default;
    virtual ~TreeItem();

    const QList<TreeItem*>& children() const;
    void setParent(TreeItem* parent);
    TreeItem* parent() const;
    void addChild(TreeItem* pChild);
    void addChildren(QList<TreeItem*> listChildren);

    virtual TreeItem* clone() const;

    virtual QString text(int col);

    void clearChild(bool isDelete);

protected:
    TreeItem(const TreeItem& other) = default;
    TreeItem& operator=(const TreeItem& other) = default;

private:
    TreeItem* m_parent = nullptr;
    QList<TreeItem*> m_children;  // for treewidget
};
