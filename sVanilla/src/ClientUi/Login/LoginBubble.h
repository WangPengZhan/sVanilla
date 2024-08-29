#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class AbstractLogin;

class LoginBubble : public QWidget
{
    Q_OBJECT

public:
    explicit LoginBubble(std::shared_ptr<AbstractLogin> loginer, QWidget* parent = nullptr);

    void showCenter(const QRect& rect);

protected:
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    void setUi();
    void movePosition(const QRect& pos);

    static QPixmap binToImage(const std::vector<uint8_t>& bin, QSize size);

private:
    qreal m_proportion = 0.5;

    std::shared_ptr<AbstractLogin> m_loginer;

    QLabel* m_background{};
    QLabel* m_orc{};
    QLabel* m_toolTip{};
    QLabel* m_strTip{};
    QPushButton* m_btnRefresh{};
};
