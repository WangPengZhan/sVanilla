#include <QPropertyAnimation>
#include <QProcess>
#include <QDir>
#include <QMenu>
#include <QBuffer>
#include <QImageReader>
#include <QMimeDatabase>
#include <QSvgRenderer>
#include <QPainter>
#include <QGuiapplication>
#include <QGuiapplication>
#include <QScreen>

#include "Utility.h"

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

QPixmap binToImage(const std::vector<uint8_t>& bin, QSize size)
{
    QBuffer buffer;
    buffer.setData(reinterpret_cast<const char*>(bin.data()), bin.size());
    buffer.open(QIODevice::ReadOnly);

    const QMimeDatabase db;  
    if (const QMimeType mime = db.mimeTypeForData(buffer.data()); mime.inherits("image/svg+xml"))
    {
        QSvgRenderer render;  
        render.load(buffer.data());  
        if (!render.isValid())  
        {  
            return {};  
        }  
        const QScreen* screen = QGuiApplication::primaryScreen();  
        const qreal ratio = screen->devicePixelRatio();  
        QPixmap pixmap(size * ratio);  
        pixmap.fill(Qt::transparent);  
        QPainter painter(&pixmap);  
        painter.setRenderHint(QPainter::Antialiasing, true);  
        render.render(&painter, pixmap.rect());  
        return pixmap;  
    }
    QImageReader render(&buffer);
    render.setQuality(100);
    QImage image = render.read();
    image = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return QPixmap::fromImage(std::move(image));;
}

}  // namespace util
