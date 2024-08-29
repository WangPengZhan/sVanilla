#include <QPainter>
#include <QPainterPath>
#include <QEvent>
#include <QMouseEvent>

#include "LoginBubble.h"

#include <QBuffer>
#include <QImageReader>
#include <QtWidgets/QGridLayout>
#include <utility>

#include "Login/login.h"

LoginBubble::LoginBubble(std::shared_ptr<AbstractLogin> loginer, QWidget* parent)
    : QWidget(parent)
      , m_loginer(std::move(loginer))

{
    setUi();
}


void LoginBubble::showCenter(const QRect& rect)
{
    qApp->installEventFilter(this);
    movePosition(rect);
    show();
    setFocus();
    activateWindow();
}

void LoginBubble::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.setOpacity(0.9);

    auto bubblePath = QPainterPath();
    bubblePath.addRoundedRect(rect(), 10, 10);
    painter.drawPath(bubblePath);
}


bool LoginBubble::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        const auto mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (!geometry().contains(mouseEvent->pos()))
        {
            close();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void LoginBubble::hideEvent(QHideEvent* event)
{
    qApp->removeEventFilter(this);
    QWidget::hideEvent(event);
}

void LoginBubble::setUi()
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    const auto mainLayout = new QGridLayout(this);
    setLayout(mainLayout);
    m_background = new QLabel(this);
    auto svgContext = m_loginer->resource(AbstractLogin::Background);
    auto pixmap = binToImage(svgContext, m_background->size());
    m_background->setPixmap(pixmap);
    mainLayout->addWidget(m_background, 0, 0, 4, 4);

    m_orc = new QLabel(this);
    mainLayout->addWidget(m_orc, 2, 2, 2, 2);
    svgContext = m_loginer->resource(AbstractLogin::Confirmed);
    pixmap = binToImage(svgContext, m_orc->size());
    m_orc->setPixmap(pixmap);
    m_orc->raise();
}

void LoginBubble::movePosition(const QRect& pos)
{
    const auto center = pos.center();
    const auto w = pos.width() * m_proportion;
    const auto h = pos.height() * m_proportion;
    resize(w, h);
    const auto topLeft = QPoint(center.x() - w / 2, center.y() - height() / 2);
    move(topLeft);
}

QPixmap LoginBubble::binToImage(const std::vector<uint8_t>& bin, QSize size)
{
    QBuffer buffer;
    buffer.setData(reinterpret_cast<const char*>(bin.data()), bin.size());
    buffer.open(QIODevice::ReadOnly);

    QImageReader render(&buffer);
    QImage image = render.read();
    image = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap pixmap = QPixmap::fromImage(image);
    return pixmap;
}