#pragma once
#include <QListWidget>

class NameRuleSelectWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit NameRuleSelectWidget(QWidget* parent = nullptr);

    void setNameRules(const std::vector<QString>& rules);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QMimeData* mimeData(const QList<QListWidgetItem*>& items) const override;
#else
    QMimeData* mimeData(const QList<QListWidgetItem*> items) const override;
#endif

private:
    void setUi();

private:
    QPoint m_dragPoint;
    QListWidgetItem* m_dragItem = nullptr;
};