#pragma once
#include <FramelessHelper/Widgets/framelesswidget.h>
#include <QWidget>

FRAMELESSHELPER_BEGIN_NAMESPACE
class StandardTitleBar;
FRAMELESSHELPER_END_NAMESPACE

class BaseWindow : public FRAMELESSHELPER_PREPEND_NAMESPACE(FramelessWidget) {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BaseWindow)
public:
    explicit BaseWindow(QWidget* parent = nullptr);
    void waitReady();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void initialize();
    FRAMELESSHELPER_PREPEND_NAMESPACE(StandardTitleBar) * m_titleBar = nullptr;
};
