#pragma once
#include "Qrcodegen.h"
#include <QImage>
#include <QtCore/QObject>

class QrCodeGenerator final : public QObject
{
public:
    explicit QrCodeGenerator(QObject* parent = nullptr);

    QImage generateQR(const QString& data);

private:
    QString toSvgString(const qrcodegen::QrCode& qr, quint16 border) const;
    QImage ToImage(const qrcodegen::QrCode& qrCode, quint16 border, const quint16 size) const;
};
