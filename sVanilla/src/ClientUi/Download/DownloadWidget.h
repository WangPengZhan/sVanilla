#pragma once
#include <QWidget>
#include "Aria2Net/Protocol/Protocol.h"

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
    void addTaskItem(const std::string& gid);

    void updateItem(const std::shared_ptr<aria2net::AriaTellStatus>& status);

private:
    Ui::DownloadWidget* ui;
};
