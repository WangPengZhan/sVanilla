#pragma once

#include <QLineEdit>
#include <QMap>

class NameRuleEditWidget : public QLineEdit
{
    Q_OBJECT
public:
    explicit NameRuleEditWidget(QWidget* parent = nullptr);

    void setRules(const QMap<QString, QString>& rules);
    const QMap<QString, QString>& rules() const;

protected:
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void appendRule(const QString& rule);

private:
    QMap<QString, QString> m_rules;
};
