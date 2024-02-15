#pragma once
#include <QColor>
#include <QPalette>
#include "Theme/Theme.h"

namespace Theme
{
namespace Color
{

QPalette standardPalette();
void setDarkMode(bool dark);
QColor WindowBackground();
QColor BaseBackground();
QColor Text();
namespace Button
{
QColor Outline();
QColor PressedOutline();
QColor PressedBackground();
QColor HoverBackground();
QColor Background();
}
};  // namespace ColorRepository


}
