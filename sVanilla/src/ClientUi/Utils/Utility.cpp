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

void createMenu(QMenu* menu, int width, const std::list<std::string>& history, const std::function<void(const QString&)>& actionCallback)
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

void moveAnimate(QObject* obj, MoveStartEndValue posChange, const std::function<void()>& finishedCallback)
{
    auto* animation = new QPropertyAnimation(obj, "pos");
    static constexpr int duration = 300;
    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->setStartValue(posChange.startValue);
    animation->setEndValue(posChange.endValue);
    if (finishedCallback)
    {
        QObject::connect(animation, &QPropertyAnimation::finished, finishedCallback);
    }
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void moveAnimate(QObject* obj, LayoutMoveStartEndValue posChange, const std::function<void()>& finishedCallback)
{
    auto* animation = new QPropertyAnimation(obj, "maximumWidth");
    static constexpr int duration = 300;
    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->setStartValue(posChange.startValue);
    animation->setEndValue(posChange.endValue);
    if (finishedCallback)
    {
        QObject::connect(animation, &QPropertyAnimation::finished, finishedCallback);
    }
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void animate(QObject* obj, AnimationStartEnd change, const QByteArray& propertyName, const std::function<void()>& callback)
{
    auto* animation = new QPropertyAnimation(obj, propertyName);
    static constexpr int duration = 300;
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
