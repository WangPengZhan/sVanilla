#include "Color.h"
#include <QToolTip>
#include <QApplication>

QPalette Theme::Color::standardPalette()
{
    QPalette palette;
    palette.setColor(QPalette::Window, WindowBackground());
    palette.setColor(QPalette::Base, BaseBackground());
    palette.setColor(QPalette::Text, Text());
    palette.setColor(QPalette::Button, Button::PressedBackground());

    // Text color on buttons
    palette.setColor(QPalette::ButtonText, Text());
    palette.setColor(QPalette::ToolTipText, Text());

    QToolTip::setPalette(palette);
    return palette;
}
void Theme::Color::setDarkMode(bool dark)
{
    s_darkMode = dark;
    qApp->setPalette(standardPalette());
}
QColor Theme::Color::WindowBackground()
{
    return s_darkMode ? QColor(0x18, 0x21, 0x29)  // dark blue
                        :
                        QColor(0xef, 0xf0, 0xf1);
}
QColor Theme::Color::BaseBackground()
{
    return s_darkMode ? QColor(0x0f, 0x0f, 0x0f)  // almost black
                        :
                        QColor(0xfb, 0xfb, 0xfb);  // almost white
}
QColor Theme::Color::Text()
{
    return s_darkMode ? QColor(0xa5, 0xa5, 0xa5) : QColor(0x25, 0x25, 0x25);

}
QColor Theme::Color::Button::Outline()
{
    return s_darkMode ? QColor(0x59, 0x51, 0x5f) : QColor(0x9f, 0x95, 0xa3);
}
QColor Theme::Color::Button::PressedOutline()
{
    return {0x32, 0x2d, 0x35};
}
QColor Theme::Color::Button::PressedBackground()
{
    return s_darkMode ? QColor(0x2d, 0x3a, 0x48)  // dark blue
                        :
                        QColor(0x99, 0x99, 0x99);
}
QColor Theme::Color::Button::HoverBackground()
{
    QColor color = PressedBackground();
    color.setAlphaF(0.2);
    return color;
}
QColor Theme::Color::Button::Background()
{
    return s_darkMode ? QColor(0x21, 0x1f, 0x22, 0xa7) : QColor(0xf5, 0xf4, 0xf5);
}