#pragma once

#include <QLineEdit>

class NameRuleEditWidget : public QLineEdit
{
    Q_OBJECT

public:
    explicit NameRuleEditWidget(QWidget* parent = nullptr);

protected:
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void appendRule(const QString& rule);
};
