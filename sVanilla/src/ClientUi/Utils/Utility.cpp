#include <QPropertyAnimation>
#include <QProcess>
#include <QDir>
#include <QMenu>

#include "ClientUi/Utils/Utility.h"

namespace util
{

void showInFileExplorer(const QString& filePath)
{
    QStringList arguments;

#ifdef _WIN32
    QString explorerCommand = "explorer";
    arguments << "/select," << QDir::toNativeSeparators(filePath);
#elif __linux__
    QString explorerCommand = "nautilus";
    arguments << filePath;
#elif __APPLE__
    QString explorerCommand = "open";
    arguments << QStringLiteral("-R") << filePath;
#endif

    QProcess::startDetached(explorerCommand, arguments);
}

void createMenu(QMenu* menu, int width, const std::vector<std::string>& history, const std::function<void(const QString&)>& actionCallback)
{
    for (const auto& uri : history)
    {
        const auto text = QString::fromStdString(uri);
        QString elidedText = text;
        if (const QFontMetrics fontMetrics(menu->font()); fontMetrics.horizontalAdvance(text) > width)
        {
            elidedText = fontMetrics.elidedText(text, Qt::ElideRight, width);
        }

        auto* const action = new QAction(elidedText, menu);
        menu->addAction(action);
        QObject::connect(action, &QAction::triggered, menu, [actionCallback, text]() {
            actionCallback(text);
        });
        action->setToolTip(text);
    }
}

void animate(QObject* obj, AnimationStartEnd change, const QByteArray& propertyName, const std::function<void()>& callback, int duration)
{
    auto* animation = new QPropertyAnimation(obj, propertyName);
    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->setStartValue(change.start);
    animation->setEndValue(change.end);
    if (callback)
    {
        QObject::connect(animation, &QPropertyAnimation::finished, callback);
    }
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

}  // namespace util
