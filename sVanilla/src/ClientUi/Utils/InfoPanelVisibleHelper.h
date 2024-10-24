#pragma once

#include <QSplitter>

template <typename InfoWidget>
void setInfoPanelVisible(InfoWidget* infoWidget, QSplitter* splitter, int currentRow, int& previousRow)
{
    if (splitter == nullptr)
    {
        return;
    }
    const auto setSizes = [infoWidget, splitter](const bool isVisible) {
        const QList<int> sizes{splitter->height(), isVisible ? infoWidget->height() : 0};
        splitter->setSizes(sizes);
    };

    if (infoWidget->isVisible() && previousRow == currentRow)
    {
        infoWidget->hide();
        setSizes(false);
    }
    else
    {
        infoWidget->setVisible(true);
        infoWidget->adjustSize();
        setSizes(true);
    }
    previousRow = currentRow;
}
