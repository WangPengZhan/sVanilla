#include "FreezeModel.h"

FreezeModel::FreezeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    initData();
}

FreezeModel::~FreezeModel()
{
}

QModelIndex FreezeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid() && row < rowCount() && column < columnCount())
    {
        return createIndex(row, column, &m_datas[row][column]);
    }

    return QModelIndex();
}

QModelIndex FreezeModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}

int FreezeModel::rowCount(const QModelIndex& parent) const
{
    return m_datas.size();
}

int FreezeModel::columnCount(const QModelIndex& parent) const
{
    return m_headers.size();
}

QVariant FreezeModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() && index.row() < rowCount() && index.column() < columnCount() && role == Qt::DisplayRole)
    {
        return m_datas[index.row()][index.column()];
    }
    return QVariant();
}

QVariant FreezeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < m_headers.size() && orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return m_headers.at(section);
    }
    return QVariant();
}

void FreezeModel::initData()
{
    for (int i = 0; i < 20; ++i)
    {
        m_headers.append(QString("header %1").arg(i));
    }

    for (int j = 0; j < 1000; ++j)
    {
        QStringList rowDatas;
        for (int i = 0; i < m_headers.size(); ++i)
        {
            rowDatas.append(QString("Data %1").arg(i + j * m_headers.size()));
        }
        m_datas.append(rowDatas);
    }
}
