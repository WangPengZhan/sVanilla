#pragma once
#include <QListWidget>

class NameRule : public QWidget
{
    Q_OBJECT

public:
    explicit NameRule(QWidget* parent = nullptr);

    void setRule(const QString& rule);

    [[nodiscard]] QString rule() const
    {
        return m_rule;
    }

    [[nodiscard]] QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_rule;
    int m_width{};
    int m_height{};
    int m_padding{5};
    int m_margin{};
};

class NameRuleSelectWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit NameRuleSelectWidget(QWidget* parent = nullptr);

    void setNameRules(const std::vector<QString>& rules);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    void setUi();
    void makeItem(const QString& name);
    void removeItem(const QListWidgetItem* item);

private:
    QPoint m_dragPoint;
    QListWidgetItem* m_dragItem = nullptr;
};