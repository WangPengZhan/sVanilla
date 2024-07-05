#include "QrCodeGenerator.h"
#include <QDebug>
#include <QPainter>
#include <QSvgRenderer>
#include <QTextStream>

QrCodeGenerator::QrCodeGenerator(QObject* parent)
    : QObject(parent)
{
}
QImage QrCodeGenerator::generateQR(const QString& data)
{
    const auto b = data.toUtf8();
    constexpr auto errorCorrection = qrcodegen::QrCode::Ecc::MEDIUM;
    const auto qrCode = qrcodegen::QrCode::encodeText(b.constData(), errorCorrection);
    return ToImage(qrCode, 4, 256);
}
QString QrCodeGenerator::toSvgString(const qrcodegen::QrCode& qr, quint16 border) const
{
    QString str;
    QTextStream sb(&str);

    sb << R"(<?xml version="1.0" encoding="UTF-8"?>)" << R"(<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">)"
       << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1" viewBox="0 0 )" << (qr.getSize() + border * 2) << " " << (qr.getSize() + border * 2)
       << R"(" stroke="none"><rect width="100%" height="100%" fill="#FFFFFF"/><path d=")";

    for (int y = 0; y < qr.getSize(); y++)
    {
        for (int x = 0; x < qr.getSize(); x++)
        {
            if (qr.getModule(x, y))
            {
                sb << (x == 0 && y == 0 ? "" : " ") << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
            }
        }
    }

    sb << R"(" fill="#000000"/></svg>)";
    return str;
}
QImage QrCodeGenerator::ToImage(const qrcodegen::QrCode& qrCode, quint16 border, const quint16 size) const
{
    const QString svg = toSvgString(qrCode, border);
    QSvgRenderer render(svg.toUtf8());
    QImage image(size, size, QImage::Format_Mono);
    image.fill(Qt::white);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    render.render(&painter);
    return image;
}