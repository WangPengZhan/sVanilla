#pragma once
#include <QTableView>
#include <QHeaderView>

class FreezeTreeView : public QTableView
{
    Q_OBJECT
public:
    FreezeTreeView(QWidget* parent = nullptr);
    ~FreezeTreeView();

    virtual void setModel(QAbstractItemModel* model) override;
    void setItemDelegate(QAbstractItemDelegate* delegate);

    void setFrozenHeader(QHeaderView* header);
    QTableView* getFrozenTableView();
    void setFrozenColumnCount(int count);
    int getFrozenColumnCount() const;

    void init();
    void setFrozenSectionResizeMode(int logicalIndex, QHeaderView::ResizeMode mode);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void scrollTo(const QModelIndex& index, ScrollHint hint = EnsureVisible) override;
    virtual bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void signalsAndSlots();

    void updateSectionWidth(int logicalIndex, int oldSize, int newSize);
    void updateselfGeometry(int logicalIndex, int oldSize, int newSize);

    void resetFrozenTableView();
    void updateFrozenTableGeometry();

private:
    int m_frozenColCount{};
    QTableView* m_frozenTableView{};
};
