#pragma once

#include <QLineEdit>
#include <QPropertyAnimation>

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(QWidget* parent = nullptr);
    ~SearchLineEdit();

    void setFocusOutHide();

signals:
    void readyHide();
    void startHide();

protected:
    void focusOutEvent(QFocusEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();
    void startHideAnimation();

private:
    bool focusOutHide = false;
    QAction* m_clearAction{nullptr};
    QAction* m_searchAction{nullptr};
};
