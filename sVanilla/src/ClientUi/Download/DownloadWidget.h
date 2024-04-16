#pragma once
#include <QWidget>

#include "Aria2Net/Protocol/Protocol.h"
#include "DownloadManager.h"

namespace download
{
struct ResourseInfo;
}
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
    ~DownloadWidget();

    void addTaskItem(const std::list<std::string>& videos, const std::list<std::string>& audios, const std::string& fileName);
    void addDownloadTask(const download::ResourseInfo& info);

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::DownloadWidget* ui;
    DownloadManager* m_downloadManager;
};
