#pragma once
#include <QString>
#include <QWebEngineView>

void initializeWebEngine(const QString& cache_path, const QString& storage_path);
void initDefaultWebSettings(QWebEngineView* view);
