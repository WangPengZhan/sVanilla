#pragma once
#include <QLineEdit>

class AddLinkLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit AddLinkLineEdit(QWidget* parent = nullptr);
    ~AddLinkLineEdit();

    void SetEditFinishedSearch(bool enabled);
    void setWebsiteIcon(const QString& iconPath) const;

protected:
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

signals:
    void Complete();

private:
    void setUi();
    void signalsAndSlots();
    void btnMoreClick(bool checked);
    void createMoreMenu();
    void setMoreBtnIcon(bool checked);

private:
    QMenu* m_moreMenu;
    QAction* m_webSiteAction{nullptr};
    QAction* m_clearAction{nullptr};
    QAction* m_enterAction{nullptr};
    QAction* m_moreAction{nullptr};
};
