#pragma once
#include <QWidget>

#include "Aria2Net/Protocol/Protocol.h"
#include "DownloadManager.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class DownloadWidget;
}
QT_END_NAMESPACE

class DownloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadWidget(QWidget* parent = nullptr);
    ~DownloadWidget() override;
    void addTaskItem(const std::list<std::string>& videos, const std::list<std::string>& audios, const std::string& fileName);

private:
    Ui::DownloadWidget* ui;
    DownloadManager* m_downloadManager;
};
