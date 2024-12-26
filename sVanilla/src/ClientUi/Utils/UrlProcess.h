#pragma once

#include <QString>

namespace util
{

static constexpr char specialChars[] = "?/\"\\<>*|:•";

QString removeSpecialChar(const QString& path);
std::string removeSpecialChar(const std::string& path);

}  // namespace util
