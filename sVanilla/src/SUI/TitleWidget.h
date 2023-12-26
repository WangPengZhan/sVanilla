#pragma once

#include <QWidget>
#include <QObject>

class QLabel;
class QPushButton;

/**
 * @def   模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 */
#ifndef QTLIKE_DECLARE_PRIVATE
#    define QTLIKE_DECLARE_PRIVATE(classname)                                                                                                                  \
        class PrivateData;                                                                                                                                     \
        friend class classname::PrivateData;                                                                                                                   \
        std::unique_ptr<PrivateData> d_ptr;
#endif
/**
 * @def   模仿Q_DECLARE_PUBLIC
 */
#ifndef QTLIKE_DECLARE_PUBLIC
#    define QTLIKE_DECLARE_PUBLIC(classname)                                                                                                                   \
        friend class classname;                                                                                                                                \
        classname* q_ptr{nullptr};
#endif

class FramelessHelper : public QObject
{
    Q_OBJECT
    QTLIKE_DECLARE_PRIVATE(FramelessHelper)
    friend class PrivateFramelessWidgetData;

public:
    explicit FramelessHelper(QObject* parent);
    ~FramelessHelper();

    // 激活窗体
    void activateOn(QWidget* topLevelWidget);

    // 移除窗体
    void removeFrom(QWidget* topLevelWidget);

    // 设置窗体移动
    void setWidgetMovable(bool movable);

    // 设置窗体缩放
    void setWidgetResizable(bool resizable);

    // 设置橡皮筋移动
    void setRubberBandOnMove(bool movable);

    // 设置橡皮筋缩放
    void setRubberBandOnResize(bool resizable);

    // 设置是否可以最大化
    void setMaximumEnable(bool maximumable);

    // 设置边框的宽度
    void setBorderWidth(int width);

    // 设置标题栏高度
    void setTitleHeight(int height);
    bool widgetResizable();
    bool widgetMovable();
    bool rubberBandOnMove();
    bool rubberBandOnResisze();
    uint borderWidth();
    uint titleHeight();

protected:
    // 事件过滤，进行移动、缩放等
    virtual bool eventFilter(QObject* obj, QEvent* event);
};

class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    TitleWidget(QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

public:
    QLabel* m_pLabelIcon;
    QLabel* m_pLabelTitle;
    QPushButton* m_pBtnMin;
    QPushButton* m_pBtnMax;
    QPushButton* m_pBtnClose;
    FramelessHelper* m_pFramelessHelper;
};
