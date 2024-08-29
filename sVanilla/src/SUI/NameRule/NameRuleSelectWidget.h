#pragma once
#include <QListWidget>

class NameRuleSelectWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit NameRuleSelectWidget(QWidget* parent = nullptr);

    void setNameRules(const std::vector<QString>& rules);

    [[nodiscard]] QMimeData* mimeData(const QList<QListWidgetItem*>& items) const override;

private:
    void setUi();

private:
    QPoint m_dragPoint;
    QListWidgetItem* m_dragItem = nullptr;
};