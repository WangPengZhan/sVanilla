#include <QApplication>

#include "PlatformInitializer.h"
#include "version.h"
#include "Config/GlobalData.h"

void PlatformInitializer::allApplicationBefore()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
    qputenv("QT_DPI_ADJUSTMENT_POLICY", "AdjustDpi");
#endif  // Qt < 6.2.0

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
#endif  // Qt < 6.0.0

    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication::setApplicationName(constant::softwareName);
    QApplication::setApplicationVersion(SVNLA_VERSION_STR);
    QApplication::setOrganizationDomain(constant::company);
    QApplication::setOrganizationName(constant::homePage);
}

void PlatformInitializer::allInit()
{
}

void PlatformInitializer::allDeinit()
{
}
