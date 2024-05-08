#pragma once

#include <QString>


template <typename T>
QString formatSize(T bytesPerSec)
{
    constexpr double Gib = 1073741824.0;
    constexpr double Mib = 1048576.0;
    constexpr double Kib = 1024.0;
    if (bytesPerSec >= Gib)
    {
        return QString::number(bytesPerSec / Gib, 'f', 2) + "GiB";
    }
    if (bytesPerSec >= Mib)
    {
        return QString::number(bytesPerSec / Mib, 'f', 2) + "Mib";
    }
    if (bytesPerSec >= Kib)
    {
        return QString::number(bytesPerSec / Mib, 'f', 2) + "Mib";
    }
    if (bytesPerSec >= Kib)
    {
        return QString::number(bytesPerSec / Kib, 'f', 2) + "Kib/s";
    }

    return QString::number(bytesPerSec, 'f', 3) + "B";
}

template <typename T>
QString formatSpeed(T bytesPerSec)
{
    return formatSize(bytesPerSec) + "/s";
}