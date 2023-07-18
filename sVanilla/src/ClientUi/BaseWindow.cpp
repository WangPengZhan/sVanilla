#include "BaseWindow.h"
#include <FramelessHelper/Widgets/framelesswidgetshelper.h>
#include <FramelessHelper/Widgets/standardtitlebar.h>
#include <QHBoxLayout>
#include <QLabel>
// extern template void Settings::set<QRect>(const QString &, const QString &,
// const QRect &);
//extern template void Settings::set<qreal>(const QString &, const QString &, const qreal &);
//
//extern template QRect Settings::get<QRect>(const QString &, const QString &);
//extern template qreal Settings::get<qreal>(const QString &, const QString &);

FRAMELESSHELPER_USE_NAMESPACE
//FRAMELESSHELPER_STRING_CONSTANT(Geometry)
//FRAMELESSHELPER_STRING_CONSTANT(DevicePixelRatio)


void BaseWindow::waitReady()
{
    FramelessWidgetsHelper *helper = FramelessWidgetsHelper::get(this);
    helper->waitForReady();
//    const QString objName = objectName();
//    const auto savedGeometry = Settings::get<QRect>(objName, kGeometry);
//    if (savedGeometry.isValid() && !parent()) {
//        const auto savedDpr = Settings::get<qreal>(objName, kDevicePixelRatio);
//        // Qt doesn't support dpr < 1.
//        const qreal oldDpr = std::max(savedDpr, qreal(1));
//        const qreal scale = (devicePixelRatioF() / oldDpr);
//        setGeometry({savedGeometry.topLeft() * scale, savedGeometry.size() * scale});
//    } else {
//        helper->moveWindowToDesktopCenter();
//    }
}
BaseWindow::BaseWindow(QWidget* parent)
    : FramelessWidget(parent)
{
//    setWindowTitle(tr("Svanilla"));
    initialize();
}
void BaseWindow::closeEvent(QCloseEvent* event)
{
//    if (!parent()) {
//        const QString objName = objectName();
//        Settings::set(objName, kGeometry, geometry());
//        Settings::set(objName, kDevicePixelRatio, devicePixelRatioF());
//    }
    FramelessWidget::closeEvent(event);
}
void BaseWindow::initialize()
{
    resize(800, 600);

    m_titleBar = new StandardTitleBar(this);
    const auto mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_titleBar);
    const auto contentLayout = new QHBoxLayout;
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->addStretch();
    mainLayout->addLayout(contentLayout);
    auto label = new QLabel(this);
    label->setText("Hello World");
    contentLayout->addWidget(label);

    FramelessWidgetsHelper *helper = FramelessWidgetsHelper::get(this);
    helper->extendsContentIntoTitleBar();
    helper->setTitleBarWidget(m_titleBar);
}
