#pragma once
#include <QAbstractItemModel>

class FreezeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    FreezeModel(QObject* parent = nullptr);
    ~FreezeModel();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    void initData();

private:
    QStringList m_headers;
    QList<QStringList> m_datas;
};
