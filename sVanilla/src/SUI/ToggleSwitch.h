#pragma once
#include <QPropertyAnimation>
#include <QWidget>

class ToggleSwitch : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY toggled)
    Q_PROPERTY(int switchPosition READ switchPosition WRITE setSwitchPosition)
public:
    explicit ToggleSwitch(QWidget* parent = nullptr);

    void setChecked(bool checked);
    [[nodiscard]] bool isChecked() const;
    void toggle();
    int switchPosition() const;
    void setSwitchPosition(int pos);


    [[nodiscard]] QSize sizeHint() const override;

signals:
    void toggled(bool checked);
    void checked(bool checked);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    bool m_checked = false;
    bool m_mouseDown = false;
    int m_switchPosition = 0;
    QPropertyAnimation* m_animation;

};
