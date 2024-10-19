#include "TreeItem.h"

TreeItem::TreeItem(TreeItem* parent)
    : m_parent(parent)
{
    if (m_parent)
    {
        m_parent->m_children.append(this);
    }
}

TreeItem::~TreeItem()
{
    clearChild(true);
    setParent(nullptr);
}

const QList<TreeItem*>& TreeItem::children() const
{
    return m_children;
}

void TreeItem::setParent(TreeItem* parent)
{
    if (m_parent)
    {
        m_parent->m_children.removeAll(this);
    }

    m_parent = parent;

    if (m_parent)
    {
        m_parent->m_children.append(this);
    }
}

TreeItem* TreeItem::parent() const
{
    return m_parent;
}

void TreeItem::addChild(TreeItem* pChild)
{
    if (pChild)
    {
        pChild->setParent(this);
    }
}

void TreeItem::clearChild(bool isDelete)
{
    if (isDelete)
    {
        for (auto& child : m_children)
        {
            auto removed = child;
            child = nullptr;
            delete removed;
        }
    }

    m_children.clear();
}

void TreeItem::addChildren(QList<TreeItem*> listChildren)
{
    for (const auto& pChild : listChildren)
    {
        addChild(pChild);
    }
}

TreeItem* TreeItem::clone() const
{
    auto pCloned = new TreeItem(*this);
    pCloned->m_parent = nullptr;
    pCloned->m_children.clear();
    for (const auto& pChild : m_children)
    {
        auto pClonedChild = pChild->clone();
        pClonedChild->setParent(pCloned);
    }
    return pCloned;
}

QString TreeItem::text(int col)
{
    return {};
}
